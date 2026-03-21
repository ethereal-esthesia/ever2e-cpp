#ifndef EVER2E_JVM_TEST_PORT_HARNESS_H
#define EVER2E_JVM_TEST_PORT_HARNESS_H

// Lightweight placeholder harness for JVM-to-C++ test ports.
// These functions are intentionally not wired to a runner yet.

#define E2_PORT_TEST(name) static void name()
#define E2_PORT_UNIMPLEMENTED(note) do { (void)(note); } while (0)

#endif // EVER2E_JVM_TEST_PORT_HARNESS_H
