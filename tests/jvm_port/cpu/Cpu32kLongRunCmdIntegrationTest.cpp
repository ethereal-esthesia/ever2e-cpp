// Auto-generated JVM test port scaffold.
// Source: /Users/shane/Project/ever2e-jvm/src/test/cpu/Cpu32kLongRunCmdIntegrationTest.java
// NOTE: Function bodies are placeholders; original Java test bodies are preserved below each test.

#include "../jvm_test_port_harness.h"

E2_PORT_TEST(Cpu32kLongRunCmdIntegrationTest__opcodeLongRun32kReachesPassLoop) {
    E2_PORT_UNIMPLEMENTED("Ported from cpu/Cpu32kLongRunCmdIntegrationTest.java::opcodeLongRun32kReachesPassLoop");
    /*
    Original Java test body:
    		String emuFile = getProp("ever2e.long32k.emu", "ever2e.smoke32k.emu", "ROMS/Apple2eNoSlots.emu");
    		String pasteFile = getProp("ever2e.long32k.pasteFile", "ever2e.smoke32k.pasteFile", "ROMS/opcode_smoke_loader_hgr_mem_32k.mon");
    		String steps = getProp("ever2e.long32k.steps", "ever2e.smoke32k.steps", "120000000");
    		String haltExecution = getProp("ever2e.long32k.haltExecution", "ever2e.smoke32k.haltExecution", "0x6A45,0x6A33");
    		String cpuProfile = getProp("ever2e.long32k.cpuProfile", "ever2e.smoke32k.cpuProfile", "cmd");
    
    		ByteArrayOutputStream output = new ByteArrayOutputStream();
    		PrintStream originalOut = System.out;
    		String previousHeadless = System.getProperty("ever2e.headless");
    		try {
    			System.setProperty("ever2e.headless", "true");
    			System.setOut(new PrintStream(output, true, StandardCharsets.UTF_8));
    				Emulator8Coordinator.main(new String[] {
    						emuFile,
    						"--steps", steps,
    						"--text-console",
    						"--paste-file", pasteFile,
    						"--cpu-profile", cpuProfile,
    						"--halt-execution", haltExecution,
    						"--print-cpu-state-at-exit",
    						"--print-text-at-exit",
    						"--no-sound",
    				});
    		}
    		finally {
    			System.setOut(originalOut);
    			if( previousHeadless==null )
    				System.clearProperty("ever2e.headless");
    			else
    				System.setProperty("ever2e.headless", previousHeadless);
    		}
    
    		String stdout = output.toString(StandardCharsets.UTF_8);
    		assertTrue("32k long-run should stop at pass loop 0x6A45.\nOutput:\n"+stdout,
    				stdout.contains("Stopped at PC=6A45"));
    		assertTrue("32k long-run should not stop at fail loop 0x6A33.\nOutput:\n"+stdout,
    				!stdout.contains("Stopped at PC=6A33"));
    
    */
}
