#include "test_harness.h"

int main()
{
    int passed = 0;
    int failed = 0;
    for( const e2test::TestCase& tc : e2test::registry() ) {
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
    }

    std::cout << "microcode tests: passed=" << passed << " failed=" << failed << "\n";
    return failed==0 ? 0 : 1;
}
