#include "test_harness.h"

#include <array>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <limits.h>
#include <sstream>
#include <string>
#include <tuple>
#include <unistd.h>
#include <vector>

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

std::string currentWorkingDirectory()
{
    char buf[PATH_MAX];
    if( getcwd(buf, sizeof(buf))==NULL )
        e2test::fail("getcwd failed");
    return std::string(buf);
}

bool pathExists(const std::string& path)
{
    return access(path.c_str(), F_OK)==0;
}

std::string shellQuote(const std::string& value)
{
    std::string quoted = "'";
    for( char ch : value ) {
        if( ch=='\'' )
            quoted += "'\\''";
        else
            quoted += ch;
    }
    quoted += "'";
    return quoted;
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
    const char* cppRootEnv = getenv("EVER2E_CPP_ROOT");
    const char* pyRootEnv = getenv("EVER2E_PY_ROOT");
    const std::string cppRoot = (cppRootEnv && *cppRootEnv) ? cppRootEnv : currentWorkingDirectory();
    const std::string pyRoot = (pyRootEnv && *pyRootEnv) ? pyRootEnv : cppRoot + "/../ever2e-py";
    const std::string scriptPath = cppRoot + "/tools/gen_floating_bus_oracle_py.py";
    const int cycles = 1024;

    E2TEST_ASSERT_TRUE(pathExists(scriptPath));
    if( !pathExists(pyRoot) ) {
        std::cout << "SKIP floatingBusBobReferenceMatchesEver2ePyOracleLineByLine: ever2e-py not found at "
                  << pyRoot << "\n";
        return;
    }

    ScopedCwd cwd(cppRoot + "/release");
    E2TEST_ASSERT_TRUE(cwd.active());

    PixelSurface surface(560, 384, 32);
    Memory128k memory;
    Monitor560x192 monitor(&surface, &memory);
    memory.putPeripheral(NULL, &monitor, NULL, NULL);

    std::vector<Uint8> pattern(0x10000);
    for( int i = 0; i<0x10000; ++i )
        pattern[(size_t) i] = (Uint8) (i & 0xFF);
    memory.putMemRange(0, 0x0000, pattern.data(), (Uint32) pattern.size());

    const std::vector<std::tuple<bool, bool, bool, bool, bool>> modes = {
        {true,  false, false, false, false}, // text
        {false, false, true,  false, false}, // hires page1
        {false, false, true,  true,  false}, // hires page2
        {false, true,  true,  false, false}, // mixed hires/text
        {false, false, false, false, false}, // lores
        {false, false, true,  false, true }, // hires with 80store
        {true,  false, false, true,  false}, // text page2
        {false, true,  true,  true,  false}, // mixed with page2
    };

    for( const auto& mode : modes ) {
        const bool text = std::get<0>(mode);
        const bool mixed = std::get<1>(mode);
        const bool hires = std::get<2>(mode);
        const bool page2 = std::get<3>(mode);
        const bool store80 = std::get<4>(mode);

        setDisplaySwitches(memory, text, mixed, hires, page2, store80);
        monitor.commitSwitches();
        monitor.resetAll();

        std::ostringstream cmd;
        cmd << "python3 " << shellQuote(scriptPath)
            << " --py-root " << shellQuote(pyRoot)
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
                oss << "mode(text=" << (text ? 1 : 0)
                    << ",mixed=" << (mixed ? 1 : 0)
                    << ",hires=" << (hires ? 1 : 0)
                    << ",page2=" << (page2 ? 1 : 0)
                    << ",store80=" << (store80 ? 1 : 0)
                    << ") line mismatch at " << i
                    << " expected=\"" << pyLines[i] << "\" actual=\"" << bobLines[i] << "\"";
                e2test::fail(oss.str());
            }
        }
    }
}
