#include "test_harness.h"

#include <csignal>
#include <cstdlib>
#include <iostream>

namespace {

const char* CurrentTestName = nullptr;

void handleCrashSignal(int signalNumber)
{
    std::cerr << "CRASH " << (CurrentTestName!=nullptr ? CurrentTestName : "<unknown>")
              << ": signal " << signalNumber << "\n";
    std::_Exit(128 + signalNumber);
}

} // namespace

int main()
{
    std::signal(SIGBUS, handleCrashSignal);
    std::signal(SIGSEGV, handleCrashSignal);

    int passed = 0;
    int failed = 0;
    for( const e2test::TestCase& tc : e2test::registry() ) {
        CurrentTestName = tc.name;
        try {
            tc.fn();
            ++passed;
        }
        catch( const std::exception& e ) {
            ++failed;
            std::cerr << "FAIL " << tc.name << ": " << e.what() << "\n";
        }
        catch( ... ) {
            ++failed;
            std::cerr << "FAIL " << tc.name << ": unknown exception\n";
        }
        CurrentTestName = nullptr;
    }

    std::cout << "microcode tests: passed=" << passed << " failed=" << failed << "\n";
    return failed==0 ? 0 : 1;
}
