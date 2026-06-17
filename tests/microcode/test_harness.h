#ifndef EVER2E_TEST_HARNESS_H
#define EVER2E_TEST_HARNESS_H

#include <exception>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace e2test {

struct TestCase {
    const char* name;
    void (*fn)();
};

inline std::vector<TestCase>& registry()
{
    static std::vector<TestCase> tests;
    return tests;
}

struct Registrar {
    Registrar(const char* name, void (*fn)())
    {
        registry().push_back(TestCase{name, fn});
    }
};

inline void fail(const std::string& msg)
{
    throw std::runtime_error(msg);
}

inline std::string testRomDirectory()
{
    const char* overrideDir = std::getenv("EVER2E_TEST_ROM_DIR");
    if( overrideDir!=nullptr && overrideDir[0]!='\0' )
        return std::string(overrideDir);

    std::filesystem::path dir = std::filesystem::temp_directory_path() / "ever2e-microcode-test-rom";
    std::error_code ec;
    std::filesystem::create_directories(dir, ec);
    if( ec )
        fail("unable to create test ROM directory: " + dir.string());

    const std::filesystem::path romPath = dir / "apple2e.rom";
    std::vector<unsigned char> rom(0x4000, 0xEA);
    for( int vector : {0x3FFA, 0x3FFC, 0x3FFE} ) {
        rom[(size_t)vector] = 0x00;
        rom[(size_t)vector + 1] = 0xF0;
    }
    std::ofstream out(romPath, std::ios::out | std::ios::binary | std::ios::trunc);
    if( !out.is_open() )
        fail("unable to write test ROM: " + romPath.string());
    out.write(reinterpret_cast<const char*>(rom.data()), (std::streamsize)rom.size());

    return dir.string();
}

} // namespace e2test

#define E2TEST_CASE(name) \
    static void name(); \
    static e2test::Registrar name##_registrar(#name, &name); \
    static void name()

#define E2TEST_ASSERT_TRUE(cond) \
    do { \
        if( !(cond) ) { \
            std::ostringstream _oss; \
            _oss << __FILE__ << ":" << __LINE__ << " assertion failed: " #cond; \
            e2test::fail(_oss.str()); \
        } \
    } while (0)

#define E2TEST_ASSERT_EQ(expected, actual) \
    do { \
        auto _e = (expected); \
        auto _a = (actual); \
        if( !(_e==_a) ) { \
            std::ostringstream _oss; \
            _oss << __FILE__ << ":" << __LINE__ << " expected=" << _e << " actual=" << _a; \
            e2test::fail(_oss.str()); \
        } \
    } while (0)

#define E2TEST_ASSERT_VEC_EQ(expected, actual) \
    do { \
        const auto& _e = (expected); \
        const auto& _a = (actual); \
        if( _e.size()!=_a.size() ) { \
            std::ostringstream _oss; \
            _oss << __FILE__ << ":" << __LINE__ << " vector size expected=" << _e.size() << " actual=" << _a.size(); \
            e2test::fail(_oss.str()); \
        } \
        for( size_t _i = 0; _i<_e.size(); ++_i ) { \
            if( !(_e[_i]==_a[_i]) ) { \
                std::ostringstream _oss; \
                _oss << __FILE__ << ":" << __LINE__ << " vector mismatch at index " << _i; \
                e2test::fail(_oss.str()); \
            } \
        } \
    } while (0)

#endif
