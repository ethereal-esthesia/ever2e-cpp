#include "test_harness.h"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <unistd.h>
#include <vector>

#include "drive5_25.h"

namespace {

const int kTrackTotal = 35;
const int kTrackBytes = 416 * 16;
const int kInitialTrack = 34;
const int kFirstWrittenByteOffset = kInitialTrack * kTrackBytes + 1;
const int kPayloadSize = 768;
const int kDiskByteCyclePeriod = 32;

std::vector<Uint8> deterministicPayload()
{
    std::vector<Uint8> payload;
    payload.reserve(kPayloadSize);
    Uint32 seed = 0x5eed1234;
    for( int i = 0; i<kPayloadSize; i++ ) {
        seed = (Uint32)(((Uint64)1103515245 * seed + 12345) & 0x7fffffff);
        payload.push_back((Uint8)((seed >> 8) & 0xff));
    }
    return payload;
}

void writeBlankNib(const std::filesystem::path& path)
{
    std::vector<Uint8> image(kTrackTotal * kTrackBytes, 0xff);
    std::ofstream out(path, std::ios::out | std::ios::binary | std::ios::trunc);
    E2TEST_ASSERT_TRUE(out.is_open());
    out.write(reinterpret_cast<const char*>(image.data()), (std::streamsize)image.size());
}

std::vector<Uint8> readFile(const std::filesystem::path& path)
{
    std::ifstream in(path, std::ios::in | std::ios::binary);
    E2TEST_ASSERT_TRUE(in.is_open());
    return std::vector<Uint8>(
            (std::istreambuf_iterator<char>(in)),
            std::istreambuf_iterator<char>());
}

void advanceDiskByte(Floppy525Controller& controller)
{
    for( int i = 0; i<kDiskByteCyclePeriod; i++ )
        controller.cycle();
}

class ScopedEnvVar {
public:
    ScopedEnvVar(const std::string& name, const std::string& value)
        : name(name),
          hadPreviousValue(false)
    {
        if( const char* existing = getenv(name.c_str()) ) {
            hadPreviousValue = true;
            previousValue = existing;
        }
        setenv(name.c_str(), value.c_str(), 1);
    }

    ~ScopedEnvVar()
    {
        if( hadPreviousValue )
            setenv(name.c_str(), previousValue.c_str(), 1);
        else
            unsetenv(name.c_str());
    }

private:
    std::string name;
    std::string previousValue;
    bool hadPreviousValue;
};

} // namespace

E2TEST_CASE(floppy525ControllerWritesDeterministicRandomBytes)
{
    ScopedEnvVar cyclePeriod("EVER2E_DISKII_CYCLE_PERIOD",
            std::to_string(kDiskByteCyclePeriod));
    const std::filesystem::path path =
            std::filesystem::temp_directory_path() /
            ("ever2e_floppy525_random_" + std::to_string(getpid()) + ".nib");

    writeBlankNib(path);
    const std::vector<Uint8> payload = deterministicPayload();

    {
        Floppy525Controller controller(6, path.string(), "");
        controller.putMem16b(0x09, 0x00); // drive on
        controller.putMem16b(0x0f, 0x00); // write mode

        for( Uint8 value : payload ) {
            controller.putMem16b(0x0d, value);
            controller.putMem16b(0x0c, 0x00);
            advanceDiskByte(controller);
        }

        controller.putMem16b(0x0e, 0x00); // read mode
        controller.putMem16b(0x08, 0x00); // drive off
        for( int i = 0; i<((0x40000 >> 3) + 2); i++ )
            advanceDiskByte(controller);
    }

    const std::vector<Uint8> image = readFile(path);
    E2TEST_ASSERT_EQ(kTrackTotal * kTrackBytes, static_cast<int>(image.size()));
    for( size_t i = 0; i<payload.size(); i++ )
        E2TEST_ASSERT_EQ(static_cast<int>(payload[i]),
                static_cast<int>(image[kFirstWrittenByteOffset + i]));

    std::filesystem::remove(path);
}
