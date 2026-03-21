#include "test_harness.h"

#include <array>
#include <cstdint>
#include <vector>

#include "cpu/cpu65c02_cmd_profile.h"
#include "cpu/cpu65c02_microcode.h"
#include "cpu/cpu65c02_opcode.h"
#include "cpu/cpu65c02_opcode_view.h"
#include "cpu65c02.h"

namespace {

enum class Profile {
    WDC,
    CMD,
};

Cpu65c02::OpcodeDescriptor opcodeFor(Profile p, int opcodeByte)
{
    if( p==Profile::WDC )
        return Cpu65c02::getOpcodeDescriptor(static_cast<uint16_t>(opcodeByte & 0xFF));
    return Cpu65c02CmdProfile::opcodeAt(static_cast<uint8_t>(opcodeByte & 0xFF));
}

void assertProfileOpcodeTableHasAllSlots(Profile p)
{
    (void)p;
    for( int opcode = 0; opcode<256; ++opcode ) {
        const Cpu65c02::OpcodeDescriptor d = opcodeFor(p, opcode);
        E2TEST_ASSERT_EQ(opcode, static_cast<int>(d.machineCode));
    }
}

void assertProfileMicrocodeCoverage(Profile p)
{
    for( Cpu65c02Opcode::Id id : Cpu65c02Opcode::values() ) {
        const Cpu65c02Opcode::Definition& microcoded = Cpu65c02Opcode::byId(id);
        const int opcodeByte = microcoded.opcodeByte & 0xFF;

        const Cpu65c02::OpcodeDescriptor profileOpcode = opcodeFor(p, opcodeByte);
        E2TEST_ASSERT_TRUE(static_cast<Cpu65c02::OpcodeMnemonic>(profileOpcode.mnemonic)!=Cpu65c02::_NOP);

        const Cpu65c02OpcodeView view = Cpu65c02Microcode::opcodeForByte(opcodeByte);
        E2TEST_ASSERT_EQ(opcodeByte, view.getOpcodeByte());

        const int noCrossCycles = static_cast<int>(view.getExpectedMnemonicOrder(false).size());
        const int crossCycles = static_cast<int>(view.getExpectedMnemonicOrder(true).size());
        const int profileCycles = static_cast<int>(profileOpcode.cycleTime);

        E2TEST_ASSERT_TRUE(profileCycles>=noCrossCycles);
        E2TEST_ASSERT_TRUE(
                profileCycles==noCrossCycles ||
                profileCycles==crossCycles ||
                profileCycles==noCrossCycles+1);
    }
}

void assertProfileCycleCountMatchesMicrocodeSteps(Profile p, bool includeNops, bool requireExplicitMicrocode)
{
    for( int opcodeByte = 0; opcodeByte<256; ++opcodeByte ) {
        const Cpu65c02::OpcodeDescriptor opcode = opcodeFor(p, opcodeByte);
        const bool isNop = static_cast<Cpu65c02::OpcodeMnemonic>(opcode.mnemonic)==Cpu65c02::_NOP;
        if( isNop!=includeNops )
            continue;

        const Cpu65c02Opcode::Definition* explicitDef = Cpu65c02Opcode::fromOpcodeByte(static_cast<uint8_t>(opcodeByte));
        if( requireExplicitMicrocode )
            E2TEST_ASSERT_TRUE(explicitDef!=nullptr);

        const Cpu65c02OpcodeView view = Cpu65c02Microcode::opcodeForByte(opcodeByte);
        const int baseCycles = static_cast<int>(opcode.cycleTime);
        const int noCrossCycles = static_cast<int>(view.getExpectedMnemonicOrder(false).size());
        const int crossCycles = static_cast<int>(view.getExpectedMnemonicOrder(true).size());

        E2TEST_ASSERT_EQ(baseCycles, noCrossCycles);
        if( crossCycles!=noCrossCycles )
            E2TEST_ASSERT_EQ(baseCycles + 1, crossCycles);
    }
}

} // namespace

E2TEST_CASE(wdcProfileHasCompleteOpcodeTableAndMicrocodeCoverage)
{
    assertProfileOpcodeTableHasAllSlots(Profile::WDC);
    assertProfileMicrocodeCoverage(Profile::WDC);
}

E2TEST_CASE(cmdProfileHasCompleteOpcodeTableAndMicrocodeCoverage)
{
    assertProfileOpcodeTableHasAllSlots(Profile::CMD);
    assertProfileMicrocodeCoverage(Profile::CMD);
}

E2TEST_CASE(wdcProfileAllNonNopOpcodesMatchMicrocodeCycleCount)
{
    assertProfileCycleCountMatchesMicrocodeSteps(Profile::WDC, false, true);
}

E2TEST_CASE(cmdProfileAllNonNopOpcodesMatchMicrocodeCycleCount)
{
    assertProfileCycleCountMatchesMicrocodeSteps(Profile::CMD, false, true);
}

E2TEST_CASE(wdcProfileNopOpcodesMatchMicrocodeCycleCount)
{
    assertProfileCycleCountMatchesMicrocodeSteps(Profile::WDC, true, false);
}

E2TEST_CASE(cmdProfileNopOpcodesMatchMicrocodeCycleCount)
{
    assertProfileCycleCountMatchesMicrocodeSteps(Profile::CMD, true, false);
}
