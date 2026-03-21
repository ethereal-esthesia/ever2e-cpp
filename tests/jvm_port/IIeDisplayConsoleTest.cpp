// Auto-generated JVM test port scaffold.
// Source: /Users/shane/Project/ever2e-jvm/src/test/IIeDisplayConsoleTest.java
// NOTE: Function bodies are placeholders; original Java test bodies are preserved below each test.

#include "jvm_test_port_harness.h"

E2_PORT_TEST(IIeDisplayConsoleTest__test) {
    E2_PORT_UNIMPLEMENTED("Ported from IIeDisplayConsoleTest.java::test");
    /*
    Original Java test body:
    		for( int scanline = 0; scanline<24; scanline++ )
    			System.out.println(scanline+1+" "+DisplayIIe.getAddressLo40(1, scanline, 0)+" "+DisplayIIe.getAddressLo40(1, scanline, 39));
    		for( int scanline = 0; scanline<192; scanline++ )
    			System.out.println(scanline+1+" "+DisplayIIe.getAddressHi40(1, scanline, 0)+" "+DisplayIIe.getAddressHi40(1, scanline, 39));
    
    		ScanlineTracer8 tracer = new ScanlineTracer8();
    
    		tracer.setScanStart(25, 70);
    		tracer.setScanSize(65, 262);
    
    		System.out.println("\nLores Trace Addresses");
    		tracer.setTraceMap(DisplayIIe.LO40_TRACE);
    		displayTest(tracer);
    
    		System.out.println("\nHires Trace Addresses");
    		tracer.setTraceMap(DisplayIIe.HI40_TRACE);
    		displayTest(tracer);
    
    
    */
}
