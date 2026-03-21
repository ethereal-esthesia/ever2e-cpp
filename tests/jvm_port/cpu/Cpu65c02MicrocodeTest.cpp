// Auto-generated JVM test port scaffold.
// Source: /Users/shane/Project/ever2e-jvm/src/test/cpu/Cpu65c02MicrocodeTest.java
// NOTE: Function bodies are placeholders; original Java test bodies are preserved below each test.

#include "../jvm_test_port_harness.h"

E2_PORT_TEST(Cpu65c02MicrocodeTest__microOpNamesUseMPrefix) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::microOpNamesUseMPrefix");
    /*
    Original Java test body:
    		for( MicroOp op : MicroOp.values() )
    			assertTrue(op.name().startsWith("M_"));
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__microOpsHaveStableSequentialCodes) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::microOpsHaveStableSequentialCodes");
    /*
    Original Java test body:
    		MicroOp[] ops = MicroOp.values();
    		for( int i = 0; i<ops.length; i++ )
    			assertEquals(i, ops[i].code());
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__ldaOpcodeEnumMatchesOpcodeByteList) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::ldaOpcodeEnumMatchesOpcodeByteList");
    /*
    Original Java test body:
    		Cpu65c02Opcode[] ldaOps = Cpu65c02Opcode.ldaFamily().toArray(new Cpu65c02Opcode[0]);
    		int[] ldaBytes = Cpu65c02Opcode.ldaOpcodeBytes();
    		assertEquals(ldaOps.length, ldaBytes.length);
    		for( int i = 0; i<ldaOps.length; i++ )
    			assertEquals(ldaOps[i].opcodeByte(), ldaBytes[i]);
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__ldaOpcodeEnumProgramsDriveResolvedMicrocode) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::ldaOpcodeEnumProgramsDriveResolvedMicrocode");
    /*
    Original Java test body:
    		for( Cpu65c02Opcode lda : Cpu65c02Opcode.ldaFamily() ) {
    			Cpu65c02OpcodeView entry = Cpu65c02Microcode.opcodeForByte(lda.opcodeByte());
    			assertEquals(lda.microcode().accessType(), entry.getAccessType());
    			assertArrayEquals(lda.microcode().noCrossScript(), entry.getExpectedMnemonicOrder(false));
    			assertArrayEquals(lda.microcode().crossScript(), entry.getExpectedMnemonicOrder(true));
    		}
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__staOpcodeEnumMatchesOpcodeByteList) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::staOpcodeEnumMatchesOpcodeByteList");
    /*
    Original Java test body:
    		Cpu65c02Opcode[] staOps = Cpu65c02Opcode.staFamily().toArray(new Cpu65c02Opcode[0]);
    		int[] staBytes = Cpu65c02Opcode.staOpcodeBytes();
    		assertEquals(staOps.length, staBytes.length);
    		for( int i = 0; i<staOps.length; i++ )
    			assertEquals(staOps[i].opcodeByte(), staBytes[i]);
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__staOpcodeEnumProgramsDriveResolvedMicrocode) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::staOpcodeEnumProgramsDriveResolvedMicrocode");
    /*
    Original Java test body:
    		for( Cpu65c02Opcode sta : Cpu65c02Opcode.staFamily() ) {
    			Cpu65c02OpcodeView entry = Cpu65c02Microcode.opcodeForByte(sta.opcodeByte());
    			assertEquals(sta.microcode().accessType(), entry.getAccessType());
    			assertArrayEquals(sta.microcode().noCrossScript(), entry.getExpectedMnemonicOrder(false));
    			assertArrayEquals(sta.microcode().crossScript(), entry.getExpectedMnemonicOrder(true));
    		}
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__incOpcodeEnumMatchesOpcodeByteList) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::incOpcodeEnumMatchesOpcodeByteList");
    /*
    Original Java test body:
    		Cpu65c02Opcode[] incOps = Cpu65c02Opcode.incFamily().toArray(new Cpu65c02Opcode[0]);
    		int[] incBytes = Cpu65c02Opcode.incOpcodeBytes();
    		assertEquals(incOps.length, incBytes.length);
    		for( int i = 0; i<incOps.length; i++ )
    			assertEquals(incOps[i].opcodeByte(), incBytes[i]);
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__incOpcodeEnumProgramsDriveResolvedMicrocode) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::incOpcodeEnumProgramsDriveResolvedMicrocode");
    /*
    Original Java test body:
    		for( Cpu65c02Opcode inc : Cpu65c02Opcode.incFamily() ) {
    			Cpu65c02OpcodeView entry = Cpu65c02Microcode.opcodeForByte(inc.opcodeByte());
    			assertEquals(inc.microcode().accessType(), entry.getAccessType());
    			assertArrayEquals(inc.microcode().noCrossScript(), entry.getExpectedMnemonicOrder(false));
    			assertArrayEquals(inc.microcode().crossScript(), entry.getExpectedMnemonicOrder(true));
    		}
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__decOpcodeEnumMatchesOpcodeByteList) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::decOpcodeEnumMatchesOpcodeByteList");
    /*
    Original Java test body:
    		Cpu65c02Opcode[] decOps = Cpu65c02Opcode.decFamily().toArray(new Cpu65c02Opcode[0]);
    		int[] decBytes = Cpu65c02Opcode.decOpcodeBytes();
    		assertEquals(decOps.length, decBytes.length);
    		for( int i = 0; i<decOps.length; i++ )
    			assertEquals(decOps[i].opcodeByte(), decBytes[i]);
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__decOpcodeEnumProgramsDriveResolvedMicrocode) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::decOpcodeEnumProgramsDriveResolvedMicrocode");
    /*
    Original Java test body:
    		for( Cpu65c02Opcode dec : Cpu65c02Opcode.decFamily() ) {
    			Cpu65c02OpcodeView entry = Cpu65c02Microcode.opcodeForByte(dec.opcodeByte());
    			assertEquals(dec.microcode().accessType(), entry.getAccessType());
    			assertArrayEquals(dec.microcode().noCrossScript(), entry.getExpectedMnemonicOrder(false));
    			assertArrayEquals(dec.microcode().crossScript(), entry.getExpectedMnemonicOrder(true));
    		}
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__aslOpcodeEnumMatchesOpcodeByteList) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::aslOpcodeEnumMatchesOpcodeByteList");
    /*
    Original Java test body:
    		Cpu65c02Opcode[] aslOps = Cpu65c02Opcode.aslFamily().toArray(new Cpu65c02Opcode[0]);
    		int[] aslBytes = Cpu65c02Opcode.aslOpcodeBytes();
    		assertEquals(aslOps.length, aslBytes.length);
    		for( int i = 0; i<aslOps.length; i++ )
    			assertEquals(aslOps[i].opcodeByte(), aslBytes[i]);
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__aslOpcodeEnumProgramsDriveResolvedMicrocode) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::aslOpcodeEnumProgramsDriveResolvedMicrocode");
    /*
    Original Java test body:
    		for( Cpu65c02Opcode asl : Cpu65c02Opcode.aslFamily() ) {
    			Cpu65c02OpcodeView entry = Cpu65c02Microcode.opcodeForByte(asl.opcodeByte());
    			assertEquals(asl.microcode().accessType(), entry.getAccessType());
    			assertArrayEquals(asl.microcode().noCrossScript(), entry.getExpectedMnemonicOrder(false));
    			assertArrayEquals(asl.microcode().crossScript(), entry.getExpectedMnemonicOrder(true));
    		}
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__lsrOpcodeEnumMatchesOpcodeByteList) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::lsrOpcodeEnumMatchesOpcodeByteList");
    /*
    Original Java test body:
    		Cpu65c02Opcode[] lsrOps = Cpu65c02Opcode.lsrFamily().toArray(new Cpu65c02Opcode[0]);
    		int[] lsrBytes = Cpu65c02Opcode.lsrOpcodeBytes();
    		assertEquals(lsrOps.length, lsrBytes.length);
    		for( int i = 0; i<lsrOps.length; i++ )
    			assertEquals(lsrOps[i].opcodeByte(), lsrBytes[i]);
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__lsrOpcodeEnumProgramsDriveResolvedMicrocode) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::lsrOpcodeEnumProgramsDriveResolvedMicrocode");
    /*
    Original Java test body:
    		for( Cpu65c02Opcode lsr : Cpu65c02Opcode.lsrFamily() ) {
    			Cpu65c02OpcodeView entry = Cpu65c02Microcode.opcodeForByte(lsr.opcodeByte());
    			assertEquals(lsr.microcode().accessType(), entry.getAccessType());
    			assertArrayEquals(lsr.microcode().noCrossScript(), entry.getExpectedMnemonicOrder(false));
    			assertArrayEquals(lsr.microcode().crossScript(), entry.getExpectedMnemonicOrder(true));
    		}
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__rolOpcodeEnumMatchesOpcodeByteList) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::rolOpcodeEnumMatchesOpcodeByteList");
    /*
    Original Java test body:
    		Cpu65c02Opcode[] rolOps = Cpu65c02Opcode.rolFamily().toArray(new Cpu65c02Opcode[0]);
    		int[] rolBytes = Cpu65c02Opcode.rolOpcodeBytes();
    		assertEquals(rolOps.length, rolBytes.length);
    		for( int i = 0; i<rolOps.length; i++ )
    			assertEquals(rolOps[i].opcodeByte(), rolBytes[i]);
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__rolOpcodeEnumProgramsDriveResolvedMicrocode) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::rolOpcodeEnumProgramsDriveResolvedMicrocode");
    /*
    Original Java test body:
    		for( Cpu65c02Opcode rol : Cpu65c02Opcode.rolFamily() ) {
    			Cpu65c02OpcodeView entry = Cpu65c02Microcode.opcodeForByte(rol.opcodeByte());
    			assertEquals(rol.microcode().accessType(), entry.getAccessType());
    			assertArrayEquals(rol.microcode().noCrossScript(), entry.getExpectedMnemonicOrder(false));
    			assertArrayEquals(rol.microcode().crossScript(), entry.getExpectedMnemonicOrder(true));
    		}
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__rorOpcodeEnumMatchesOpcodeByteList) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::rorOpcodeEnumMatchesOpcodeByteList");
    /*
    Original Java test body:
    		Cpu65c02Opcode[] rorOps = Cpu65c02Opcode.rorFamily().toArray(new Cpu65c02Opcode[0]);
    		int[] rorBytes = Cpu65c02Opcode.rorOpcodeBytes();
    		assertEquals(rorOps.length, rorBytes.length);
    		for( int i = 0; i<rorOps.length; i++ )
    			assertEquals(rorOps[i].opcodeByte(), rorBytes[i]);
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__rorOpcodeEnumProgramsDriveResolvedMicrocode) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::rorOpcodeEnumProgramsDriveResolvedMicrocode");
    /*
    Original Java test body:
    		for( Cpu65c02Opcode ror : Cpu65c02Opcode.rorFamily() ) {
    			Cpu65c02OpcodeView entry = Cpu65c02Microcode.opcodeForByte(ror.opcodeByte());
    			assertEquals(ror.microcode().accessType(), entry.getAccessType());
    			assertArrayEquals(ror.microcode().noCrossScript(), entry.getExpectedMnemonicOrder(false));
    			assertArrayEquals(ror.microcode().crossScript(), entry.getExpectedMnemonicOrder(true));
    		}
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__oraOpcodeEnumMatchesOpcodeByteList) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::oraOpcodeEnumMatchesOpcodeByteList");
    /*
    Original Java test body:
    		Cpu65c02Opcode[] oraOps = Cpu65c02Opcode.oraFamily().toArray(new Cpu65c02Opcode[0]);
    		int[] oraBytes = Cpu65c02Opcode.oraOpcodeBytes();
    		assertEquals(oraOps.length, oraBytes.length);
    		for( int i = 0; i<oraOps.length; i++ )
    			assertEquals(oraOps[i].opcodeByte(), oraBytes[i]);
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__oraOpcodeEnumProgramsDriveResolvedMicrocode) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::oraOpcodeEnumProgramsDriveResolvedMicrocode");
    /*
    Original Java test body:
    		for( Cpu65c02Opcode ora : Cpu65c02Opcode.oraFamily() ) {
    			Cpu65c02OpcodeView entry = Cpu65c02Microcode.opcodeForByte(ora.opcodeByte());
    			assertEquals(ora.microcode().accessType(), entry.getAccessType());
    			assertArrayEquals(ora.microcode().noCrossScript(), entry.getExpectedMnemonicOrder(false));
    			assertArrayEquals(ora.microcode().crossScript(), entry.getExpectedMnemonicOrder(true));
    		}
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__andOpcodeEnumMatchesOpcodeByteList) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::andOpcodeEnumMatchesOpcodeByteList");
    /*
    Original Java test body:
    		Cpu65c02Opcode[] andOps = Cpu65c02Opcode.andFamily().toArray(new Cpu65c02Opcode[0]);
    		int[] andBytes = Cpu65c02Opcode.andOpcodeBytes();
    		assertEquals(andOps.length, andBytes.length);
    		for( int i = 0; i<andOps.length; i++ )
    			assertEquals(andOps[i].opcodeByte(), andBytes[i]);
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__andOpcodeEnumProgramsDriveResolvedMicrocode) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::andOpcodeEnumProgramsDriveResolvedMicrocode");
    /*
    Original Java test body:
    		for( Cpu65c02Opcode and : Cpu65c02Opcode.andFamily() ) {
    			Cpu65c02OpcodeView entry = Cpu65c02Microcode.opcodeForByte(and.opcodeByte());
    			assertEquals(and.microcode().accessType(), entry.getAccessType());
    			assertArrayEquals(and.microcode().noCrossScript(), entry.getExpectedMnemonicOrder(false));
    			assertArrayEquals(and.microcode().crossScript(), entry.getExpectedMnemonicOrder(true));
    		}
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__eorOpcodeEnumMatchesOpcodeByteList) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::eorOpcodeEnumMatchesOpcodeByteList");
    /*
    Original Java test body:
    		Cpu65c02Opcode[] eorOps = Cpu65c02Opcode.eorFamily().toArray(new Cpu65c02Opcode[0]);
    		int[] eorBytes = Cpu65c02Opcode.eorOpcodeBytes();
    		assertEquals(eorOps.length, eorBytes.length);
    		for( int i = 0; i<eorOps.length; i++ )
    			assertEquals(eorOps[i].opcodeByte(), eorBytes[i]);
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__eorOpcodeEnumProgramsDriveResolvedMicrocode) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::eorOpcodeEnumProgramsDriveResolvedMicrocode");
    /*
    Original Java test body:
    		for( Cpu65c02Opcode eor : Cpu65c02Opcode.eorFamily() ) {
    			Cpu65c02OpcodeView entry = Cpu65c02Microcode.opcodeForByte(eor.opcodeByte());
    			assertEquals(eor.microcode().accessType(), entry.getAccessType());
    			assertArrayEquals(eor.microcode().noCrossScript(), entry.getExpectedMnemonicOrder(false));
    			assertArrayEquals(eor.microcode().crossScript(), entry.getExpectedMnemonicOrder(true));
    		}
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__adcOpcodeEnumMatchesOpcodeByteList) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::adcOpcodeEnumMatchesOpcodeByteList");
    /*
    Original Java test body:
    		Cpu65c02Opcode[] adcOps = Cpu65c02Opcode.adcFamily().toArray(new Cpu65c02Opcode[0]);
    		int[] adcBytes = Cpu65c02Opcode.adcOpcodeBytes();
    		assertEquals(adcOps.length, adcBytes.length);
    		for( int i = 0; i<adcOps.length; i++ )
    			assertEquals(adcOps[i].opcodeByte(), adcBytes[i]);
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__adcOpcodeEnumProgramsDriveResolvedMicrocode) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::adcOpcodeEnumProgramsDriveResolvedMicrocode");
    /*
    Original Java test body:
    		for( Cpu65c02Opcode adc : Cpu65c02Opcode.adcFamily() ) {
    			Cpu65c02OpcodeView entry = Cpu65c02Microcode.opcodeForByte(adc.opcodeByte());
    			assertEquals(adc.microcode().accessType(), entry.getAccessType());
    			assertArrayEquals(adc.microcode().noCrossScript(), entry.getExpectedMnemonicOrder(false));
    			assertArrayEquals(adc.microcode().crossScript(), entry.getExpectedMnemonicOrder(true));
    		}
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__sbcOpcodeEnumMatchesOpcodeByteList) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::sbcOpcodeEnumMatchesOpcodeByteList");
    /*
    Original Java test body:
    		Cpu65c02Opcode[] sbcOps = Cpu65c02Opcode.sbcFamily().toArray(new Cpu65c02Opcode[0]);
    		int[] sbcBytes = Cpu65c02Opcode.sbcOpcodeBytes();
    		assertEquals(sbcOps.length, sbcBytes.length);
    		for( int i = 0; i<sbcOps.length; i++ )
    			assertEquals(sbcOps[i].opcodeByte(), sbcBytes[i]);
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__sbcOpcodeEnumProgramsDriveResolvedMicrocode) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::sbcOpcodeEnumProgramsDriveResolvedMicrocode");
    /*
    Original Java test body:
    		for( Cpu65c02Opcode sbc : Cpu65c02Opcode.sbcFamily() ) {
    			Cpu65c02OpcodeView entry = Cpu65c02Microcode.opcodeForByte(sbc.opcodeByte());
    			assertEquals(sbc.microcode().accessType(), entry.getAccessType());
    			assertArrayEquals(sbc.microcode().noCrossScript(), entry.getExpectedMnemonicOrder(false));
    			assertArrayEquals(sbc.microcode().crossScript(), entry.getExpectedMnemonicOrder(true));
    		}
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__cmpOpcodeEnumMatchesOpcodeByteList) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::cmpOpcodeEnumMatchesOpcodeByteList");
    /*
    Original Java test body:
    		Cpu65c02Opcode[] cmpOps = Cpu65c02Opcode.cmpFamily().toArray(new Cpu65c02Opcode[0]);
    		int[] cmpBytes = Cpu65c02Opcode.cmpOpcodeBytes();
    		assertEquals(cmpOps.length, cmpBytes.length);
    		for( int i = 0; i<cmpOps.length; i++ )
    			assertEquals(cmpOps[i].opcodeByte(), cmpBytes[i]);
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__cmpOpcodeEnumProgramsDriveResolvedMicrocode) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::cmpOpcodeEnumProgramsDriveResolvedMicrocode");
    /*
    Original Java test body:
    		for( Cpu65c02Opcode cmp : Cpu65c02Opcode.cmpFamily() ) {
    			Cpu65c02OpcodeView entry = Cpu65c02Microcode.opcodeForByte(cmp.opcodeByte());
    			assertEquals(cmp.microcode().accessType(), entry.getAccessType());
    			assertArrayEquals(cmp.microcode().noCrossScript(), entry.getExpectedMnemonicOrder(false));
    			assertArrayEquals(cmp.microcode().crossScript(), entry.getExpectedMnemonicOrder(true));
    		}
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__bitOpcodeEnumMatchesOpcodeByteList) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::bitOpcodeEnumMatchesOpcodeByteList");
    /*
    Original Java test body:
    		Cpu65c02Opcode[] bitOps = Cpu65c02Opcode.bitFamily().toArray(new Cpu65c02Opcode[0]);
    		int[] bitBytes = Cpu65c02Opcode.bitOpcodeBytes();
    		assertEquals(bitOps.length, bitBytes.length);
    		for( int i = 0; i<bitOps.length; i++ )
    			assertEquals(bitOps[i].opcodeByte(), bitBytes[i]);
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__bitOpcodeEnumProgramsDriveResolvedMicrocode) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::bitOpcodeEnumProgramsDriveResolvedMicrocode");
    /*
    Original Java test body:
    		for( Cpu65c02Opcode bit : Cpu65c02Opcode.bitFamily() ) {
    			Cpu65c02OpcodeView entry = Cpu65c02Microcode.opcodeForByte(bit.opcodeByte());
    			assertEquals(bit.microcode().accessType(), entry.getAccessType());
    			assertArrayEquals(bit.microcode().noCrossScript(), entry.getExpectedMnemonicOrder(false));
    			assertArrayEquals(bit.microcode().crossScript(), entry.getExpectedMnemonicOrder(true));
    		}
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__ldxOpcodeEnumMatchesOpcodeByteList) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::ldxOpcodeEnumMatchesOpcodeByteList");
    /*
    Original Java test body:
    		Cpu65c02Opcode[] ldxOps = Cpu65c02Opcode.ldxFamily().toArray(new Cpu65c02Opcode[0]);
    		int[] ldxBytes = Cpu65c02Opcode.ldxOpcodeBytes();
    		assertEquals(ldxOps.length, ldxBytes.length);
    		for( int i = 0; i<ldxOps.length; i++ )
    			assertEquals(ldxOps[i].opcodeByte(), ldxBytes[i]);
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__ldxOpcodeEnumProgramsDriveResolvedMicrocode) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::ldxOpcodeEnumProgramsDriveResolvedMicrocode");
    /*
    Original Java test body:
    		for( Cpu65c02Opcode ldx : Cpu65c02Opcode.ldxFamily() ) {
    			Cpu65c02OpcodeView entry = Cpu65c02Microcode.opcodeForByte(ldx.opcodeByte());
    			assertEquals(ldx.microcode().accessType(), entry.getAccessType());
    			assertArrayEquals(ldx.microcode().noCrossScript(), entry.getExpectedMnemonicOrder(false));
    			assertArrayEquals(ldx.microcode().crossScript(), entry.getExpectedMnemonicOrder(true));
    		}
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__ldyOpcodeEnumMatchesOpcodeByteList) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::ldyOpcodeEnumMatchesOpcodeByteList");
    /*
    Original Java test body:
    		Cpu65c02Opcode[] ldyOps = Cpu65c02Opcode.ldyFamily().toArray(new Cpu65c02Opcode[0]);
    		int[] ldyBytes = Cpu65c02Opcode.ldyOpcodeBytes();
    		assertEquals(ldyOps.length, ldyBytes.length);
    		for( int i = 0; i<ldyOps.length; i++ )
    			assertEquals(ldyOps[i].opcodeByte(), ldyBytes[i]);
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__ldyOpcodeEnumProgramsDriveResolvedMicrocode) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::ldyOpcodeEnumProgramsDriveResolvedMicrocode");
    /*
    Original Java test body:
    		for( Cpu65c02Opcode ldy : Cpu65c02Opcode.ldyFamily() ) {
    			Cpu65c02OpcodeView entry = Cpu65c02Microcode.opcodeForByte(ldy.opcodeByte());
    			assertEquals(ldy.microcode().accessType(), entry.getAccessType());
    			assertArrayEquals(ldy.microcode().noCrossScript(), entry.getExpectedMnemonicOrder(false));
    			assertArrayEquals(ldy.microcode().crossScript(), entry.getExpectedMnemonicOrder(true));
    		}
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__stxOpcodeEnumMatchesOpcodeByteList) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::stxOpcodeEnumMatchesOpcodeByteList");
    /*
    Original Java test body:
    		Cpu65c02Opcode[] stxOps = Cpu65c02Opcode.stxFamily().toArray(new Cpu65c02Opcode[0]);
    		int[] stxBytes = Cpu65c02Opcode.stxOpcodeBytes();
    		assertEquals(stxOps.length, stxBytes.length);
    		for( int i = 0; i<stxOps.length; i++ )
    			assertEquals(stxOps[i].opcodeByte(), stxBytes[i]);
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__stxOpcodeEnumProgramsDriveResolvedMicrocode) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::stxOpcodeEnumProgramsDriveResolvedMicrocode");
    /*
    Original Java test body:
    		for( Cpu65c02Opcode stx : Cpu65c02Opcode.stxFamily() ) {
    			Cpu65c02OpcodeView entry = Cpu65c02Microcode.opcodeForByte(stx.opcodeByte());
    			assertEquals(stx.microcode().accessType(), entry.getAccessType());
    			assertArrayEquals(stx.microcode().noCrossScript(), entry.getExpectedMnemonicOrder(false));
    			assertArrayEquals(stx.microcode().crossScript(), entry.getExpectedMnemonicOrder(true));
    		}
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__styOpcodeEnumMatchesOpcodeByteList) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::styOpcodeEnumMatchesOpcodeByteList");
    /*
    Original Java test body:
    		Cpu65c02Opcode[] styOps = Cpu65c02Opcode.styFamily().toArray(new Cpu65c02Opcode[0]);
    		int[] styBytes = Cpu65c02Opcode.styOpcodeBytes();
    		assertEquals(styOps.length, styBytes.length);
    		for( int i = 0; i<styOps.length; i++ )
    			assertEquals(styOps[i].opcodeByte(), styBytes[i]);
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__styOpcodeEnumProgramsDriveResolvedMicrocode) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::styOpcodeEnumProgramsDriveResolvedMicrocode");
    /*
    Original Java test body:
    		for( Cpu65c02Opcode sty : Cpu65c02Opcode.styFamily() ) {
    			Cpu65c02OpcodeView entry = Cpu65c02Microcode.opcodeForByte(sty.opcodeByte());
    			assertEquals(sty.microcode().accessType(), entry.getAccessType());
    			assertArrayEquals(sty.microcode().noCrossScript(), entry.getExpectedMnemonicOrder(false));
    			assertArrayEquals(sty.microcode().crossScript(), entry.getExpectedMnemonicOrder(true));
    		}
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__cpxOpcodeEnumMatchesOpcodeByteList) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::cpxOpcodeEnumMatchesOpcodeByteList");
    /*
    Original Java test body:
    		Cpu65c02Opcode[] cpxOps = Cpu65c02Opcode.cpxFamily().toArray(new Cpu65c02Opcode[0]);
    		int[] cpxBytes = Cpu65c02Opcode.cpxOpcodeBytes();
    		assertEquals(cpxOps.length, cpxBytes.length);
    		for( int i = 0; i<cpxOps.length; i++ )
    			assertEquals(cpxOps[i].opcodeByte(), cpxBytes[i]);
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__cpxOpcodeEnumProgramsDriveResolvedMicrocode) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::cpxOpcodeEnumProgramsDriveResolvedMicrocode");
    /*
    Original Java test body:
    		for( Cpu65c02Opcode cpx : Cpu65c02Opcode.cpxFamily() ) {
    			Cpu65c02OpcodeView entry = Cpu65c02Microcode.opcodeForByte(cpx.opcodeByte());
    			assertEquals(cpx.microcode().accessType(), entry.getAccessType());
    			assertArrayEquals(cpx.microcode().noCrossScript(), entry.getExpectedMnemonicOrder(false));
    			assertArrayEquals(cpx.microcode().crossScript(), entry.getExpectedMnemonicOrder(true));
    		}
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__cpyOpcodeEnumMatchesOpcodeByteList) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::cpyOpcodeEnumMatchesOpcodeByteList");
    /*
    Original Java test body:
    		Cpu65c02Opcode[] cpyOps = Cpu65c02Opcode.cpyFamily().toArray(new Cpu65c02Opcode[0]);
    		int[] cpyBytes = Cpu65c02Opcode.cpyOpcodeBytes();
    		assertEquals(cpyOps.length, cpyBytes.length);
    		for( int i = 0; i<cpyOps.length; i++ )
    			assertEquals(cpyOps[i].opcodeByte(), cpyBytes[i]);
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__cpyOpcodeEnumProgramsDriveResolvedMicrocode) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::cpyOpcodeEnumProgramsDriveResolvedMicrocode");
    /*
    Original Java test body:
    		for( Cpu65c02Opcode cpy : Cpu65c02Opcode.cpyFamily() ) {
    			Cpu65c02OpcodeView entry = Cpu65c02Microcode.opcodeForByte(cpy.opcodeByte());
    			assertEquals(cpy.microcode().accessType(), entry.getAccessType());
    			assertArrayEquals(cpy.microcode().noCrossScript(), entry.getExpectedMnemonicOrder(false));
    			assertArrayEquals(cpy.microcode().crossScript(), entry.getExpectedMnemonicOrder(true));
    		}
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__jumpOpcodeEnumMatchesOpcodeByteList) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::jumpOpcodeEnumMatchesOpcodeByteList");
    /*
    Original Java test body:
    		Cpu65c02Opcode[] ops = Cpu65c02Opcode.jumpFamily().toArray(new Cpu65c02Opcode[0]);
    		int[] bytes = Cpu65c02Opcode.jumpOpcodeBytes();
    		assertEquals(ops.length, bytes.length);
    		for( int i = 0; i<ops.length; i++ )
    			assertEquals(ops[i].opcodeByte(), bytes[i]);
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__jumpOpcodeEnumProgramsDriveResolvedMicrocode) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::jumpOpcodeEnumProgramsDriveResolvedMicrocode");
    /*
    Original Java test body:
    		for( Cpu65c02Opcode opcode : Cpu65c02Opcode.jumpFamily() ) {
    			Cpu65c02OpcodeView entry = Cpu65c02Microcode.opcodeForByte(opcode.opcodeByte());
    			assertEquals(opcode.microcode().accessType(), entry.getAccessType());
    			assertArrayEquals(opcode.microcode().noCrossScript(), entry.getExpectedMnemonicOrder(false));
    			assertArrayEquals(opcode.microcode().crossScript(), entry.getExpectedMnemonicOrder(true));
    		}
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__bitTestSetOpcodeEnumMatchesOpcodeByteList) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::bitTestSetOpcodeEnumMatchesOpcodeByteList");
    /*
    Original Java test body:
    		Cpu65c02Opcode[] ops = Cpu65c02Opcode.bitTestSetFamily().toArray(new Cpu65c02Opcode[0]);
    		int[] bytes = Cpu65c02Opcode.bitTestSetOpcodeBytes();
    		assertEquals(ops.length, bytes.length);
    		for( int i = 0; i<ops.length; i++ )
    			assertEquals(ops[i].opcodeByte(), bytes[i]);
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__bitTestSetOpcodeEnumProgramsDriveResolvedMicrocode) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::bitTestSetOpcodeEnumProgramsDriveResolvedMicrocode");
    /*
    Original Java test body:
    		for( Cpu65c02Opcode opcode : Cpu65c02Opcode.bitTestSetFamily() ) {
    			Cpu65c02OpcodeView entry = Cpu65c02Microcode.opcodeForByte(opcode.opcodeByte());
    			assertEquals(opcode.microcode().accessType(), entry.getAccessType());
    			assertArrayEquals(opcode.microcode().noCrossScript(), entry.getExpectedMnemonicOrder(false));
    			assertArrayEquals(opcode.microcode().crossScript(), entry.getExpectedMnemonicOrder(true));
    		}
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__transferOpcodeEnumMatchesOpcodeByteList) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::transferOpcodeEnumMatchesOpcodeByteList");
    /*
    Original Java test body:
    		Cpu65c02Opcode[] ops = Cpu65c02Opcode.transferFamily().toArray(new Cpu65c02Opcode[0]);
    		int[] bytes = Cpu65c02Opcode.transferOpcodeBytes();
    		assertEquals(ops.length, bytes.length);
    		for( int i = 0; i<ops.length; i++ )
    			assertEquals(ops[i].opcodeByte(), bytes[i]);
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__transferOpcodeEnumProgramsDriveResolvedMicrocode) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::transferOpcodeEnumProgramsDriveResolvedMicrocode");
    /*
    Original Java test body:
    		for( Cpu65c02Opcode opcode : Cpu65c02Opcode.transferFamily() ) {
    			Cpu65c02OpcodeView entry = Cpu65c02Microcode.opcodeForByte(opcode.opcodeByte());
    			assertEquals(opcode.microcode().accessType(), entry.getAccessType());
    			assertArrayEquals(opcode.microcode().noCrossScript(), entry.getExpectedMnemonicOrder(false));
    			assertArrayEquals(opcode.microcode().crossScript(), entry.getExpectedMnemonicOrder(true));
    		}
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__indexIncDecOpcodeEnumMatchesOpcodeByteList) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::indexIncDecOpcodeEnumMatchesOpcodeByteList");
    /*
    Original Java test body:
    		Cpu65c02Opcode[] ops = Cpu65c02Opcode.indexIncDecFamily().toArray(new Cpu65c02Opcode[0]);
    		int[] bytes = Cpu65c02Opcode.indexIncDecOpcodeBytes();
    		assertEquals(ops.length, bytes.length);
    		for( int i = 0; i<ops.length; i++ )
    			assertEquals(ops[i].opcodeByte(), bytes[i]);
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__indexIncDecOpcodeEnumProgramsDriveResolvedMicrocode) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::indexIncDecOpcodeEnumProgramsDriveResolvedMicrocode");
    /*
    Original Java test body:
    		for( Cpu65c02Opcode opcode : Cpu65c02Opcode.indexIncDecFamily() ) {
    			Cpu65c02OpcodeView entry = Cpu65c02Microcode.opcodeForByte(opcode.opcodeByte());
    			assertEquals(opcode.microcode().accessType(), entry.getAccessType());
    			assertArrayEquals(opcode.microcode().noCrossScript(), entry.getExpectedMnemonicOrder(false));
    			assertArrayEquals(opcode.microcode().crossScript(), entry.getExpectedMnemonicOrder(true));
    		}
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__stzOpcodeEnumMatchesOpcodeByteList) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::stzOpcodeEnumMatchesOpcodeByteList");
    /*
    Original Java test body:
    		Cpu65c02Opcode[] ops = Cpu65c02Opcode.stzFamily().toArray(new Cpu65c02Opcode[0]);
    		int[] bytes = Cpu65c02Opcode.stzOpcodeBytes();
    		assertEquals(ops.length, bytes.length);
    		for( int i = 0; i<ops.length; i++ )
    			assertEquals(ops[i].opcodeByte(), bytes[i]);
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__stzOpcodeEnumProgramsDriveResolvedMicrocode) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::stzOpcodeEnumProgramsDriveResolvedMicrocode");
    /*
    Original Java test body:
    		for( Cpu65c02Opcode opcode : Cpu65c02Opcode.stzFamily() ) {
    			Cpu65c02OpcodeView entry = Cpu65c02Microcode.opcodeForByte(opcode.opcodeByte());
    			assertEquals(opcode.microcode().accessType(), entry.getAccessType());
    			assertArrayEquals(opcode.microcode().noCrossScript(), entry.getExpectedMnemonicOrder(false));
    			assertArrayEquals(opcode.microcode().crossScript(), entry.getExpectedMnemonicOrder(true));
    		}
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__interruptControlFlowOpcodeEnumMatchesOpcodeByteList) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::interruptControlFlowOpcodeEnumMatchesOpcodeByteList");
    /*
    Original Java test body:
    		Cpu65c02Opcode[] ops = Cpu65c02Opcode.interruptControlFlowFamily().toArray(new Cpu65c02Opcode[0]);
    		int[] bytes = Cpu65c02Opcode.interruptControlFlowOpcodeBytes();
    		assertEquals(ops.length, bytes.length);
    		for( int i = 0; i<ops.length; i++ )
    			assertEquals(ops[i].opcodeByte(), bytes[i]);
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__interruptControlFlowOpcodeEnumProgramsDriveResolvedMicrocode) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::interruptControlFlowOpcodeEnumProgramsDriveResolvedMicrocode");
    /*
    Original Java test body:
    		for( Cpu65c02Opcode opcode : Cpu65c02Opcode.interruptControlFlowFamily() ) {
    			Cpu65c02OpcodeView entry = Cpu65c02Microcode.opcodeForByte(opcode.opcodeByte());
    			assertEquals(opcode.microcode().accessType(), entry.getAccessType());
    			assertArrayEquals(opcode.microcode().noCrossScript(), entry.getExpectedMnemonicOrder(false));
    			assertArrayEquals(opcode.microcode().crossScript(), entry.getExpectedMnemonicOrder(true));
    		}
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__readModifyWriteOpcodeEnumMatchesOpcodeByteList) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::readModifyWriteOpcodeEnumMatchesOpcodeByteList");
    /*
    Original Java test body:
    		Cpu65c02Opcode[] ops = Cpu65c02Opcode.readModifyWriteFamily().toArray(new Cpu65c02Opcode[0]);
    		int[] bytes = Cpu65c02Opcode.readModifyWriteOpcodeBytes();
    		assertEquals(ops.length, bytes.length);
    		for( int i = 0; i<ops.length; i++ )
    			assertEquals(ops[i].opcodeByte(), bytes[i]);
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__readModifyWriteOpcodeEnumProgramsDriveResolvedMicrocode) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::readModifyWriteOpcodeEnumProgramsDriveResolvedMicrocode");
    /*
    Original Java test body:
    		for( Cpu65c02Opcode opcode : Cpu65c02Opcode.readModifyWriteFamily() ) {
    			Cpu65c02OpcodeView entry = Cpu65c02Microcode.opcodeForByte(opcode.opcodeByte());
    			assertEquals(opcode.microcode().accessType(), entry.getAccessType());
    			assertArrayEquals(opcode.microcode().noCrossScript(), entry.getExpectedMnemonicOrder(false));
    			assertArrayEquals(opcode.microcode().crossScript(), entry.getExpectedMnemonicOrder(true));
    		}
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__logicalArithmeticOpcodeEnumMatchesOpcodeByteList) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::logicalArithmeticOpcodeEnumMatchesOpcodeByteList");
    /*
    Original Java test body:
    		Cpu65c02Opcode[] ops = Cpu65c02Opcode.logicalArithmeticFamily().toArray(new Cpu65c02Opcode[0]);
    		int[] bytes = Cpu65c02Opcode.logicalArithmeticOpcodeBytes();
    		assertEquals(ops.length, bytes.length);
    		for( int i = 0; i<ops.length; i++ )
    			assertEquals(ops[i].opcodeByte(), bytes[i]);
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__logicalArithmeticOpcodeEnumProgramsDriveResolvedMicrocode) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::logicalArithmeticOpcodeEnumProgramsDriveResolvedMicrocode");
    /*
    Original Java test body:
    		for( Cpu65c02Opcode opcode : Cpu65c02Opcode.logicalArithmeticFamily() ) {
    			Cpu65c02OpcodeView entry = Cpu65c02Microcode.opcodeForByte(opcode.opcodeByte());
    			assertEquals(opcode.microcode().accessType(), entry.getAccessType());
    			assertArrayEquals(opcode.microcode().noCrossScript(), entry.getExpectedMnemonicOrder(false));
    			assertArrayEquals(opcode.microcode().crossScript(), entry.getExpectedMnemonicOrder(true));
    		}
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__stackOpcodeEnumMatchesOpcodeByteList) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::stackOpcodeEnumMatchesOpcodeByteList");
    /*
    Original Java test body:
    		Cpu65c02Opcode[] ops = Cpu65c02Opcode.stackFamily().toArray(new Cpu65c02Opcode[0]);
    		int[] bytes = Cpu65c02Opcode.stackOpcodeBytes();
    		assertEquals(ops.length, bytes.length);
    		for( int i = 0; i<ops.length; i++ )
    			assertEquals(ops[i].opcodeByte(), bytes[i]);
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__stackOpcodeEnumProgramsDriveResolvedMicrocode) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::stackOpcodeEnumProgramsDriveResolvedMicrocode");
    /*
    Original Java test body:
    		for( Cpu65c02Opcode opcode : Cpu65c02Opcode.stackFamily() ) {
    			Cpu65c02OpcodeView entry = Cpu65c02Microcode.opcodeForByte(opcode.opcodeByte());
    			assertEquals(opcode.microcode().accessType(), entry.getAccessType());
    			assertArrayEquals(opcode.microcode().noCrossScript(), entry.getExpectedMnemonicOrder(false));
    			assertArrayEquals(opcode.microcode().crossScript(), entry.getExpectedMnemonicOrder(true));
    		}
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__flagsOpcodeEnumMatchesOpcodeByteList) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::flagsOpcodeEnumMatchesOpcodeByteList");
    /*
    Original Java test body:
    		Cpu65c02Opcode[] ops = Cpu65c02Opcode.flagsFamily().toArray(new Cpu65c02Opcode[0]);
    		int[] bytes = Cpu65c02Opcode.flagsOpcodeBytes();
    		assertEquals(ops.length, bytes.length);
    		for( int i = 0; i<ops.length; i++ )
    			assertEquals(ops[i].opcodeByte(), bytes[i]);
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__flagsOpcodeEnumProgramsDriveResolvedMicrocode) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::flagsOpcodeEnumProgramsDriveResolvedMicrocode");
    /*
    Original Java test body:
    		for( Cpu65c02Opcode opcode : Cpu65c02Opcode.flagsFamily() ) {
    			Cpu65c02OpcodeView entry = Cpu65c02Microcode.opcodeForByte(opcode.opcodeByte());
    			assertEquals(opcode.microcode().accessType(), entry.getAccessType());
    			assertArrayEquals(opcode.microcode().noCrossScript(), entry.getExpectedMnemonicOrder(false));
    			assertArrayEquals(opcode.microcode().crossScript(), entry.getExpectedMnemonicOrder(true));
    		}
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__branchOpcodeEnumMatchesOpcodeByteList) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::branchOpcodeEnumMatchesOpcodeByteList");
    /*
    Original Java test body:
    		Cpu65c02Opcode[] branchOps = Cpu65c02Opcode.branchFamily().toArray(new Cpu65c02Opcode[0]);
    		int[] branchBytes = Cpu65c02Opcode.branchOpcodeBytes();
    		assertEquals(branchOps.length, branchBytes.length);
    		for( int i = 0; i<branchOps.length; i++ )
    			assertEquals(branchOps[i].opcodeByte(), branchBytes[i]);
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__branchOpcodeEnumProgramsDriveResolvedMicrocode) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::branchOpcodeEnumProgramsDriveResolvedMicrocode");
    /*
    Original Java test body:
    		for( Cpu65c02Opcode branch : Cpu65c02Opcode.branchFamily() ) {
    			Cpu65c02OpcodeView entry = Cpu65c02Microcode.opcodeForByte(branch.opcodeByte());
    			assertEquals(branch.microcode().accessType(), entry.getAccessType());
    			assertArrayEquals(branch.microcode().noCrossScript(), entry.getExpectedMnemonicOrder(false));
    			assertArrayEquals(branch.microcode().crossScript(), entry.getExpectedMnemonicOrder(true));
    		}
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__controlMiscOpcodeEnumMatchesOpcodeByteList) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::controlMiscOpcodeEnumMatchesOpcodeByteList");
    /*
    Original Java test body:
    		Cpu65c02Opcode[] ops = Cpu65c02Opcode.controlMiscFamily().toArray(new Cpu65c02Opcode[0]);
    		int[] bytes = Cpu65c02Opcode.controlMiscOpcodeBytes();
    		assertEquals(ops.length, bytes.length);
    		for( int i = 0; i<ops.length; i++ )
    			assertEquals(ops[i].opcodeByte(), bytes[i]);
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__controlMiscOpcodeEnumProgramsDriveResolvedMicrocode) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::controlMiscOpcodeEnumProgramsDriveResolvedMicrocode");
    /*
    Original Java test body:
    		for( Cpu65c02Opcode opcode : Cpu65c02Opcode.controlMiscFamily() ) {
    			Cpu65c02OpcodeView entry = Cpu65c02Microcode.opcodeForByte(opcode.opcodeByte());
    			assertEquals(opcode.microcode().accessType(), entry.getAccessType());
    			assertArrayEquals(opcode.microcode().noCrossScript(), entry.getExpectedMnemonicOrder(false));
    			assertArrayEquals(opcode.microcode().crossScript(), entry.getExpectedMnemonicOrder(true));
    		}
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__opcodeByteRoundTripsToEnum) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::opcodeByteRoundTripsToEnum");
    /*
    Original Java test body:
    		for( Cpu65c02Opcode lda : Cpu65c02Opcode.ldaFamily() )
    			assertEquals(lda, Cpu65c02Opcode.fromOpcodeByte(lda.opcodeByte()));
    		for( Cpu65c02Opcode sta : Cpu65c02Opcode.staFamily() )
    			assertEquals(sta, Cpu65c02Opcode.fromOpcodeByte(sta.opcodeByte()));
    		for( Cpu65c02Opcode inc : Cpu65c02Opcode.incFamily() )
    			assertEquals(inc, Cpu65c02Opcode.fromOpcodeByte(inc.opcodeByte()));
    		for( Cpu65c02Opcode dec : Cpu65c02Opcode.decFamily() )
    			assertEquals(dec, Cpu65c02Opcode.fromOpcodeByte(dec.opcodeByte()));
    		for( Cpu65c02Opcode asl : Cpu65c02Opcode.aslFamily() )
    			assertEquals(asl, Cpu65c02Opcode.fromOpcodeByte(asl.opcodeByte()));
    		for( Cpu65c02Opcode lsr : Cpu65c02Opcode.lsrFamily() )
    			assertEquals(lsr, Cpu65c02Opcode.fromOpcodeByte(lsr.opcodeByte()));
    		for( Cpu65c02Opcode rol : Cpu65c02Opcode.rolFamily() )
    			assertEquals(rol, Cpu65c02Opcode.fromOpcodeByte(rol.opcodeByte()));
    		for( Cpu65c02Opcode ror : Cpu65c02Opcode.rorFamily() )
    			assertEquals(ror, Cpu65c02Opcode.fromOpcodeByte(ror.opcodeByte()));
    		for( Cpu65c02Opcode ora : Cpu65c02Opcode.oraFamily() )
    			assertEquals(ora, Cpu65c02Opcode.fromOpcodeByte(ora.opcodeByte()));
    		for( Cpu65c02Opcode and : Cpu65c02Opcode.andFamily() )
    			assertEquals(and, Cpu65c02Opcode.fromOpcodeByte(and.opcodeByte()));
    		for( Cpu65c02Opcode eor : Cpu65c02Opcode.eorFamily() )
    			assertEquals(eor, Cpu65c02Opcode.fromOpcodeByte(eor.opcodeByte()));
    		for( Cpu65c02Opcode adc : Cpu65c02Opcode.adcFamily() )
    			assertEquals(adc, Cpu65c02Opcode.fromOpcodeByte(adc.opcodeByte()));
    		for( Cpu65c02Opcode sbc : Cpu65c02Opcode.sbcFamily() )
    			assertEquals(sbc, Cpu65c02Opcode.fromOpcodeByte(sbc.opcodeByte()));
    		for( Cpu65c02Opcode cmp : Cpu65c02Opcode.cmpFamily() )
    			assertEquals(cmp, Cpu65c02Opcode.fromOpcodeByte(cmp.opcodeByte()));
    		for( Cpu65c02Opcode bit : Cpu65c02Opcode.bitFamily() )
    			assertEquals(bit, Cpu65c02Opcode.fromOpcodeByte(bit.opcodeByte()));
    		for( Cpu65c02Opcode ldx : Cpu65c02Opcode.ldxFamily() )
    			assertEquals(ldx, Cpu65c02Opcode.fromOpcodeByte(ldx.opcodeByte()));
    		for( Cpu65c02Opcode ldy : Cpu65c02Opcode.ldyFamily() )
    			assertEquals(ldy, Cpu65c02Opcode.fromOpcodeByte(ldy.opcodeByte()));
    		for( Cpu65c02Opcode stx : Cpu65c02Opcode.stxFamily() )
    			assertEquals(stx, Cpu65c02Opcode.fromOpcodeByte(stx.opcodeByte()));
    		for( Cpu65c02Opcode sty : Cpu65c02Opcode.styFamily() )
    			assertEquals(sty, Cpu65c02Opcode.fromOpcodeByte(sty.opcodeByte()));
    		for( Cpu65c02Opcode cpx : Cpu65c02Opcode.cpxFamily() )
    			assertEquals(cpx, Cpu65c02Opcode.fromOpcodeByte(cpx.opcodeByte()));
    		for( Cpu65c02Opcode cpy : Cpu65c02Opcode.cpyFamily() )
    			assertEquals(cpy, Cpu65c02Opcode.fromOpcodeByte(cpy.opcodeByte()));
    		for( Cpu65c02Opcode opcode : Cpu65c02Opcode.jumpFamily() )
    			assertEquals(opcode, Cpu65c02Opcode.fromOpcodeByte(opcode.opcodeByte()));
    		for( Cpu65c02Opcode opcode : Cpu65c02Opcode.bitTestSetFamily() )
    			assertEquals(opcode, Cpu65c02Opcode.fromOpcodeByte(opcode.opcodeByte()));
    		for( Cpu65c02Opcode opcode : Cpu65c02Opcode.transferFamily() )
    			assertEquals(opcode, Cpu65c02Opcode.fromOpcodeByte(opcode.opcodeByte()));
    		for( Cpu65c02Opcode opcode : Cpu65c02Opcode.indexIncDecFamily() )
    			assertEquals(opcode, Cpu65c02Opcode.fromOpcodeByte(opcode.opcodeByte()));
    		for( Cpu65c02Opcode opcode : Cpu65c02Opcode.stzFamily() )
    			assertEquals(opcode, Cpu65c02Opcode.fromOpcodeByte(opcode.opcodeByte()));
    		for( Cpu65c02Opcode opcode : Cpu65c02Opcode.interruptControlFlowFamily() )
    			assertEquals(opcode, Cpu65c02Opcode.fromOpcodeByte(opcode.opcodeByte()));
    		for( Cpu65c02Opcode opcode : Cpu65c02Opcode.readModifyWriteFamily() )
    			assertEquals(opcode, Cpu65c02Opcode.fromOpcodeByte(opcode.opcodeByte()));
    		for( Cpu65c02Opcode opcode : Cpu65c02Opcode.logicalArithmeticFamily() )
    			assertEquals(opcode, Cpu65c02Opcode.fromOpcodeByte(opcode.opcodeByte()));
    		for( Cpu65c02Opcode opcode : Cpu65c02Opcode.stackFamily() )
    			assertEquals(opcode, Cpu65c02Opcode.fromOpcodeByte(opcode.opcodeByte()));
    		for( Cpu65c02Opcode opcode : Cpu65c02Opcode.flagsFamily() )
    			assertEquals(opcode, Cpu65c02Opcode.fromOpcodeByte(opcode.opcodeByte()));
    		for( Cpu65c02Opcode branch : Cpu65c02Opcode.branchFamily() )
    			assertEquals(branch, Cpu65c02Opcode.fromOpcodeByte(branch.opcodeByte()));
    		for( Cpu65c02Opcode opcode : Cpu65c02Opcode.controlMiscFamily() )
    			assertEquals(opcode, Cpu65c02Opcode.fromOpcodeByte(opcode.opcodeByte()));
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__ldaAbsoluteCycleScript) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::ldaAbsoluteCycleScript");
    /*
    Original Java test body:
    		Cpu65c02OpcodeView instr = Cpu65c02Microcode.opcodeForByte(0xAD); // LDA abs
    		assertEquals(MicroOp.M_FETCH_OPCODE, instr.getExpectedMnemonicOrder(false)[0]);
    		assertEquals(MicroOp.M_FETCH_OPERAND_LO, instr.getExpectedMnemonicOrder(false)[1]);
    		assertEquals(MicroOp.M_FETCH_OPERAND_HI, instr.getExpectedMnemonicOrder(false)[2]);
    		assertEquals(MicroOp.M_READ_EA, instr.getExpectedMnemonicOrder(false)[3]);
    		assertEquals(3, instr.getOperandReadCycleOffset(false));
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__ldaAbsoluteXCrossAddsDummyRead) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::ldaAbsoluteXCrossAddsDummyRead");
    /*
    Original Java test body:
    		Cpu65c02OpcodeView instr = Cpu65c02Microcode.opcodeForByte(0xBD); // LDA abs,X
    		MicroOp[] noCross = instr.getExpectedMnemonicOrder(false);
    		MicroOp[] cross = instr.getExpectedMnemonicOrder(true);
    		assertEquals(4, noCross.length);
    		assertEquals(5, cross.length);
    		assertEquals(MicroOp.M_READ_EA, noCross[3]);
    		assertEquals(MicroOp.M_READ_DUMMY, cross[3]);
    		assertEquals(MicroOp.M_READ_EA, cross[4]);
    		assertEquals(3, instr.getOperandReadCycleOffset(false));
    		assertEquals(4, instr.getOperandReadCycleOffset(true));
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__staAbsoluteHasWriteCycle) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::staAbsoluteHasWriteCycle");
    /*
    Original Java test body:
    		Cpu65c02OpcodeView instr = Cpu65c02Microcode.opcodeForByte(0x8D); // STA abs
    		MicroOp[] script = instr.getExpectedMnemonicOrder(false);
    		assertEquals(MicroOp.M_FETCH_OPCODE, script[0]);
    		assertEquals(MicroOp.M_FETCH_OPERAND_LO, script[1]);
    		assertEquals(MicroOp.M_FETCH_OPERAND_HI, script[2]);
    		assertEquals(MicroOp.M_WRITE_EA, script[3]);
    		assertEquals(-1, instr.getOperandReadCycleOffset(false));
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__incZeroPageIsReadModifyWrite) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::incZeroPageIsReadModifyWrite");
    /*
    Original Java test body:
    		Cpu65c02OpcodeView instr = Cpu65c02Microcode.opcodeForByte(0xE6); // INC zpg
    		MicroOp[] script = instr.getExpectedMnemonicOrder(false);
    		assertEquals(Cpu65c02Microcode.AccessType.AT_RMW, instr.getAccessType());
    		assertEquals(MicroOp.M_READ_EA, script[2]);
    		assertEquals(MicroOp.M_WRITE_EA_DUMMY, script[3]);
    		assertEquals(MicroOp.M_WRITE_EA, script[4]);
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__everyOpcodeHasMicroInstrEntry) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::everyOpcodeHasMicroInstrEntry");
    /*
    Original Java test body:
    		for( int op = 0; op<256; op++ ) {
    			Cpu65c02OpcodeView instr = Cpu65c02Microcode.opcodeForByte(op);
    			assertTrue(instr!=null);
    			assertEquals(op, instr.getOpcodeByte());
    			assertTrue(instr.getExpectedMnemonicOrder(false).length>=1);
    			assertEquals(MicroOp.M_FETCH_OPCODE, instr.getExpectedMnemonicOrder(false)[0]);
    		}
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__allLdaOpcodesHaveExpectedMicrocodeOrder) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::allLdaOpcodesHaveExpectedMicrocodeOrder");
    /*
    Original Java test body:
    		for( LdaExpect expect : LDA_EXPECTATIONS ) {
    			Cpu65c02OpcodeView entry = Cpu65c02Microcode.opcodeForByte(expect.opcode);
    			assertTrue(entry.usesMemoryDataRead());
    			assertEquals(expect.opcode, entry.getOpcodeByte());
    			assertEquals(expect.noCross.length, entry.getExpectedMnemonicOrder(false).length);
    			assertEquals(expect.cross.length, entry.getExpectedMnemonicOrder(true).length);
    			for( int i = 0; i<expect.noCross.length; i++ )
    				assertEquals(expect.noCross[i], entry.getExpectedMnemonicOrder(false)[i]);
    			for( int i = 0; i<expect.cross.length; i++ )
    				assertEquals(expect.cross[i], entry.getExpectedMnemonicOrder(true)[i]);
    			assertEquals(expect.readOffsetNoCross, entry.getOperandReadCycleOffset(false));
    			assertEquals(expect.readOffsetCross, entry.getOperandReadCycleOffset(true));
    		}
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__allOraOpcodesHaveExpectedMicrocodeOrder) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::allOraOpcodesHaveExpectedMicrocodeOrder");
    /*
    Original Java test body:
    		for( OraExpect expect : ORA_EXPECTATIONS ) {
    			Cpu65c02OpcodeView entry = Cpu65c02Microcode.opcodeForByte(expect.opcode);
    			assertTrue(entry.usesMemoryDataRead());
    			assertEquals(expect.opcode, entry.getOpcodeByte());
    			assertEquals(expect.noCross.length, entry.getExpectedMnemonicOrder(false).length);
    			assertEquals(expect.cross.length, entry.getExpectedMnemonicOrder(true).length);
    			for( int i = 0; i<expect.noCross.length; i++ )
    				assertEquals(expect.noCross[i], entry.getExpectedMnemonicOrder(false)[i]);
    			for( int i = 0; i<expect.cross.length; i++ )
    				assertEquals(expect.cross[i], entry.getExpectedMnemonicOrder(true)[i]);
    			assertEquals(expect.readOffsetNoCross, entry.getOperandReadCycleOffset(false));
    			assertEquals(expect.readOffsetCross, entry.getOperandReadCycleOffset(true));
    		}
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__allAndOpcodesHaveExpectedMicrocodeOrder) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::allAndOpcodesHaveExpectedMicrocodeOrder");
    /*
    Original Java test body:
    		for( AndExpect expect : AND_EXPECTATIONS ) {
    			Cpu65c02OpcodeView entry = Cpu65c02Microcode.opcodeForByte(expect.opcode);
    			assertTrue(entry.usesMemoryDataRead());
    			assertEquals(expect.opcode, entry.getOpcodeByte());
    			assertEquals(expect.noCross.length, entry.getExpectedMnemonicOrder(false).length);
    			assertEquals(expect.cross.length, entry.getExpectedMnemonicOrder(true).length);
    			for( int i = 0; i<expect.noCross.length; i++ )
    				assertEquals(expect.noCross[i], entry.getExpectedMnemonicOrder(false)[i]);
    			for( int i = 0; i<expect.cross.length; i++ )
    				assertEquals(expect.cross[i], entry.getExpectedMnemonicOrder(true)[i]);
    			assertEquals(expect.readOffsetNoCross, entry.getOperandReadCycleOffset(false));
    			assertEquals(expect.readOffsetCross, entry.getOperandReadCycleOffset(true));
    		}
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__allEorOpcodesHaveExpectedMicrocodeOrder) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::allEorOpcodesHaveExpectedMicrocodeOrder");
    /*
    Original Java test body:
    		for( EorExpect expect : EOR_EXPECTATIONS ) {
    			Cpu65c02OpcodeView entry = Cpu65c02Microcode.opcodeForByte(expect.opcode);
    			assertTrue(entry.usesMemoryDataRead());
    			assertEquals(expect.opcode, entry.getOpcodeByte());
    			assertEquals(expect.noCross.length, entry.getExpectedMnemonicOrder(false).length);
    			assertEquals(expect.cross.length, entry.getExpectedMnemonicOrder(true).length);
    			for( int i = 0; i<expect.noCross.length; i++ )
    				assertEquals(expect.noCross[i], entry.getExpectedMnemonicOrder(false)[i]);
    			for( int i = 0; i<expect.cross.length; i++ )
    				assertEquals(expect.cross[i], entry.getExpectedMnemonicOrder(true)[i]);
    			assertEquals(expect.readOffsetNoCross, entry.getOperandReadCycleOffset(false));
    			assertEquals(expect.readOffsetCross, entry.getOperandReadCycleOffset(true));
    		}
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__allAdcOpcodesHaveExpectedMicrocodeOrder) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::allAdcOpcodesHaveExpectedMicrocodeOrder");
    /*
    Original Java test body:
    		for( AdcExpect expect : ADC_EXPECTATIONS ) {
    			Cpu65c02OpcodeView entry = Cpu65c02Microcode.opcodeForByte(expect.opcode);
    			assertTrue(entry.usesMemoryDataRead());
    			assertEquals(expect.opcode, entry.getOpcodeByte());
    			assertEquals(expect.noCross.length, entry.getExpectedMnemonicOrder(false).length);
    			assertEquals(expect.cross.length, entry.getExpectedMnemonicOrder(true).length);
    			for( int i = 0; i<expect.noCross.length; i++ )
    				assertEquals(expect.noCross[i], entry.getExpectedMnemonicOrder(false)[i]);
    			for( int i = 0; i<expect.cross.length; i++ )
    				assertEquals(expect.cross[i], entry.getExpectedMnemonicOrder(true)[i]);
    			assertEquals(expect.readOffsetNoCross, entry.getOperandReadCycleOffset(false));
    			assertEquals(expect.readOffsetCross, entry.getOperandReadCycleOffset(true));
    		}
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__allSbcOpcodesHaveExpectedMicrocodeOrder) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::allSbcOpcodesHaveExpectedMicrocodeOrder");
    /*
    Original Java test body:
    		for( SbcExpect expect : SBC_EXPECTATIONS ) {
    			Cpu65c02OpcodeView entry = Cpu65c02Microcode.opcodeForByte(expect.opcode);
    			assertTrue(entry.usesMemoryDataRead());
    			assertEquals(expect.opcode, entry.getOpcodeByte());
    			assertEquals(expect.noCross.length, entry.getExpectedMnemonicOrder(false).length);
    			assertEquals(expect.cross.length, entry.getExpectedMnemonicOrder(true).length);
    			for( int i = 0; i<expect.noCross.length; i++ )
    				assertEquals(expect.noCross[i], entry.getExpectedMnemonicOrder(false)[i]);
    			for( int i = 0; i<expect.cross.length; i++ )
    				assertEquals(expect.cross[i], entry.getExpectedMnemonicOrder(true)[i]);
    			assertEquals(expect.readOffsetNoCross, entry.getOperandReadCycleOffset(false));
    			assertEquals(expect.readOffsetCross, entry.getOperandReadCycleOffset(true));
    		}
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__allCmpOpcodesHaveExpectedMicrocodeOrder) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::allCmpOpcodesHaveExpectedMicrocodeOrder");
    /*
    Original Java test body:
    		for( CmpExpect expect : CMP_EXPECTATIONS ) {
    			Cpu65c02OpcodeView entry = Cpu65c02Microcode.opcodeForByte(expect.opcode);
    			assertTrue(entry.usesMemoryDataRead());
    			assertEquals(expect.opcode, entry.getOpcodeByte());
    			assertEquals(expect.noCross.length, entry.getExpectedMnemonicOrder(false).length);
    			assertEquals(expect.cross.length, entry.getExpectedMnemonicOrder(true).length);
    			for( int i = 0; i<expect.noCross.length; i++ )
    				assertEquals(expect.noCross[i], entry.getExpectedMnemonicOrder(false)[i]);
    			for( int i = 0; i<expect.cross.length; i++ )
    				assertEquals(expect.cross[i], entry.getExpectedMnemonicOrder(true)[i]);
    			assertEquals(expect.readOffsetNoCross, entry.getOperandReadCycleOffset(false));
    			assertEquals(expect.readOffsetCross, entry.getOperandReadCycleOffset(true));
    		}
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__ldxAbsoluteYCrossAddsDummyRead) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::ldxAbsoluteYCrossAddsDummyRead");
    /*
    Original Java test body:
    		Cpu65c02OpcodeView instr = Cpu65c02Microcode.opcodeForByte(0xBE); // LDX abs,Y
    		MicroOp[] noCross = instr.getExpectedMnemonicOrder(false);
    		MicroOp[] cross = instr.getExpectedMnemonicOrder(true);
    		assertEquals(4, noCross.length);
    		assertEquals(5, cross.length);
    		assertEquals(MicroOp.M_READ_EA, noCross[3]);
    		assertEquals(MicroOp.M_READ_DUMMY, cross[3]);
    		assertEquals(MicroOp.M_READ_EA, cross[4]);
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__ldyAbsoluteXCrossAddsDummyRead) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::ldyAbsoluteXCrossAddsDummyRead");
    /*
    Original Java test body:
    		Cpu65c02OpcodeView instr = Cpu65c02Microcode.opcodeForByte(0xBC); // LDY abs,X
    		MicroOp[] noCross = instr.getExpectedMnemonicOrder(false);
    		MicroOp[] cross = instr.getExpectedMnemonicOrder(true);
    		assertEquals(4, noCross.length);
    		assertEquals(5, cross.length);
    		assertEquals(MicroOp.M_READ_EA, noCross[3]);
    		assertEquals(MicroOp.M_READ_DUMMY, cross[3]);
    		assertEquals(MicroOp.M_READ_EA, cross[4]);
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__bitAbsoluteXHasSingleReadCycleNoSplitScript) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::bitAbsoluteXHasSingleReadCycleNoSplitScript");
    /*
    Original Java test body:
    		Cpu65c02OpcodeView instr = Cpu65c02Microcode.opcodeForByte(0x3C); // BIT abs,X
    		assertArrayEquals(instr.getExpectedMnemonicOrder(false), instr.getExpectedMnemonicOrder(true));
    		assertEquals(4, instr.getExpectedMnemonicOrder(false).length);
    		assertEquals(MicroOp.M_READ_EA, instr.getExpectedMnemonicOrder(false)[3]);
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__stxZeroPageYIncludesIndexedDummyReadBeforeWrite) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::stxZeroPageYIncludesIndexedDummyReadBeforeWrite");
    /*
    Original Java test body:
    		Cpu65c02OpcodeView instr = Cpu65c02Microcode.opcodeForByte(0x96); // STX zpg,Y
    		MicroOp[] script = instr.getExpectedMnemonicOrder(false);
    		assertEquals(Cpu65c02Microcode.AccessType.AT_WRITE, instr.getAccessType());
    		assertEquals(MicroOp.M_READ_DUMMY, script[2]);
    		assertEquals(MicroOp.M_WRITE_EA, script[3]);
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__styZeroPageXIncludesIndexedDummyReadBeforeWrite) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::styZeroPageXIncludesIndexedDummyReadBeforeWrite");
    /*
    Original Java test body:
    		Cpu65c02OpcodeView instr = Cpu65c02Microcode.opcodeForByte(0x94); // STY zpg,X
    		MicroOp[] script = instr.getExpectedMnemonicOrder(false);
    		assertEquals(Cpu65c02Microcode.AccessType.AT_WRITE, instr.getAccessType());
    		assertEquals(MicroOp.M_READ_DUMMY, script[2]);
    		assertEquals(MicroOp.M_WRITE_EA, script[3]);
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__allPageCrossPenaltyReadOpcodesUseSplitScriptWithDummyRead) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::allPageCrossPenaltyReadOpcodesUseSplitScriptWithDummyRead");
    /*
    Original Java test body:
    		int[] opcodes = new int[] {
    				0x7D, 0x79, 0x71, // ADC abs,x abs,y ind,y
    				0x3D, 0x39, 0x31, // AND
    				0xDD, 0xD9, 0xD1, // CMP
    				0x5D, 0x59, 0x51, // EOR
    				0xBD, 0xB9, 0xB1, // LDA
    				0x1D, 0x19, 0x11, // ORA
    				0xFD, 0xF9, 0xF1, // SBC
    				0xBC,             // LDY abs,x
    				0xBE              // LDX abs,y
    		};
    		for( int opcode : opcodes ) {
    			Cpu65c02OpcodeView view = Cpu65c02Microcode.opcodeForByte(opcode);
    			MicroOp[] noCross = view.getExpectedMnemonicOrder(false);
    			MicroOp[] cross = view.getExpectedMnemonicOrder(true);
    			assertEquals(String.format("$%02X must add exactly one page-cross cycle", opcode),
    					noCross.length + 1, cross.length);
    			assertEquals(String.format("$%02X must insert dummy read on page-cross path", opcode),
    					MicroOp.M_READ_DUMMY, cross[cross.length-2]);
    			assertEquals(String.format("$%02X must still perform final effective read", opcode),
    					MicroOp.M_READ_EA, cross[cross.length-1]);
    		}
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__noPenaltyAbsoluteIndexedReadsDoNotSplit) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::noPenaltyAbsoluteIndexedReadsDoNotSplit");
    /*
    Original Java test body:
    		int[] opcodes = new int[] {
    				0x3C, // BIT abs,x
    				0x5C, 0xDC, 0xFC // CMD fixed-cycle NOP abs,x
    		};
    		for( int opcode : opcodes ) {
    			Cpu65c02OpcodeView view = Cpu65c02Microcode.opcodeForByte(opcode);
    			assertArrayEquals(String.format("$%02X should not have a page-cross split path", opcode),
    					view.getExpectedMnemonicOrder(false), view.getExpectedMnemonicOrder(true));
    		}
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__allBranchOpcodesExposeTakenMicrocodeStep) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::allBranchOpcodesExposeTakenMicrocodeStep");
    /*
    Original Java test body:
    		int[] branches = new int[] { 0x10, 0x30, 0x50, 0x70, 0x80, 0x90, 0xB0, 0xD0, 0xF0 };
    		for( int opcode : branches ) {
    			Cpu65c02OpcodeView view = Cpu65c02Microcode.opcodeForByte(opcode);
    			MicroOp[] notTaken = view.getExpectedMnemonicOrder(false);
    			MicroOp[] taken = view.getExpectedMnemonicOrder(true);
    			assertEquals(String.format("$%02X branch not-taken cycles", opcode), 2, notTaken.length);
    			assertEquals(String.format("$%02X branch taken cycles", opcode), 3, taken.length);
    			assertArrayEquals(String.format("$%02X branch base path", opcode),
    					new MicroOp[] { MicroOp.M_FETCH_OPCODE, MicroOp.M_READ_IMM_DATA }, notTaken);
    			assertArrayEquals(String.format("$%02X branch taken path", opcode),
    					new MicroOp[] { MicroOp.M_FETCH_OPCODE, MicroOp.M_READ_IMM_DATA, MicroOp.M_INTERNAL }, taken);
    		}
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__allStaOpcodesHaveExpectedMicrocodeOrder) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::allStaOpcodesHaveExpectedMicrocodeOrder");
    /*
    Original Java test body:
    		for( StaExpect expect : STA_EXPECTATIONS ) {
    			Cpu65c02OpcodeView entry = Cpu65c02Microcode.opcodeForByte(expect.opcode);
    			assertEquals(expect.opcode, entry.getOpcodeByte());
    			assertEquals(expect.script.length, entry.getExpectedMnemonicOrder(false).length);
    			assertEquals(expect.script.length, entry.getExpectedMnemonicOrder(true).length);
    			assertArrayEquals(expect.script, entry.getExpectedMnemonicOrder(false));
    			assertArrayEquals(expect.script, entry.getExpectedMnemonicOrder(true));
    			assertEquals(-1, entry.getOperandReadCycleOffset(false));
    		}
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__allIncOpcodesHaveExpectedMicrocodeOrder) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::allIncOpcodesHaveExpectedMicrocodeOrder");
    /*
    Original Java test body:
    		for( IncExpect expect : INC_EXPECTATIONS ) {
    			Cpu65c02OpcodeView entry = Cpu65c02Microcode.opcodeForByte(expect.opcode);
    			assertEquals(expect.opcode, entry.getOpcodeByte());
    			assertEquals(expect.script.length, entry.getExpectedMnemonicOrder(false).length);
    			assertEquals(expect.script.length, entry.getExpectedMnemonicOrder(true).length);
    			assertArrayEquals(expect.script, entry.getExpectedMnemonicOrder(false));
    			assertArrayEquals(expect.script, entry.getExpectedMnemonicOrder(true));
    		}
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__allDecOpcodesHaveExpectedMicrocodeOrder) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::allDecOpcodesHaveExpectedMicrocodeOrder");
    /*
    Original Java test body:
    		for( DecExpect expect : DEC_EXPECTATIONS ) {
    			Cpu65c02OpcodeView entry = Cpu65c02Microcode.opcodeForByte(expect.opcode);
    			assertEquals(expect.opcode, entry.getOpcodeByte());
    			assertEquals(expect.script.length, entry.getExpectedMnemonicOrder(false).length);
    			assertEquals(expect.script.length, entry.getExpectedMnemonicOrder(true).length);
    			assertArrayEquals(expect.script, entry.getExpectedMnemonicOrder(false));
    			assertArrayEquals(expect.script, entry.getExpectedMnemonicOrder(true));
    		}
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__allAslOpcodesHaveExpectedMicrocodeOrder) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::allAslOpcodesHaveExpectedMicrocodeOrder");
    /*
    Original Java test body:
    		for( AslExpect expect : ASL_EXPECTATIONS ) {
    			Cpu65c02OpcodeView entry = Cpu65c02Microcode.opcodeForByte(expect.opcode);
    			assertEquals(expect.opcode, entry.getOpcodeByte());
    			assertEquals(expect.accessType, entry.getAccessType());
    			assertEquals(expect.script.length, entry.getExpectedMnemonicOrder(false).length);
    			assertEquals(expect.script.length, entry.getExpectedMnemonicOrder(true).length);
    			assertArrayEquals(expect.script, entry.getExpectedMnemonicOrder(false));
    			assertArrayEquals(expect.script, entry.getExpectedMnemonicOrder(true));
    		}
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__allLsrOpcodesHaveExpectedMicrocodeOrder) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::allLsrOpcodesHaveExpectedMicrocodeOrder");
    /*
    Original Java test body:
    		for( LsrExpect expect : LSR_EXPECTATIONS ) {
    			Cpu65c02OpcodeView entry = Cpu65c02Microcode.opcodeForByte(expect.opcode);
    			assertEquals(expect.opcode, entry.getOpcodeByte());
    			assertEquals(expect.accessType, entry.getAccessType());
    			assertEquals(expect.script.length, entry.getExpectedMnemonicOrder(false).length);
    			assertEquals(expect.script.length, entry.getExpectedMnemonicOrder(true).length);
    			assertArrayEquals(expect.script, entry.getExpectedMnemonicOrder(false));
    			assertArrayEquals(expect.script, entry.getExpectedMnemonicOrder(true));
    		}
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__allRolOpcodesHaveExpectedMicrocodeOrder) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::allRolOpcodesHaveExpectedMicrocodeOrder");
    /*
    Original Java test body:
    		for( RolExpect expect : ROL_EXPECTATIONS ) {
    			Cpu65c02OpcodeView entry = Cpu65c02Microcode.opcodeForByte(expect.opcode);
    			assertEquals(expect.opcode, entry.getOpcodeByte());
    			assertEquals(expect.accessType, entry.getAccessType());
    			assertEquals(expect.script.length, entry.getExpectedMnemonicOrder(false).length);
    			assertEquals(expect.script.length, entry.getExpectedMnemonicOrder(true).length);
    			assertArrayEquals(expect.script, entry.getExpectedMnemonicOrder(false));
    			assertArrayEquals(expect.script, entry.getExpectedMnemonicOrder(true));
    		}
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__allRorOpcodesHaveExpectedMicrocodeOrder) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::allRorOpcodesHaveExpectedMicrocodeOrder");
    /*
    Original Java test body:
    		for( RorExpect expect : ROR_EXPECTATIONS ) {
    			Cpu65c02OpcodeView entry = Cpu65c02Microcode.opcodeForByte(expect.opcode);
    			assertEquals(expect.opcode, entry.getOpcodeByte());
    			assertEquals(expect.accessType, entry.getAccessType());
    			assertEquals(expect.script.length, entry.getExpectedMnemonicOrder(false).length);
    			assertEquals(expect.script.length, entry.getExpectedMnemonicOrder(true).length);
    			assertArrayEquals(expect.script, entry.getExpectedMnemonicOrder(false));
    			assertArrayEquals(expect.script, entry.getExpectedMnemonicOrder(true));
    		}
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__microContextExposesCpuAndInternalRegisters) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::microContextExposesCpuAndInternalRegisters");
    /*
    Original Java test body:
    		Cpu65c02Microcode.MicroContext ctx = Cpu65c02Microcode.newContext();
    		assertEquals(0, ctx.cpu.a);
    		assertEquals(0, ctx.cpu.x);
    		assertEquals(0, ctx.cpu.y);
    		assertEquals(0, ctx.cpu.pc);
    		assertEquals(0, ctx.internal.effectiveAddress);
    		assertEquals(0, ctx.internal.cycleIndex);
    		assertTrue(!ctx.internal.pageCrossed);
    		ctx.cpu.a = 0x42;
    		ctx.internal.effectiveAddress = 0xC054;
    		ctx.internal.pageCrossed = true;
    		assertEquals(0x42, ctx.cpu.a);
    		assertEquals(0xC054, ctx.internal.effectiveAddress);
    		assertTrue(ctx.internal.pageCrossed);
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__allOpcodesUseNonFallbackScriptsBeyondSingleCycle) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::allOpcodesUseNonFallbackScriptsBeyondSingleCycle");
    /*
    Original Java test body:
    		for( int opcode = 0; opcode<256; opcode++ ) {
    			Cpu65c02OpcodeView entry = Cpu65c02Microcode.opcodeForByte(opcode);
    			Opcode cpuOp = entry.getOpcode();
    			MicroOp[] noCross = entry.getExpectedMnemonicOrder(false);
    			MicroOp[] cross = entry.getExpectedMnemonicOrder(true);
    			assertEquals(opcode, entry.getOpcodeByte());
    			assertTrue(noCross.length>0);
    			assertTrue(cross.length>0);
    			if( cpuOp.getCycleTime()>1 ) {
    				assertTrue(!(noCross.length==1 && noCross[0]==MicroOp.M_FETCH_OPCODE));
    			}
    			if( cross.length>noCross.length ) {
    				assertEquals(noCross.length+1, cross.length);
    			}
    		}
    
    */
}

E2_PORT_TEST(Cpu65c02MicrocodeTest__allOpcodesMicrocodeLengthMatchesCpuCycleTable) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu65c02MicrocodeTest.java::allOpcodesMicrocodeLengthMatchesCpuCycleTable");
    /*
    Original Java test body:
    		for( int opcode = 0; opcode<256; opcode++ ) {
    			Cpu65c02OpcodeView entry = Cpu65c02Microcode.opcodeForByte(opcode);
    			Opcode cpuOp = entry.getOpcode();
    			int baseCycles = cpuOp.getCycleTime();
    			MicroOp[] noCross = entry.getExpectedMnemonicOrder(false);
    			MicroOp[] cross = entry.getExpectedMnemonicOrder(true);
    			assertEquals("no-cross length mismatch for opcode " + String.format("%02X", opcode), baseCycles, noCross.length);
    			if( cross.length!=noCross.length ) {
    				assertEquals("cross-path must be exactly one cycle longer for opcode " + String.format("%02X", opcode),
    						noCross.length + 1, cross.length);
    			}
    		}
    
    */
}
