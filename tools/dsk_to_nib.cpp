#include <algorithm>
#include <array>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

constexpr int kTracks = 35;
constexpr int kSectors = 16;
constexpr int kSectorSize = 256;
constexpr int kDskSize = kTracks * kSectors * kSectorSize;      // 143360
constexpr int kNibTrackSize = 6656;                              // 0x1A00
constexpr int kNibSectorSize = 416;
constexpr int kNibSize = kTracks * kNibTrackSize;                // 232960

constexpr std::array<uint8_t, 16> kDosOrder = {
    0, 7, 14, 6, 13, 5, 12, 4, 11, 3, 10, 2, 9, 1, 8, 15
};

constexpr std::array<uint8_t, 64> kGcr62 = {
    0x96, 0x97, 0x9A, 0x9B, 0x9D, 0x9E, 0x9F, 0xA6,
    0xA7, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 0xB2, 0xB3,
    0xB4, 0xB5, 0xB6, 0xB7, 0xB9, 0xBA, 0xBB, 0xBC,
    0xBD, 0xBE, 0xBF, 0xCB, 0xCD, 0xCE, 0xCF, 0xD3,
    0xD6, 0xD7, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE,
    0xDF, 0xE5, 0xE6, 0xE7, 0xE9, 0xEA, 0xEB, 0xEC,
    0xED, 0xEE, 0xEF, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6,
    0xF7, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF
};

void usage() {
    std::cerr
        << "Usage: dsk_to_nib <in.dsk> <out.nib> [--volume N]\n"
        << "  --volume N     DOS volume byte (0-255), default 254\n"
        << "  --template P   use an existing 35-track NIB layout as timing template\n";
}

std::vector<uint8_t> readAll(const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) {
        throw std::runtime_error("cannot open input: " + path);
    }
    in.seekg(0, std::ios::end);
    const std::streamoff len = in.tellg();
    in.seekg(0, std::ios::beg);
    if (len < 0) {
        throw std::runtime_error("failed to read input length: " + path);
    }
    std::vector<uint8_t> data(static_cast<size_t>(len));
    if (!data.empty()) {
        in.read(reinterpret_cast<char*>(data.data()), static_cast<std::streamsize>(data.size()));
        if (!in) {
            throw std::runtime_error("failed to read input: " + path);
        }
    }
    return data;
}

void writeAll(const std::string& path, const std::vector<uint8_t>& data) {
    std::ofstream out(path, std::ios::binary);
    if (!out) {
        throw std::runtime_error("cannot open output: " + path);
    }
    out.write(reinterpret_cast<const char*>(data.data()), static_cast<std::streamsize>(data.size()));
    if (!out) {
        throw std::runtime_error("failed to write output: " + path);
    }
}

void append4and4(std::vector<uint8_t>& dst, uint8_t v) {
    const uint8_t a = static_cast<uint8_t>(((v >> 1) & 0x55) | 0xAA);
    const uint8_t b = static_cast<uint8_t>((v & 0x55) | 0xAA);
    dst.push_back(a);
    dst.push_back(b);
}

uint8_t decode4and4(uint8_t a, uint8_t b) {
    return static_cast<uint8_t>(((a & 0x55) << 1) | (b & 0x55));
}

std::vector<uint8_t> encodeSector62(const uint8_t* src256) {
    std::vector<uint8_t> out;
    out.reserve(343);

    // Match the Disk II ROM decode order used by ever2e-jvm:
    // - 86 aux bytes consumed in descending index order (85..0)
    // - 256 main bytes consumed in ascending order (0..255)
    // - rolling XOR is applied across that stream, then checksum byte emitted.
    std::array<uint8_t, 256> main6{};
    std::array<uint8_t, 86> aux2{};
    std::array<uint8_t, 86> occ{};

    for (int i = 0; i < 256; ++i) {
        main6[static_cast<size_t>(i)] = static_cast<uint8_t>((src256[i] >> 2) & 0x3F);
    }

    for (int y = 0; y < 256; ++y) {
        const int j = 85 - (y % 86);
        const uint8_t pair = static_cast<uint8_t>(((src256[y] & 0x02) >> 1) | ((src256[y] & 0x01) << 1));
        const uint8_t shift = static_cast<uint8_t>(occ[static_cast<size_t>(j)] * 2);
        aux2[static_cast<size_t>(j)] = static_cast<uint8_t>(aux2[static_cast<size_t>(j)] | static_cast<uint8_t>(pair << shift));
        occ[static_cast<size_t>(j)] = static_cast<uint8_t>(occ[static_cast<size_t>(j)] + 1);
    }

    uint8_t prev = 0;
    for (int i = 85; i >= 0; --i) {
        const uint8_t v = static_cast<uint8_t>(aux2[static_cast<size_t>(i)] & 0x3F);
        out.push_back(kGcr62[static_cast<size_t>((prev ^ v) & 0x3F)]);
        prev = v;
    }

    for (int i = 0; i < 256; ++i) {
        const uint8_t v = static_cast<uint8_t>(main6[static_cast<size_t>(i)] & 0x3F);
        out.push_back(kGcr62[static_cast<size_t>((prev ^ v) & 0x3F)]);
        prev = v;
    }

    out.push_back(kGcr62[static_cast<size_t>(prev & 0x3F)]);
    return out;
}

std::vector<uint8_t> dskToNib(const std::vector<uint8_t>& dsk, uint8_t volume) {
    if (static_cast<int>(dsk.size()) != kDskSize) {
        throw std::runtime_error("input size is not 143360 bytes (.dsk expected)");
    }

    // DOS 3.3 formatter-style sync cadence (from RWTS/formatter listings):
    // - 128 sync bytes before sector 0 address field
    // - ~20 sync bytes between sectors (about 50 raw bit cells)
    constexpr int kSyncFirst = 128;
    constexpr int kSyncBetween = 20;

    std::vector<uint8_t> nib;
    nib.reserve(kNibSize);

    for (int track = 0; track < kTracks; ++track) {
        const size_t trackStart = nib.size();

        for (int physicalSector = 0; physicalSector < kSectors; ++physicalSector) {
            nib.insert(nib.end(), (physicalSector == 0) ? kSyncFirst : kSyncBetween, 0xFF);

            // Address field
            nib.push_back(0xD5);
            nib.push_back(0xAA);
            nib.push_back(0x96);
            append4and4(nib, volume);
            append4and4(nib, static_cast<uint8_t>(track));
            append4and4(nib, static_cast<uint8_t>(physicalSector));
            append4and4(nib, static_cast<uint8_t>(volume ^ track ^ physicalSector));
            nib.push_back(0xDE);
            nib.push_back(0xAA);
            nib.push_back(0xEB);

            // ~5 self-sync bytes between address and data fields.
            nib.insert(nib.end(), 5, 0xFF);

            // Data prolog + 6-and-2 payload + data epilog
            nib.push_back(0xD5);
            nib.push_back(0xAA);
            nib.push_back(0xAD);

            const int logicalSector = kDosOrder[static_cast<size_t>(physicalSector)];
            const size_t dskOff = static_cast<size_t>((track * kSectors + logicalSector) * kSectorSize);
            const auto enc = encodeSector62(dsk.data() + dskOff);
            nib.insert(nib.end(), enc.begin(), enc.end());

            nib.push_back(0xDE);
            nib.push_back(0xAA);
            nib.push_back(0xEB);
        }

        // Fill remaining bytes in the track with sync bytes to exact NIB track length.
        const size_t producedTrack = nib.size() - trackStart;
        if (producedTrack > static_cast<size_t>(kNibTrackSize)) {
            throw std::runtime_error("internal error: track encoding overflow");
        }
        nib.insert(nib.end(), static_cast<size_t>(kNibTrackSize) - producedTrack, 0xFF);
    }

    if (nib.size() != static_cast<size_t>(kNibSize)) {
        throw std::runtime_error("internal error: output size mismatch");
    }

    return nib;
}

std::vector<uint8_t> dskToNibWithTemplate(const std::vector<uint8_t>& dsk, const std::vector<uint8_t>& tmpl) {
    if (static_cast<int>(dsk.size()) != kDskSize) {
        throw std::runtime_error("input size is not 143360 bytes (.dsk expected)");
    }
    if (static_cast<int>(tmpl.size()) < kNibSize) {
        throw std::runtime_error("template nib must be at least 232960 bytes (35 tracks)");
    }

    std::vector<uint8_t> out(tmpl.begin(), tmpl.begin() + kNibSize);

    for (int track = 0; track < kTracks; ++track) {
        const size_t base = static_cast<size_t>(track) * kNibTrackSize;
        std::vector<int> addrPos;
        addrPos.reserve(16);
        for (int i = 0; i < kNibTrackSize; ++i) {
            const int i1 = (i + 1) % kNibTrackSize;
            const int i2 = (i + 2) % kNibTrackSize;
            const size_t p0 = base + static_cast<size_t>(i);
            const size_t p1 = base + static_cast<size_t>(i1);
            const size_t p2 = base + static_cast<size_t>(i2);
            if (out[p0] == 0xD5 && out[p1] == 0xAA && out[p2] == 0x96) {
                addrPos.push_back(i);
            }
        }
        int patched = 0;
        for (int i : addrPos) {
            const int a7 = (i + 7) % kNibTrackSize;
            const int a8 = (i + 8) % kNibTrackSize;
            const uint8_t sectorId = decode4and4(
                out[base + static_cast<size_t>(a7)],
                out[base + static_cast<size_t>(a8)]);
            if (sectorId >= kSectors) {
                continue;
            }

            // Find following data prolog near this address mark with track wrap.
            int dataRel = -1;
            for (int d = 10; d <= 128; ++d) {
                const int j = (i + d) % kNibTrackSize;
                const int j1 = (j + 1) % kNibTrackSize;
                const int j2 = (j + 2) % kNibTrackSize;
                if (out[base + static_cast<size_t>(j)] == 0xD5 &&
                    out[base + static_cast<size_t>(j1)] == 0xAA &&
                    out[base + static_cast<size_t>(j2)] == 0xAD) {
                    dataRel = j;
                    break;
                }
            }
            if (dataRel < 0) {
                continue;
            }

            const int logicalSector = kDosOrder[static_cast<size_t>(sectorId)];
            const size_t dskOff = static_cast<size_t>((track * kSectors + logicalSector) * kSectorSize);
            const auto enc = encodeSector62(dsk.data() + dskOff);
            for (size_t k = 0; k < enc.size(); ++k) {
                const size_t w = base + static_cast<size_t>((dataRel + 3 + static_cast<int>(k)) % kNibTrackSize);
                out[w] = enc[k];
            }
            patched++;
        }
        if (patched != 16) {
            throw std::runtime_error("template track " + std::to_string(track) + " did not yield 16 address/data pairs");
        }
    }

    return out;
}

}  // namespace

int main(int argc, char** argv) {
    try {
        if (argc < 3) {
            usage();
            return 2;
        }

        std::string inPath = argv[1];
        std::string outPath = argv[2];
        int volume = 254;
        std::optional<std::string> templatePath;

        for (int i = 3; i < argc; ++i) {
            const std::string arg = argv[i];
            if (arg == "--volume") {
                if (i + 1 >= argc) {
                    throw std::runtime_error("missing value for --volume");
                }
                volume = std::stoi(argv[++i]);
                if (volume < 0 || volume > 255) {
                    throw std::runtime_error("--volume must be 0..255");
                }
            } else if (arg == "--template") {
                if (i + 1 >= argc) {
                    throw std::runtime_error("missing value for --template");
                }
                templatePath = argv[++i];
            } else {
                throw std::runtime_error("unknown option: " + arg);
            }
        }

        const auto dsk = readAll(inPath);
        std::vector<uint8_t> nib;
        if (templatePath.has_value()) {
            const auto tmpl = readAll(*templatePath);
            nib = dskToNibWithTemplate(dsk, tmpl);
        } else {
            nib = dskToNib(dsk, static_cast<uint8_t>(volume));
        }
        writeAll(outPath, nib);

        std::cout << "wrote " << outPath << " (" << nib.size() << " bytes) from " << inPath << "\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "dsk_to_nib: " << e.what() << "\n";
        return 1;
    }
}
