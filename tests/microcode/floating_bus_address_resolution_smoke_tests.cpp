#include "test_harness.h"

#include <limits.h>
#include <sstream>
#include <string>
#include <unistd.h>

#include "memory128k.h"
#include "mon560x192.h"
#include "pixel.h"

namespace {

static const int kBobHorizontalScans = 65;
static const int kBobHorizontalVisible = 40;
static const int kBobHorizontalBlank = kBobHorizontalScans - kBobHorizontalVisible;
static const int kBobVerticalScans = 262;
static const int kBobVerticalVisible = 192;
static const int kBobVerticalBlank = kBobVerticalScans - kBobVerticalVisible;
static const int kTextBaseLo = 0x0400;
static const int kTextBaseHi = 0x07FF;
static const int kHiresPage1Lo = 0x2000;
static const int kHiresPage1Hi = 0x3FFF;
static const int kHiresPage2Lo = 0x4000;
static const int kHiresPage2Hi = 0x5FFF;

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

void setDisplaySwitches(Memory128k& memory, bool text, bool mixed, bool hires, bool page2, bool store80)
{
    if( text ) memory.setSwitch(Memory128k::_TEXT); else memory.resetSwitch(Memory128k::_TEXT);
    if( mixed ) memory.setSwitch(Memory128k::_MIXED); else memory.resetSwitch(Memory128k::_MIXED);
    if( hires ) memory.setSwitch(Memory128k::_HIRES); else memory.resetSwitch(Memory128k::_HIRES);
    if( page2 ) memory.setSwitch(Memory128k::_PAGE2); else memory.resetSwitch(Memory128k::_PAGE2);
    if( store80 ) memory.setSwitch(Memory128k::_80STORE); else memory.resetSwitch(Memory128k::_80STORE);
}

bool mixedUsesTextBand(int vScan)
{
    int vLine = vScan + kBobVerticalVisible;
    if( vLine>=256 )
        vLine -= kBobVerticalScans;
    const int vState = 0x100 + vLine;
    const int v2 = (vState >> 5) & 1;
    const int v4 = (vState >> 7) & 1;
    return (v4 & v2)!=0;
}

void assertRangeEveryXY(
        Monitor560x192& monitor,
        int loExpected,
        int hiExpected)
{
    const int totalCycles = kBobHorizontalScans * kBobVerticalScans;
    for( int i = 0; i<totalCycles; ++i ) {
        const int addr = static_cast<int>(monitor.getFloatingBusAddress());
        if( !(addr>=loExpected && addr<=hiExpected) ) {
            std::ostringstream oss;
            oss << "out-of-range addr=0x" << std::hex << addr
                << " h=" << std::dec << (int) monitor.getHRefreshCount()
                << " v=" << (int) monitor.getVRefreshCount()
                << " expected=[0x" << std::hex << loExpected << ",0x" << hiExpected << "]";
            e2test::fail(oss.str());
        }
        monitor.cycle();
    }
}

bool isNonHiresTextWindow(int addr)
{
    return addr>=kTextBaseLo && addr<=kTextBaseHi;
}

} // namespace

E2TEST_CASE(floatingBusAddressSmokeTextModeRangeAcrossEveryXY)
{
    ScopedCwd cwd("release");
    E2TEST_ASSERT_TRUE(cwd.active());

    PixelSurface surface(560, 384, 32);
    Memory128k memory;
    Monitor560x192 monitor(&surface, &memory);

    setDisplaySwitches(memory, true, false, false, false, false);
    monitor.commitSwitches();
    monitor.resetAll();
    const int totalCycles = kBobHorizontalScans * kBobVerticalScans;
    for( int i = 0; i<totalCycles; ++i ) {
        const int addr = static_cast<int>(monitor.getFloatingBusAddress());
        E2TEST_ASSERT_TRUE(isNonHiresTextWindow(addr));
        monitor.cycle();
    }
}

E2TEST_CASE(floatingBusAddressSmokeHiresPage1RangeAcrossEveryXY)
{
    ScopedCwd cwd("release");
    E2TEST_ASSERT_TRUE(cwd.active());

    PixelSurface surface(560, 384, 32);
    Memory128k memory;
    Monitor560x192 monitor(&surface, &memory);

    setDisplaySwitches(memory, false, false, true, false, false);
    monitor.commitSwitches();
    monitor.resetAll();
    assertRangeEveryXY(monitor, kHiresPage1Lo, kHiresPage1Hi);
}

E2TEST_CASE(floatingBusAddressSmokeHiresPage2RangeAcrossEveryXY)
{
    ScopedCwd cwd("release");
    E2TEST_ASSERT_TRUE(cwd.active());

    PixelSurface surface(560, 384, 32);
    Memory128k memory;
    Monitor560x192 monitor(&surface, &memory);

    setDisplaySwitches(memory, false, false, true, true, false);
    monitor.commitSwitches();
    monitor.resetAll();
    assertRangeEveryXY(monitor, kHiresPage2Lo, kHiresPage2Hi);
}

E2TEST_CASE(floatingBusAddressSmokeMixedModeRangeAcrossEveryXY)
{
    ScopedCwd cwd("release");
    E2TEST_ASSERT_TRUE(cwd.active());

    PixelSurface surface(560, 384, 32);
    Memory128k memory;
    Monitor560x192 monitor(&surface, &memory);

    setDisplaySwitches(memory, false, true, true, false, false);
    monitor.commitSwitches();
    monitor.resetAll();

    const int totalCycles = kBobHorizontalScans * kBobVerticalScans;
    for( int i = 0; i<totalCycles; ++i ) {
        const int v = static_cast<int>(monitor.getVRefreshCount());
        const int addr = static_cast<int>(monitor.getFloatingBusAddress());
        if( mixedUsesTextBand(v) )
            E2TEST_ASSERT_TRUE(isNonHiresTextWindow(addr));
        else
            E2TEST_ASSERT_TRUE(addr>=kHiresPage1Lo && addr<=kHiresPage1Hi);
        monitor.cycle();
    }

    E2TEST_ASSERT_EQ(25, kBobHorizontalBlank);
    E2TEST_ASSERT_EQ(70, kBobVerticalBlank);
}
