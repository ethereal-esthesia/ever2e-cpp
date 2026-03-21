# JVM Test Port Manifest

This directory contains one-to-one placeholder C++ ports of all JVM JUnit tests.
Each function keeps the original Java method body in comments for follow-up semantic translation.

- `IIeDisplayConsoleTest.java` -> `IIeDisplayConsoleTest.cpp` (1 tests)
- `cpu/Cpu32kLongRunCmdIntegrationTest.java` -> `cpu/Cpu32kLongRunCmdIntegrationTest.cpp` (1 tests)
- `cpu/Cpu65C02OpcodeLevelTest.java` -> `cpu/Cpu65C02OpcodeLevelTest.cpp` (1 tests)
- `cpu/Cpu65c02CycleTimingTest.java` -> `cpu/Cpu65c02CycleTimingTest.cpp` (13 tests)
- `cpu/Cpu65c02DecimalMathTest.java` -> `cpu/Cpu65c02DecimalMathTest.cpp` (2 tests)
- `cpu/Cpu65c02MicrocodeTest.java` -> `cpu/Cpu65c02MicrocodeTest.cpp` (99 tests)
- `cpu/Cpu65c02ProfileMicrocodeCoverageTest.java` -> `cpu/Cpu65c02ProfileMicrocodeCoverageTest.cpp` (8 tests)
- `cpu/CpuLdaOpcodeBenchTest.java` -> `cpu/CpuLdaOpcodeBenchTest.cpp` (1 tests)
- `cpu/Emulator8CoordinatorDisplayValuesTest.java` -> `cpu/Emulator8CoordinatorDisplayValuesTest.cpp` (2 tests)
- `cpu/EmulatorSchedulerContractTest.java` -> `cpu/EmulatorSchedulerContractTest.cpp` (8 tests)
- `cpu/EmulatorSchedulerIntegrationTest.java` -> `cpu/EmulatorSchedulerIntegrationTest.cpp` (2 tests)
- `device/display/FloatingBusAddressResolutionTest.java` -> `device/display/FloatingBusAddressResolutionTest.cpp` (2 tests)

Total files: 12
Total tests: 140
