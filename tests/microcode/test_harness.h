#ifndef EVER2E_TEST_HARNESS_H
#define EVER2E_TEST_HARNESS_H

#include <exception>
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
