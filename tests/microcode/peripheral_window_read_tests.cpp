#include "test_harness.h"

#include <filesystem>
#include <fstream>
#include <limits.h>
#include <string>
#include <unistd.h>
#include <vector>

#include "memory128k.h"
#include "mon560x192.h"
#include "pixel.h"
#include "drive5_25.h"

namespace {

class ScopedCwd {
public:
    explicit ScopedCwd(const std::string& path)
        : active_(false)
    {
        char buf[PATH_MAX];
        if( getcwd(buf, sizeof(buf))!=NULL ) {
            oldCwd_ = buf;
            if( chdir(path.c_str())==0 )
                active_ = true;
        }
    }

    ~ScopedCwd()
    {
        if( active_ )
            chdir(oldCwd_.c_str());
    }

    bool active() const
    {
        return active_;
    }

private:
    bool active_;
    std::string oldCwd_;
};

} // namespace

static std::filesystem::path writeGeneratedSlotRom()
{
    const std::filesystem::path path =
            std::filesystem::temp_directory_path() /
            ("ever2e_slot6_window_rom_" + std::to_string(getpid()) + ".rom");
    std::vector<Uint8> rom(256, 0xea);
    rom[0x00] = 0xa2;
    rom[0x01] = 0x20;
    rom[0x02] = 0xa0;
    rom[0xc4] = 0xa4;
    rom[0xd0] = 0x59;
    rom[0xf0] = 0x3d;
    std::ofstream out(path, std::ios::out | std::ios::binary | std::ios::trunc);
    E2TEST_ASSERT_TRUE(out.is_open());
    out.write(reinterpret_cast<const char*>(rom.data()), (std::streamsize)rom.size());
    return path;
}

E2TEST_CASE(peripheralWindowReadFallsBackToFloatingBusWhenSlotCardMissing)
{
    ScopedCwd cwd("release");
    E2TEST_ASSERT_TRUE(cwd.active());

    PixelSurface surface(560, 384, 32);
    Memory128k memory;
    Monitor560x192 monitor(&surface, &memory);
    memory.putPeripheral(NULL, &monitor, NULL, NULL);

    // Make the floating bus deterministic at cycle 0.
    monitor.commitSwitches();
    monitor.resetAll();
    const Uint16 floatingAddr = monitor.getFloatingBusAddress();
    memory.putMem(floatingAddr, 0x5A);
    monitor.cycle();
    const Uint8 expectedFloating = monitor.getLastRead();

    memory.resetSwitch(Memory128k::_INTCXROM);
    const Uint8 readC105 = memory.getMem(0xC105);
    E2TEST_ASSERT_EQ(static_cast<int>(expectedFloating), static_cast<int>(readC105));
}

E2TEST_CASE(slot3ReadSetsIntc8romAndFallsBackToFloatingBusWhenSlot3Missing)
{
    ScopedCwd cwd("release");
    E2TEST_ASSERT_TRUE(cwd.active());

    PixelSurface surface(560, 384, 32);
    Memory128k memory;
    Monitor560x192 monitor(&surface, &memory);
    memory.putPeripheral(NULL, &monitor, NULL, NULL);

    monitor.commitSwitches();
    monitor.resetAll();
    const Uint16 floatingAddr = monitor.getFloatingBusAddress();
    memory.putMem(floatingAddr, 0xA6);
    monitor.cycle();
    const Uint8 expectedFloating = monitor.getLastRead();

    // Force peripheral-slot path for C3xx (no internal C3 ROM path).
    memory.setSwitch(Memory128k::_SLOTC3ROM);
    memory.resetSwitch(Memory128k::_INTCXROM);
    memory.resetSwitch(Memory128k::_INTC8ROM);

    const Uint8 readC307 = memory.getMem(0xC307);
    E2TEST_ASSERT_EQ(static_cast<int>(expectedFloating), static_cast<int>(readC307));
    E2TEST_ASSERT_EQ(0, memory.getSwitch(Memory128k::_INTC8ROM) ? 1 : 0);

    // With SLOTC3ROM reset, C3xx should set INTC8ROM and read internal ROM.
    memory.resetSwitch(Memory128k::_SLOTC3ROM);
    memory.resetSwitch(Memory128k::_INTC8ROM);
    (void) memory.getMem(0xC307);
    E2TEST_ASSERT_EQ(1, memory.getSwitch(Memory128k::_INTC8ROM) ? 1 : 0);
}

E2TEST_CASE(expansionWindowReadsUseLastSelectedSlotRom)
{
    ScopedCwd cwd("release");
    E2TEST_ASSERT_TRUE(cwd.active());

    PixelSurface surface(560, 384, 32);
    Memory128k memory;
    Monitor560x192 monitor(&surface, &memory);
    memory.putPeripheral(NULL, &monitor, NULL, NULL);

    PeripheralCard16bit slot5;
    slot5.setSlotPattern(0xA5);
    memory.putSlot(5, &slot5);

    memory.resetSwitch(Memory128k::_INTCXROM);
    (void) memory.getMem(0xC500); // Latch slot 5 into expansion window.
    const Uint8 readC800 = memory.getMem(0xC800);
    E2TEST_ASSERT_EQ(0xA5, static_cast<int>(readC800));
}

E2TEST_CASE(expansionWindowSelectionSwitchesWithNewSlotAccess)
{
    ScopedCwd cwd("release");
    E2TEST_ASSERT_TRUE(cwd.active());

    PixelSurface surface(560, 384, 32);
    Memory128k memory;
    Monitor560x192 monitor(&surface, &memory);
    memory.putPeripheral(NULL, &monitor, NULL, NULL);

    PeripheralCard16bit slot1;
    PeripheralCard16bit slot5;
    slot1.setSlotPattern(0x11);
    slot5.setSlotPattern(0x55);
    memory.putSlot(1, &slot1);
    memory.putSlot(5, &slot5);

    memory.resetSwitch(Memory128k::_INTCXROM);

    (void) memory.getMem(0xC500);
    E2TEST_ASSERT_EQ(0x55, static_cast<int>(memory.getMem(0xC8AB)));

    (void) memory.getMem(0xC100);
    E2TEST_ASSERT_EQ(0x11, static_cast<int>(memory.getMem(0xC8AB)));
}

E2TEST_CASE(expansionWindowResetByCfffFallsBackToInternalRom)
{
    ScopedCwd cwd("release");
    E2TEST_ASSERT_TRUE(cwd.active());

    PixelSurface surface(560, 384, 32);
    Memory128k memory;
    Monitor560x192 monitor(&surface, &memory);
    memory.putPeripheral(NULL, &monitor, NULL, NULL);

    PeripheralCard16bit slot5;
    slot5.setSlotPattern(0xA5);
    memory.putSlot(5, &slot5);

    memory.resetSwitch(Memory128k::_INTCXROM);
    (void) memory.getMem(0xC500); // Latch slot 5.
    const Uint8 beforeReset = memory.getMem(0xC800);
    E2TEST_ASSERT_EQ(0xA5, static_cast<int>(beforeReset));

    memory.setSwitch(Memory128k::_INTCXROM);
    const Uint8 expectedInternal = memory.getMem(0xC800);
    memory.resetSwitch(Memory128k::_INTCXROM);

    (void) memory.getMem(0xCFFF); // Reset expansion-slot window.
    const Uint8 afterReset = memory.getMem(0xC800);
    E2TEST_ASSERT_EQ(static_cast<int>(expectedInternal), static_cast<int>(afterReset));
}

E2TEST_CASE(floppy525ControllerExposesDiskIiSlotRom)
{
    ScopedCwd cwd("release");
    E2TEST_ASSERT_TRUE(cwd.active());

    PixelSurface surface(560, 384, 32);
    Memory128k memory;
    Monitor560x192 monitor(&surface, &memory);
    memory.putPeripheral(NULL, &monitor, NULL, NULL);

    const std::filesystem::path romPath = writeGeneratedSlotRom();
    Floppy525Controller slot6(6, "", "", romPath.string());
    memory.putSlot(6, &slot6);
    memory.resetSwitch(Memory128k::_INTCXROM);

    E2TEST_ASSERT_EQ(0xA2, static_cast<int>(memory.getMem(0xC600)));
    E2TEST_ASSERT_EQ(0x20, static_cast<int>(memory.getMem(0xC601)));
    E2TEST_ASSERT_EQ(0xA0, static_cast<int>(memory.getMem(0xC602)));
    E2TEST_ASSERT_EQ(0xA4, static_cast<int>(memory.getMem(0xC6C4)));
    E2TEST_ASSERT_EQ(0x59, static_cast<int>(memory.getMem(0xC6D0)));
    E2TEST_ASSERT_EQ(0x3D, static_cast<int>(memory.getMem(0xC6F0)));

    // Slot I/O switches should dispatch without corrupting the Cn00 ROM window.
    memory.putMem(0xC0E9, 0x00);
    E2TEST_ASSERT_EQ(0xA2, static_cast<int>(memory.getMem(0xC600)));

    std::filesystem::remove(romPath);
}
