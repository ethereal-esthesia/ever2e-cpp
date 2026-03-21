#include "test_harness.h"

#include <limits.h>
#include <string>
#include <unistd.h>

#include "memory128k.h"
#include "mon560x192.h"
#include "pixel.h"

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

E2TEST_CASE(monitorRetraceTimingMatchesAppleIIeNtscDurationsAcrossTenFrames)
{
    ScopedCwd cwd("release");
    E2TEST_ASSERT_TRUE(cwd.active());

    PixelSurface surface(560, 384, 32);
    Memory128k memory;
    Monitor560x192 monitor(&surface, &memory);

    const int hCyclesPerLine = Monitor560x192::HORIZONTAL_SCANS;       // 65
    const int visibleCyclesPerLine = Monitor560x192::HORIZONTAL_BLANK; // 40
    const int hBlankCyclesPerLine = hCyclesPerLine - visibleCyclesPerLine; // 25

    const int vLinesPerFrame = Monitor560x192::VERTICAL_SCANS;         // 262
    const int visibleLinesPerFrame = Monitor560x192::VERTICAL_BLANK;   // 192
    const int vBlankLinesPerFrame = vLinesPerFrame - visibleLinesPerFrame; // 70

    const int framesToRun = 10;
    const int totalCycles = framesToRun * hCyclesPerLine * vLinesPerFrame;

    int cyclesInCurrentLine = 0;
    int hBlankCyclesInCurrentLine = 0;
    int linesInCurrentFrame = 0;
    int vBlankCyclesInCurrentFrame = 0;
    int completedFrames = 0;

    for( int i = 0; i<totalCycles; ++i ) {
        const int h = monitor.getHRefreshCount();
        const bool inHBlank = h >= visibleCyclesPerLine;
        if( inHBlank )
            hBlankCyclesInCurrentLine++;

        if( monitor.getVbl() )
            vBlankCyclesInCurrentFrame++;

        cyclesInCurrentLine++;
        monitor.cycle();

        if( monitor.getHRefreshCount()==0 ) {
            E2TEST_ASSERT_EQ(hCyclesPerLine, cyclesInCurrentLine);
            E2TEST_ASSERT_EQ(hBlankCyclesPerLine, hBlankCyclesInCurrentLine);
            cyclesInCurrentLine = 0;
            hBlankCyclesInCurrentLine = 0;
            linesInCurrentFrame++;

            if( monitor.getVRefreshCount()==0 ) {
                E2TEST_ASSERT_EQ(vLinesPerFrame, linesInCurrentFrame);
                E2TEST_ASSERT_EQ(vBlankLinesPerFrame * hCyclesPerLine, vBlankCyclesInCurrentFrame);
                linesInCurrentFrame = 0;
                vBlankCyclesInCurrentFrame = 0;
                completedFrames++;
            }
        }
    }

    E2TEST_ASSERT_EQ(framesToRun, completedFrames);
}
