// Auto-generated JVM test port scaffold.
// Source: /Users/shane/Project/ever2e-jvm/src/test/cpu/Cpu65c02CycleTimingTest.java
// NOTE: Function bodies are placeholders; original Java test bodies are preserved below each test.

#include "../jvm_test_port_harness.h"

E2_PORT_TEST(Cpu65c02CycleTimingTest__ldaAbsXAddsCycleOnPageCrossOnly) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02CycleTimingTest.java::ldaAbsXAddsCycleOnPageCrossOnly");
    /*
    Original Java test body:
            for( String profile : CPU_PROFILES ) {
                CpuEnv env = createEnv(profile);
                loadProgram(env, 0xBD, 0xFE, 0x20); // LDA $20FE,X
                runInstruction(env); // execute reset after program bytes are loaded
                env.reg.setX(0x01);
                env.bus.setByte(0x20FF, 0x11);
                runInstruction(env);
                assertEquals(estimateCycles(env, 0xBD), env.cpu.getLastInstructionCycleCount());
    
                env = createEnv(profile);
                loadProgram(env, 0xBD, 0xFF, 0x20); // LDA $20FF,X
                runInstruction(env);
                env.reg.setX(0x01);
                env.bus.setByte(0x2100, 0x22);
                runInstruction(env);
                assertEquals(estimateCycles(env, 0xBD), env.cpu.getLastInstructionCycleCount());
            }
    
    */
}

E2_PORT_TEST(Cpu65c02CycleTimingTest__ldaAbsYAddsCycleOnPageCrossOnly) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02CycleTimingTest.java::ldaAbsYAddsCycleOnPageCrossOnly");
    /*
    Original Java test body:
            for( String profile : CPU_PROFILES ) {
                CpuEnv env = createEnv(profile);
                loadProgram(env, 0xB9, 0xFE, 0x20); // LDA $20FE,Y
                runInstruction(env);
                env.reg.setY(0x01);
                env.bus.setByte(0x20FF, 0x33);
                runInstruction(env);
                assertEquals(estimateCycles(env, 0xB9), env.cpu.getLastInstructionCycleCount());
    
                env = createEnv(profile);
                loadProgram(env, 0xB9, 0xFF, 0x20); // LDA $20FF,Y
                runInstruction(env);
                env.reg.setY(0x01);
                env.bus.setByte(0x2100, 0x44);
                runInstruction(env);
                assertEquals(estimateCycles(env, 0xB9), env.cpu.getLastInstructionCycleCount());
            }
    
    */
}

E2_PORT_TEST(Cpu65c02CycleTimingTest__ldaIndYAddsCycleOnPageCrossOnly) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02CycleTimingTest.java::ldaIndYAddsCycleOnPageCrossOnly");
    /*
    Original Java test body:
            for( String profile : CPU_PROFILES ) {
                CpuEnv env = createEnv(profile);
                loadProgram(env, 0xB1, 0x10); // LDA ($10),Y
                runInstruction(env);
                env.reg.setY(0x01);
                env.bus.setByte(0x0010, 0xFE);
                env.bus.setByte(0x0011, 0x20);
                env.bus.setByte(0x20FF, 0x55);
                runInstruction(env);
                assertEquals(estimateCycles(env, 0xB1), env.cpu.getLastInstructionCycleCount());
    
                env = createEnv(profile);
                loadProgram(env, 0xB1, 0x10); // LDA ($10),Y
                runInstruction(env);
                env.reg.setY(0x01);
                env.bus.setByte(0x0010, 0xFF);
                env.bus.setByte(0x0011, 0x20);
                env.bus.setByte(0x2100, 0x66);
                runInstruction(env);
                assertEquals(estimateCycles(env, 0xB1), env.cpu.getLastInstructionCycleCount());
            }
    
    */
}

E2_PORT_TEST(Cpu65c02CycleTimingTest__jmpAbsIndXHasNoPageCrossPenalty) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02CycleTimingTest.java::jmpAbsIndXHasNoPageCrossPenalty");
    /*
    Original Java test body:
            for( String profile : CPU_PROFILES ) {
                CpuEnv env = createEnv(profile);
                loadProgram(env, 0x7C, 0xFF, 0x20); // JMP ($20FF,X)
                runInstruction(env);
                env.reg.setX(0x01);
                env.bus.setByte(0x2100, 0x34);
                env.bus.setByte(0x2101, 0x12);
                runInstruction(env);
                assertEquals(6, env.cpu.getLastInstructionCycleCount());
                assertEquals(0x1234, env.cpu.getPendingPC());
            }
    
    */
}

E2_PORT_TEST(Cpu65c02CycleTimingTest__staAbsXHasNoExtraPageCrossCycle) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02CycleTimingTest.java::staAbsXHasNoExtraPageCrossCycle");
    /*
    Original Java test body:
            for( String profile : CPU_PROFILES ) {
                CpuEnv env = createEnv(profile);
                loadProgram(env, 0x9D, 0xFF, 0x20); // STA $20FF,X
                runInstruction(env);
                env.reg.setA(0xA5);
                env.reg.setX(0x01);
                runInstruction(env);
                assertEquals(5, env.cpu.getLastInstructionCycleCount());
                assertEquals(0xA5, env.bus.getByte(0x2100));
            }
    
    */
}

E2_PORT_TEST(Cpu65c02CycleTimingTest__cmdProfileUsesHardwareValidatedNopTimingAndLength) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02CycleTimingTest.java::cmdProfileUsesHardwareValidatedNopTimingAndLength");
    /*
    Original Java test body:
            CpuEnv env = createEnv("cmd");
            loadProgram(env, 0x03, 0x00, 0x00); // CMD undocumented NOP
            runInstruction(env); // reset
            runInstruction(env);
            assertEquals(1, env.cpu.getLastInstructionCycleCount());
            assertEquals(PROG_PC + 1, env.cpu.getPendingPC());
    
            env = createEnv("cmd");
            loadProgram(env, 0x5C, 0x00, 0x00); // CMD undocumented long NOP
            runInstruction(env); // reset
            runInstruction(env);
            assertEquals(8, env.cpu.getLastInstructionCycleCount());
            assertEquals(PROG_PC + 3, env.cpu.getPendingPC());
    
    */
}

E2_PORT_TEST(Cpu65c02CycleTimingTest__cmdProfileAllMeasuredNopCyclesMatchHardwareTable) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02CycleTimingTest.java::cmdProfileAllMeasuredNopCyclesMatchHardwareTable");
    /*
    Original Java test body:
            CpuEnv env = createEnv("cmd");
            Opcode[] table = opcodeTableFor(env.cpu);
            for( int[] expected : CMD_NOP_CYCLE_EXPECTATIONS ) {
                int opcode = expected[0] & 0xFF;
                int cycles = expected[1];
                Opcode actual = table[opcode];
                assertEquals("CMD opcode should be NOP for $" + String.format("%02X", opcode),
                        Cpu65c02.OpcodeMnemonic.NOP, actual.getMnemonic());
                assertEquals("CMD cycle mismatch for $" + String.format("%02X", opcode),
                        cycles, actual.getCycleTime());
            }
    
    */
}

E2_PORT_TEST(Cpu65c02CycleTimingTest__cmdProfileMeasuredCompatibilityNopWidthsMatchMameTable) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02CycleTimingTest.java::cmdProfileMeasuredCompatibilityNopWidthsMatchMameTable");
    /*
    Original Java test body:
            CpuEnv env = createEnv("cmd");
            Opcode[] table = opcodeTableFor(env.cpu);
            for( int[] expected : CMD_NOP_WIDTH_EXPECTATIONS ) {
                int opcode = expected[0] & 0xFF;
                int width = expected[1];
                Opcode actual = table[opcode];
                assertEquals("CMD opcode should be NOP for $" + String.format("%02X", opcode),
                        Cpu65c02.OpcodeMnemonic.NOP, actual.getMnemonic());
                assertEquals("CMD width mismatch for $" + String.format("%02X", opcode),
                        width, actual.getInstrSize());
            }
    
    */
}

E2_PORT_TEST(Cpu65c02CycleTimingTest__cmdEightCycleUndocumentedNopRetiresAsSingleInstructionEvent) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02CycleTimingTest.java::cmdEightCycleUndocumentedNopRetiresAsSingleInstructionEvent");
    /*
    Original Java test body:
            CpuEnv env = createEnv("cmd");
            loadProgram(env, 0x5C, 0x00, 0x00, 0xEA); // 8-cycle undocumented NOP, then official NOP
            runInstruction(env); // reset
    
            int subcycles = 0;
            int retireEvents = 0;
            int cpuCalls = 0;
            while( true ) {
                boolean instructionEndsThisCycle = env.cpu.hasPendingInstructionEndEvent();
                boolean subcycleThisCycle = env.cpu.hasPendingInFlightMicroEvent();
                env.emulator.startWithStepPhases(1, env.cpu, (step, manager, preCycle) -> true);
                cpuCalls++;
                if( subcycleThisCycle )
                    subcycles++;
                if( instructionEndsThisCycle ) {
                    retireEvents++;
                    break;
                }
                if( cpuCalls>32 )
                    throw new AssertionError("NOP $5C did not retire within expected safety bound");
            }
    
            assertEquals(1, retireEvents);
            assertEquals(1, cpuCalls);
            assertEquals(0, subcycles);
            assertEquals(8, env.cpu.getLastInstructionCycleCount());
            assertEquals(PROG_PC + 3, env.cpu.getPendingPC());
    
    */
}

E2_PORT_TEST(Cpu65c02CycleTimingTest__decimalAdcZeroPageTakesFourCycles) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02CycleTimingTest.java::decimalAdcZeroPageTakesFourCycles");
    /*
    Original Java test body:
            for( String profile : CPU_PROFILES ) {
                CpuEnv env = createEnv(profile);
                loadProgram(env, 0x65, 0x10); // ADC $10
                runInstruction(env); // reset
                env.reg.setA(0x45);
                env.reg.setP(0x3C | 0x01); // R,B,D,I + carry-in
                env.bus.setByte(0x0010, 0x55);
                runInstruction(env);
                assertEquals("ADC decimal cycle count mismatch for profile " + profile, 4, env.cpu.getLastInstructionCycleCount());
                assertEquals(0x01, env.reg.getA()); // 45 + 55 + carry(1) = 01 with BCD carry out
            }
    
    */
}

E2_PORT_TEST(Cpu65c02CycleTimingTest__decimalSbcZeroPageTakesFourCycles) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02CycleTimingTest.java::decimalSbcZeroPageTakesFourCycles");
    /*
    Original Java test body:
            for( String profile : CPU_PROFILES ) {
                CpuEnv env = createEnv(profile);
                loadProgram(env, 0xE5, 0x10); // SBC $10
                runInstruction(env); // reset
                env.reg.setA(0x45);
                env.reg.setP(0x3C | 0x01); // R,B,D,I + no borrow
                env.bus.setByte(0x0010, 0x12);
                runInstruction(env);
                assertEquals("SBC decimal cycle count mismatch for profile " + profile, 4, env.cpu.getLastInstructionCycleCount());
                assertEquals(0x33, env.reg.getA()); // 45 - 12 = 33 in BCD
            }
    
    */
}

E2_PORT_TEST(Cpu65c02CycleTimingTest__decimalModeAddsOneQueuedSubcycleForAdcAndSbc) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02CycleTimingTest.java::decimalModeAddsOneQueuedSubcycleForAdcAndSbc");
    /*
    Original Java test body:
            for( String profile : CPU_PROFILES ) {
                // ADC zpg: binary=3 cycles (2 subs), decimal=4 cycles (3 subs)
                CpuEnv env = createEnv(profile);
                loadProgram(env, 0x65, 0x10); // ADC $10
                runInstruction(env); // reset
                env.reg.setA(0x21);
                env.reg.setP(0x34); // binary mode
                env.bus.setByte(0x0010, 0x11);
                int adcBinarySubcycles = runInstructionAndCountSubcycles(env);
                assertEquals(2, adcBinarySubcycles);
                assertEquals(3, env.cpu.getLastInstructionCycleCount());
    
                env = createEnv(profile);
                loadProgram(env, 0xF8, 0x65, 0x10); // SED ; ADC $10
                runInstruction(env); // reset
                runInstruction(env); // SED, so ADC is planned with D=1
                env.reg.setA(0x21);
                env.bus.setByte(0x0010, 0x11);
                int adcDecimalSubcycles = runInstructionAndCountSubcycles(env);
                assertEquals(3, adcDecimalSubcycles);
                assertEquals(4, env.cpu.getLastInstructionCycleCount());
    
                // SBC zpg: binary=3 cycles (2 subs), decimal=4 cycles (3 subs)
                env = createEnv(profile);
                loadProgram(env, 0xE5, 0x10); // SBC $10
                runInstruction(env); // reset
                env.reg.setA(0x54);
                env.reg.setP(0x35); // binary mode + carry set
                env.bus.setByte(0x0010, 0x12);
                int sbcBinarySubcycles = runInstructionAndCountSubcycles(env);
                assertEquals(2, sbcBinarySubcycles);
                assertEquals(3, env.cpu.getLastInstructionCycleCount());
    
                env = createEnv(profile);
                loadProgram(env, 0xF8, 0xE5, 0x10); // SED ; SBC $10
                runInstruction(env); // reset
                runInstruction(env); // SED, so SBC is planned with D=1
                env.reg.setA(0x54);
                env.reg.setP(env.reg.getP() | 0x01); // keep D from SED, set carry for no borrow
                env.bus.setByte(0x0010, 0x12);
                int sbcDecimalSubcycles = runInstructionAndCountSubcycles(env);
                assertEquals(3, sbcDecimalSubcycles);
                assertEquals(4, env.cpu.getLastInstructionCycleCount());
            }
    
    */
}

E2_PORT_TEST(Cpu65c02CycleTimingTest__branchTakenAndPageCrossQueueExpectedSubcycles) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02CycleTimingTest.java::branchTakenAndPageCrossQueueExpectedSubcycles");
    /*
    Original Java test body:
            for( String profile : CPU_PROFILES ) {
                // LDA #$01 ; BEQ +1 => branch not taken: 2 cycles => 1 subcycle
                CpuEnv env = createEnv(profile);
                loadProgram(env, 0xA9, 0x01, 0xF0, 0x01);
                runInstruction(env); // reset
                runInstruction(env); // LDA #$01 (Z clear)
                int subNotTaken = runInstructionAndCountSubcycles(env);
                assertEquals(1, subNotTaken);
                assertEquals(2, env.cpu.getLastInstructionCycleCount());
    
                // LDA #$00 ; BEQ +1 => taken same page: 3 cycles => 2 subcycles
                env = createEnv(profile);
                loadProgram(env, 0xA9, 0x00, 0xF0, 0x01);
                runInstruction(env); // reset
                runInstruction(env); // LDA #$00 (Z set)
                int subTakenSame = runInstructionAndCountSubcycles(env);
                assertEquals(2, subTakenSame);
                assertEquals(3, env.cpu.getLastInstructionCycleCount());
    
                // LDA #$00 ; BEQ -128 at $0202 => target crosses into $01xx page.
                env = createEnv(profile);
                loadProgram(env, 0xA9, 0x00, 0xF0, 0x80); // LDA #$00 ; BEQ -128
                runInstruction(env); // reset
                runInstruction(env); // LDA #$00 (Z set)
                int subTakenCross = runInstructionAndCountSubcycles(env);
                assertEquals(3, subTakenCross);
                assertEquals(4, env.cpu.getLastInstructionCycleCount());
            }
    
    */
}
