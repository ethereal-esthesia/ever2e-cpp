#include "test_harness.h"

#include <array>
#include <cstdio>
#include <cstdlib>
#include <limits.h>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>

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

std::vector<std::string> runCommandLines(const std::string& command)
{
    std::vector<std::string> lines;
    FILE* pipe = popen(command.c_str(), "r");
    if( pipe==NULL )
        e2test::fail("popen failed for command: " + command);
    std::array<char, 512> buffer;
    while( fgets(buffer.data(), (int) buffer.size(), pipe)!=NULL ) {
        std::string line(buffer.data());
        if( !line.empty() && line.back()=='\n' )
            line.pop_back();
        if( !line.empty() )
            lines.push_back(line);
    }
    int status = pclose(pipe);
    if( status!=0 ) {
        std::ostringstream oss;
        oss << "command failed (" << status << "): " << command;
        e2test::fail(oss.str());
    }
    return lines;
}

void setDisplaySwitches(Memory128k& memory, bool text, bool mixed, bool hires, bool page2, bool store80)
{
    if( text ) memory.setSwitch(Memory128k::_TEXT); else memory.resetSwitch(Memory128k::_TEXT);
    if( mixed ) memory.setSwitch(Memory128k::_MIXED); else memory.resetSwitch(Memory128k::_MIXED);
    if( hires ) memory.setSwitch(Memory128k::_HIRES); else memory.resetSwitch(Memory128k::_HIRES);
    if( page2 ) memory.setSwitch(Memory128k::_PAGE2); else memory.resetSwitch(Memory128k::_PAGE2);
    if( store80 ) memory.setSwitch(Memory128k::_80STORE); else memory.resetSwitch(Memory128k::_80STORE);
}

std::vector<std::string> buildBobLines(Memory128k& memory, Monitor560x192& monitor, int cycles)
{
    std::vector<std::string> lines;
    lines.reserve((size_t) cycles);
    for( int t = 0; t<cycles; ++t ) {
        Uint16 addr = monitor.getFloatingBusAddress();
        Uint8 val = memory.getMemPassive(0, addr);
        std::ostringstream oss;
        oss << (t<10000 ? "0" : "") << (t<1000 ? "0" : "") << (t<100 ? "0" : "") << (t<10 ? "0" : "") << t
            << ",0x" << std::hex << std::uppercase
            << (addr<0x1000 ? "0" : "") << (addr<0x100 ? "0" : "") << (addr<0x10 ? "0" : "") << (int) addr
            << ",0x"
            << (val<0x10 ? "0" : "") << (int) val;
        lines.push_back(oss.str());
        monitor.cycle();
    }
    return lines;
}

} // namespace

E2TEST_CASE(floatingBusBobReferenceMatchesEver2ePyOracleLineByLine)
{
    ScopedCwd cwd("release");
    E2TEST_ASSERT_TRUE(cwd.active());

    const char* cppRootEnv = getenv("EVER2E_CPP_ROOT");
    const char* pyRootEnv = getenv("EVER2E_PY_ROOT");
    const std::string cppRoot = (cppRootEnv && *cppRootEnv) ? cppRootEnv : "/Users/shane/Project/ever2e-cpp";
    const std::string pyRoot = (pyRootEnv && *pyRootEnv) ? pyRootEnv : "/Users/shane/Project/ever2e-py";
    const std::string scriptPath = cppRoot + "/tools/gen_floating_bus_oracle_py.py";
    const int cycles = 2048;

    PixelSurface surface(560, 384, 32);
    Memory128k memory;
    Monitor560x192 monitor(&surface, &memory);
    Cpu65c02 cpu(&memory, Cpu65c02::PROFILE_CMD);
    Speaker1bit speaker;
    Keyboard2e keyboard(&monitor, &cpu);
    memory.putPeripheral(&cpu, &monitor, &speaker, &keyboard);

    std::vector<Uint8> pattern(0x10000);
    for( int i = 0; i<0x10000; ++i )
        pattern[(size_t) i] = (Uint8) (i & 0xFF);
    memory.putMemRange(0, 0x0000, pattern.data(), (Uint32) pattern.size());

    const bool text = false;
    const bool mixed = true;
    const bool hires = true;
    const bool page2 = false;
    const bool store80 = false;
    setDisplaySwitches(memory, text, mixed, hires, page2, store80);

    std::ostringstream cmd;
    cmd << "python3 " << scriptPath
        << " --py-root " << pyRoot
        << " --cycles " << cycles
        << " --text " << (text ? 1 : 0)
        << " --mixed " << (mixed ? 1 : 0)
        << " --hires " << (hires ? 1 : 0)
        << " --page2 " << (page2 ? 1 : 0)
        << " --store80 " << (store80 ? 1 : 0);
    std::vector<std::string> pyLines = runCommandLines(cmd.str());
    std::vector<std::string> bobLines = buildBobLines(memory, monitor, cycles);

    E2TEST_ASSERT_EQ(pyLines.size(), bobLines.size());
    for( size_t i = 0; i<pyLines.size(); ++i ) {
        if( pyLines[i] != bobLines[i] ) {
            std::ostringstream oss;
            oss << "line mismatch at " << i << " expected=\"" << pyLines[i] << "\" actual=\"" << bobLines[i] << "\"";
            e2test::fail(oss.str());
        }
    }
}
