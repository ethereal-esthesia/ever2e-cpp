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
const int kSlotRomSize = 256;
const int kDriveOffDiskByteDelay = (0x40000 >> 3) + 2;

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

void writeFilledNib(const std::filesystem::path& path, Uint8 fill)
{
    std::vector<Uint8> image(kTrackTotal * kTrackBytes, fill);
    std::ofstream out(path, std::ios::out | std::ios::binary | std::ios::trunc);
    E2TEST_ASSERT_TRUE(out.is_open());
    out.write(reinterpret_cast<const char*>(image.data()), (std::streamsize)image.size());
}

void writeTestSlotRom(const std::filesystem::path& path)
{
    std::vector<Uint8> rom(kSlotRomSize, 0xea);
    rom[0] = 0xa2;
    rom[1] = 0x20;
    std::ofstream out(path, std::ios::out | std::ios::binary | std::ios::trunc);
    E2TEST_ASSERT_TRUE(out.is_open());
    out.write(reinterpret_cast<const char*>(rom.data()), (std::streamsize)rom.size());
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

E2TEST_CASE(floppy525ControllerRejectsMissingSlotRom)
{
    bool rejected = false;
    try {
        Floppy525Controller controller(6, "", "", "");
    }
    catch( const std::exception& e ) {
        rejected = std::string(e.what()).find("Disk II slot ROM is required") != std::string::npos;
    }
    E2TEST_ASSERT_TRUE(rejected);
}

E2TEST_CASE(floppy525ControllerWritesDeterministicRandomBytes)
{
    ScopedEnvVar cyclePeriod("EVER2E_DISKII_CYCLE_PERIOD",
            std::to_string(kDiskByteCyclePeriod));
    const std::filesystem::path path =
            std::filesystem::temp_directory_path() /
            ("ever2e_floppy525_random_" + std::to_string(getpid()) + ".nib");
    const std::filesystem::path romPath =
            std::filesystem::temp_directory_path() /
            ("ever2e_floppy525_rom_" + std::to_string(getpid()) + ".rom");

    writeBlankNib(path);
    writeTestSlotRom(romPath);
    const std::vector<Uint8> payload = deterministicPayload();

    {
        Floppy525Controller controller(6, path.string(), "", romPath.string());
        controller.putMem16b(0x09, 0x00); // drive on
        controller.putMem16b(0x0f, 0x00); // write mode

        for( Uint8 value : payload ) {
            controller.putMem16b(0x0d, value);
            controller.putMem16b(0x0c, 0x00);
            advanceDiskByte(controller);
        }

        controller.putMem16b(0x0e, 0x00); // read mode
        controller.putMem16b(0x08, 0x00); // drive off
        for( int i = 0; i<kDriveOffDiskByteDelay; i++ )
            advanceDiskByte(controller);
    }

    const std::vector<Uint8> image = readFile(path);
    E2TEST_ASSERT_EQ(kTrackTotal * kTrackBytes, static_cast<int>(image.size()));
    for( size_t i = 0; i<payload.size(); i++ )
        E2TEST_ASSERT_EQ(static_cast<int>(payload[i]),
                static_cast<int>(image[kFirstWrittenByteOffset + i]));

    std::filesystem::remove(path);
    std::filesystem::remove(romPath);
}

E2TEST_CASE(floppy525ControllerPendingWriteStaysWithSelectedDrive)
{
    ScopedEnvVar cyclePeriod("EVER2E_DISKII_CYCLE_PERIOD",
            std::to_string(kDiskByteCyclePeriod));
    const std::filesystem::path drive1Path =
            std::filesystem::temp_directory_path() /
            ("ever2e_floppy525_drive1_" + std::to_string(getpid()) + ".nib");
    const std::filesystem::path drive2Path =
            std::filesystem::temp_directory_path() /
            ("ever2e_floppy525_drive2_" + std::to_string(getpid()) + ".nib");
    const std::filesystem::path romPath =
            std::filesystem::temp_directory_path() /
            ("ever2e_floppy525_rom_switch_" + std::to_string(getpid()) + ".rom");

    writeFilledNib(drive1Path, 0x11);
    writeFilledNib(drive2Path, 0xff);
    writeTestSlotRom(romPath);

    {
        Floppy525Controller controller(6, drive1Path.string(), drive2Path.string(), romPath.string());
        controller.putMem16b(0x0b, 0x00); // select drive 2
        controller.putMem16b(0x09, 0x00); // drive on
        controller.putMem16b(0x0f, 0x00); // write mode
        controller.putMem16b(0x0d, 0x5a);
        controller.putMem16b(0x0c, 0x00); // queue write for drive 2
        controller.putMem16b(0x0a, 0x00); // switch back to drive 1 before the byte lands
        advanceDiskByte(controller);

        controller.putMem16b(0x0b, 0x00); // reselect drive 2 so its dirty image is flushed
        controller.putMem16b(0x0e, 0x00); // read mode
        controller.putMem16b(0x08, 0x00); // drive off
        for( int i = 0; i<kDriveOffDiskByteDelay; i++ )
            advanceDiskByte(controller);
    }

    const std::vector<Uint8> drive1 = readFile(drive1Path);
    const std::vector<Uint8> drive2 = readFile(drive2Path);
    E2TEST_ASSERT_EQ(kTrackTotal * kTrackBytes, static_cast<int>(drive1.size()));
    E2TEST_ASSERT_EQ(kTrackTotal * kTrackBytes, static_cast<int>(drive2.size()));
    E2TEST_ASSERT_EQ(0x11, static_cast<int>(drive1[kFirstWrittenByteOffset]));
    E2TEST_ASSERT_EQ(0x5a, static_cast<int>(drive2[kFirstWrittenByteOffset]));

    std::filesystem::remove(drive1Path);
    std::filesystem::remove(drive2Path);
    std::filesystem::remove(romPath);
}

E2TEST_CASE(floppy525ControllerDriveSelectCancelsPreviousIdleSpinDown)
{
    ScopedEnvVar cyclePeriod("EVER2E_DISKII_CYCLE_PERIOD",
            std::to_string(kDiskByteCyclePeriod));
    const std::filesystem::path drive1Path =
            std::filesystem::temp_directory_path() /
            ("ever2e_floppy525_spindown_drive1_" + std::to_string(getpid()) + ".nib");
    const std::filesystem::path drive2Path =
            std::filesystem::temp_directory_path() /
            ("ever2e_floppy525_spindown_drive2_" + std::to_string(getpid()) + ".nib");
    const std::filesystem::path romPath =
            std::filesystem::temp_directory_path() /
            ("ever2e_floppy525_spindown_rom_" + std::to_string(getpid()) + ".rom");

    writeFilledNib(drive1Path, 0x11);
    writeFilledNib(drive2Path, 0xff);
    writeTestSlotRom(romPath);

    {
        Floppy525Controller controller(6, drive1Path.string(), drive2Path.string(), romPath.string());
        controller.putMem16b(0x09, 0x00); // drive 1 on
        controller.putMem16b(0x08, 0x00); // request delayed spin-down
        controller.putMem16b(0x0b, 0x00); // selecting drive 2 immediately stops drive 1

        for( int i = 0; i<kDriveOffDiskByteDelay; i++ )
            advanceDiskByte(controller);

        controller.putMem16b(0x0f, 0x00); // write mode; should still be on drive 2
        controller.putMem16b(0x0d, 0x6b);
        controller.putMem16b(0x0c, 0x00);
        advanceDiskByte(controller);

        controller.putMem16b(0x0e, 0x00); // read mode
        controller.putMem16b(0x08, 0x00); // drive off
        for( int i = 0; i<kDriveOffDiskByteDelay; i++ )
            advanceDiskByte(controller);
    }

    const std::vector<Uint8> drive1 = readFile(drive1Path);
    const std::vector<Uint8> drive2 = readFile(drive2Path);
    const int expectedWriteOffset =
            kInitialTrack * kTrackBytes + ((kDriveOffDiskByteDelay % kTrackBytes) + 1);
    E2TEST_ASSERT_EQ(kTrackTotal * kTrackBytes, static_cast<int>(drive1.size()));
    E2TEST_ASSERT_EQ(kTrackTotal * kTrackBytes, static_cast<int>(drive2.size()));
    E2TEST_ASSERT_EQ(0x11, static_cast<int>(drive1[expectedWriteOffset]));
    E2TEST_ASSERT_EQ(0x6b, static_cast<int>(drive2[expectedWriteOffset]));

    std::filesystem::remove(drive1Path);
    std::filesystem::remove(drive2Path);
    std::filesystem::remove(romPath);
}
