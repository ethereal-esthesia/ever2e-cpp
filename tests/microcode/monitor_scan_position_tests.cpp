#include "test_harness.h"

#include <limits.h>
#include <string>
#include <unistd.h>

#include "cpu65c02.h"
#include "keyboard2e.h"
#include "memory128k.h"
#include "mon560x192.h"
#include "pixel.h"
#include "speaker1bit.h"

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

enum DisplayModeCase {
    MODE_TEXT40 = 0,
    MODE_TEXT80 = 1,
    MODE_LORES40 = 2,
    MODE_DLORES80 = 3,
    MODE_HIRES40 = 4,
    MODE_DHIRES80 = 5,
    MODE_SPLIT_TEXT40 = 6,
    MODE_SPLIT_HIRES40 = 7,
};

void applyDisplayMode(Cpu65c02& cpu, DisplayModeCase modeCase)
{
    cpu.setModeFullScreen();
    cpu.setPage(1);
    cpu.setMemoryPage(true);

    switch( modeCase ) {
    case MODE_TEXT40:
        cpu.setModeText40();
        break;
    case MODE_TEXT80:
        cpu.setModeText80();
        break;
    case MODE_LORES40:
        cpu.setModeLores();
        break;
    case MODE_DLORES80:
        cpu.setModeDoubleLores();
        break;
    case MODE_HIRES40:
        cpu.setModeHires();
        break;
    case MODE_DHIRES80:
        cpu.setModeDoubleHires();
        break;
    case MODE_SPLIT_TEXT40:
        cpu.setModeText40();
        cpu.setModeSplitScreen();
        break;
    case MODE_SPLIT_HIRES40:
        cpu.setModeHires();
        cpu.setModeSplitScreen();
        break;
    default:
        E2TEST_ASSERT_TRUE(false);
        break;
    }
}

} // namespace

E2TEST_CASE(monitorHorizontalAndVerticalReadPositionsMatchEveryCycleAcrossAllModes)
{
    ScopedCwd cwd("release");
    E2TEST_ASSERT_TRUE(cwd.active());

    const int framesToRun = 10;
    const int hCycles = Monitor560x192::HORIZONTAL_SCANS; // 65
    const int vLines = Monitor560x192::VERTICAL_SCANS;    // 262
    const int frameCycles = hCycles * vLines;             // 17030
    const int totalCycles = framesToRun * frameCycles;

    for( int mode = MODE_TEXT40; mode<=MODE_SPLIT_HIRES40; ++mode ) {
        PixelSurface surface(560, 384, 32);
        Memory128k memory;
        Monitor560x192 monitor(&surface, &memory);
        Cpu65c02 cpu(&memory, Cpu65c02::PROFILE_CMD);
        Speaker1bit speaker;
        Keyboard2e keyboard(&monitor, &cpu);
        memory.putPeripheral(&cpu, &monitor, &speaker, &keyboard);

        applyDisplayMode(cpu, static_cast<DisplayModeCase>(mode));
        memory.cycle(); // Commit soft-switches to monitor

        for( int t = 0; t<totalCycles; ++t ) {
            const int expectedH = t % hCycles;
            const int expectedV = (t / hCycles) % vLines;

            E2TEST_ASSERT_EQ(expectedH, static_cast<int>(monitor.getHRefreshCount()));
            E2TEST_ASSERT_EQ(expectedV, static_cast<int>(monitor.getVRefreshCount()));

            monitor.cycle();
        }
    }
}
