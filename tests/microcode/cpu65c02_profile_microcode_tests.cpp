#include "test_harness.h"

#include <array>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include "cpu/cpu65c02_cmd_profile.h"
#include "cpu/cpu65c02_microcode.h"
#include "cpu/cpu65c02_opcode.h"
#include "cpu/cpu65c02_opcode_view.h"
#include "cpu65c02.h"
#include "memory128k.h"
#include "savestate.h"

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

class ScopedCwd {
public:
    explicit ScopedCwd(const std::string& path)
        : active_(false)
    {
        std::error_code ec;
        oldCwd_ = std::filesystem::current_path(ec);
        if( ec )
            return;
        std::filesystem::current_path(path, ec);
        if( !ec )
            active_ = true;
    }

    ~ScopedCwd()
    {
        if( active_ )
            std::filesystem::current_path(oldCwd_);
    }

    bool active() const
    {
        return active_;
    }

private:
    bool active_;
    std::filesystem::path oldCwd_;
};

void primeCpuAtOpcode(Cpu65c02& cpu, Uint16 pc, Uint16 opcodeIndex, Uint32 cycleCount)
{
    SaveState state;
    state.write8(0x00);
    state.write8(0x00);
    state.write8(0x00);
    state.write16(pc);
    state.write8(0xFF);
    state.write8(0x24);
    state.write32(1);
    state.write32(cycleCount);
    state.write32(0);
    state.write16(opcodeIndex);
    state.write16((Uint16)Cpu65c02::_NOI);
    state.write16((Uint16)Cpu65c02::NORMAL);
    E2TEST_ASSERT_EQ(0, cpu.restore(state));
}

void primeCpuState(
        Cpu65c02& cpu,
        Uint16 pc,
        Uint16 opcodeIndex,
        Uint32 cycleCount,
        Uint8 a,
        Uint8 y,
        Uint8 x,
        Uint8 p,
        Uint8 s)
{
    SaveState state;
    state.write8(a);
    state.write8(y);
    state.write8(x);
    state.write16(pc);
    state.write8(s);
    state.write8(p);
    state.write32(1);
    state.write32(cycleCount);
    state.write32(0);
    state.write16(opcodeIndex);
    state.write16((Uint16)Cpu65c02::_NOI);
    state.write16((Uint16)Cpu65c02::NORMAL);
    E2TEST_ASSERT_EQ(0, cpu.restore(state));
}

int runUntilProgramCounterChanges(Cpu65c02& cpu, Uint16 startPc)
{
    int cycles = 0;
    while( cpu.getProgramCounter()==startPc && cycles<256 ) {
        cpu.cycle();
        cycles++;
    }
    E2TEST_ASSERT_TRUE(cpu.getProgramCounter()!=startPc);
    return cycles;
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

E2TEST_CASE(cpuDescriptorProfilesExposeCmdOverrides)
{
    const Cpu65c02::OpcodeDescriptor wdc07 =
            Cpu65c02::getOpcodeDescriptorForProfile(Cpu65c02::PROFILE_WDC, 0x07);
    const Cpu65c02::OpcodeDescriptor cmd07 =
            Cpu65c02::getOpcodeDescriptorForProfile(Cpu65c02::PROFILE_CMD, 0x07);
    E2TEST_ASSERT_EQ(1, static_cast<int>(wdc07.instrSize));
    E2TEST_ASSERT_EQ(2, static_cast<int>(cmd07.instrSize));

    const Cpu65c02::OpcodeDescriptor wdc0f =
            Cpu65c02::getOpcodeDescriptorForProfile(Cpu65c02::PROFILE_WDC, 0x0F);
    const Cpu65c02::OpcodeDescriptor cmd0f =
            Cpu65c02::getOpcodeDescriptorForProfile(Cpu65c02::PROFILE_CMD, 0x0F);
    E2TEST_ASSERT_EQ(1, static_cast<int>(wdc0f.instrSize));
    E2TEST_ASSERT_EQ(3, static_cast<int>(cmd0f.instrSize));

}

E2TEST_CASE(runtimeCmdProfileNopWidthDiffersFromWdc)
{
    ScopedCwd cwd("release");
    E2TEST_ASSERT_TRUE(cwd.active());

    std::unique_ptr<Memory128k> memoryWdc(new Memory128k());
    std::unique_ptr<Memory128k> memoryCmd(new Memory128k());

    Cpu65c02 cpuWdc(memoryWdc.get(), Cpu65c02::PROFILE_WDC);
    Cpu65c02 cpuCmd(memoryCmd.get(), Cpu65c02::PROFILE_CMD);

    const Uint16 startPc = 0x0200;
    memoryWdc->putMem(startPc, 0x07);
    memoryCmd->putMem(startPc, 0x07);
    memoryWdc->putMem(startPc + 1, 0xEA);
    memoryCmd->putMem(startPc + 1, 0xEA);
    memoryWdc->putMem(startPc + 2, 0xEA);
    memoryCmd->putMem(startPc + 2, 0xEA);

    const Cpu65c02::OpcodeDescriptor wdcDesc =
            Cpu65c02::getOpcodeDescriptorForProfile(Cpu65c02::PROFILE_WDC, 0x07);
    const Cpu65c02::OpcodeDescriptor cmdDesc =
            Cpu65c02::getOpcodeDescriptorForProfile(Cpu65c02::PROFILE_CMD, 0x07);

    primeCpuAtOpcode(cpuWdc, startPc, 0x07, wdcDesc.cycleTime);
    primeCpuAtOpcode(cpuCmd, startPc, 0x07, cmdDesc.cycleTime);

    int cyclesWdc = 0;
    while( cpuWdc.getProgramCounter()==startPc && cyclesWdc<64 ) {
        cpuWdc.cycle();
        cyclesWdc++;
    }
    int cyclesCmd = 0;
    while( cpuCmd.getProgramCounter()==startPc && cyclesCmd<64 ) {
        cpuCmd.cycle();
        cyclesCmd++;
    }

    E2TEST_ASSERT_EQ(1, cyclesWdc);
    E2TEST_ASSERT_EQ(1, cyclesCmd);
    E2TEST_ASSERT_EQ(startPc + 1, cpuWdc.getProgramCounter());
    E2TEST_ASSERT_EQ(startPc + 2, cpuCmd.getProgramCounter());
}

E2TEST_CASE(cmdProfileAllMeasuredNopCyclesMatchHardwareTable)
{
    const int cmdNopCycleExpectations[][2] = {
            { 0x02, 2 }, { 0x03, 1 }, { 0x07, 1 }, { 0x0B, 1 }, { 0x0F, 1 }, { 0x13, 1 }, { 0x17, 1 }, { 0x1B, 1 },
            { 0x1F, 1 }, { 0x22, 2 }, { 0x23, 1 }, { 0x27, 1 }, { 0x2B, 1 }, { 0x2F, 1 }, { 0x33, 1 }, { 0x37, 1 },
            { 0x3B, 1 }, { 0x3F, 1 }, { 0x42, 2 }, { 0x43, 1 }, { 0x44, 3 }, { 0x47, 1 }, { 0x4B, 1 }, { 0x4F, 1 },
            { 0x53, 1 }, { 0x54, 4 }, { 0x57, 1 }, { 0x5B, 1 }, { 0x5C, 8 }, { 0x5F, 1 }, { 0x62, 2 }, { 0x63, 1 },
            { 0x67, 1 }, { 0x6B, 1 }, { 0x6F, 1 }, { 0x73, 1 }, { 0x77, 1 }, { 0x7B, 1 }, { 0x7F, 1 }, { 0x82, 2 },
            { 0x83, 1 }, { 0x87, 1 }, { 0x8B, 1 }, { 0x8F, 1 }, { 0x93, 1 }, { 0x97, 1 }, { 0x9B, 1 }, { 0x9F, 1 },
            { 0xA3, 1 }, { 0xA7, 1 }, { 0xAB, 1 }, { 0xAF, 1 }, { 0xB3, 1 }, { 0xB7, 1 }, { 0xBB, 1 }, { 0xBF, 1 },
            { 0xC2, 2 }, { 0xC3, 1 }, { 0xC7, 1 }, { 0xCB, 1 }, { 0xCF, 1 }, { 0xD3, 1 }, { 0xD4, 4 }, { 0xD7, 1 },
            { 0xDB, 1 }, { 0xDC, 4 }, { 0xDF, 1 }, { 0xE2, 2 }, { 0xE3, 1 }, { 0xE7, 1 }, { 0xEA, 2 }, { 0xEB, 1 },
            { 0xEF, 1 }, { 0xF3, 1 }, { 0xF4, 4 }, { 0xF7, 1 }, { 0xFB, 1 }, { 0xFC, 4 }, { 0xFF, 1 },
    };

    for( size_t i = 0; i<sizeof(cmdNopCycleExpectations)/sizeof(cmdNopCycleExpectations[0]); ++i ) {
        const int opcode = cmdNopCycleExpectations[i][0] & 0xFF;
        const int expectedCycles = cmdNopCycleExpectations[i][1];
        const Cpu65c02::OpcodeDescriptor actual =
                Cpu65c02::getOpcodeDescriptorForProfile(Cpu65c02::PROFILE_CMD, static_cast<uint16_t>(opcode));
        E2TEST_ASSERT_EQ(static_cast<int>(Cpu65c02::_NOP), static_cast<int>(actual.mnemonic));
        E2TEST_ASSERT_EQ(expectedCycles, static_cast<int>(actual.cycleTime));
    }
}

E2TEST_CASE(cmdProfileMeasuredCompatibilityNopWidthsMatchMameTable)
{
    const int cmdNopWidthExpectations[][2] = {
            { 0x07, 2 }, { 0x0F, 3 }, { 0x17, 2 }, { 0x1F, 3 }, { 0x27, 2 }, { 0x2F, 3 }, { 0x37, 2 }, { 0x3F, 3 },
            { 0x47, 2 }, { 0x4F, 3 }, { 0x57, 2 }, { 0x5F, 3 }, { 0x67, 2 }, { 0x6F, 3 }, { 0x77, 2 }, { 0x7F, 3 },
            { 0x87, 2 }, { 0x8F, 3 }, { 0x97, 2 }, { 0x9F, 3 }, { 0xA7, 2 }, { 0xAF, 3 }, { 0xB7, 2 }, { 0xBF, 3 },
            { 0xC7, 2 }, { 0xCF, 3 }, { 0xD7, 2 }, { 0xDF, 3 }, { 0xE7, 2 }, { 0xEF, 3 }, { 0xF7, 2 }, { 0xFF, 3 },
    };

    for( size_t i = 0; i<sizeof(cmdNopWidthExpectations)/sizeof(cmdNopWidthExpectations[0]); ++i ) {
        const int opcode = cmdNopWidthExpectations[i][0] & 0xFF;
        const int expectedWidth = cmdNopWidthExpectations[i][1];
        const Cpu65c02::OpcodeDescriptor actual =
                Cpu65c02::getOpcodeDescriptorForProfile(Cpu65c02::PROFILE_CMD, static_cast<uint16_t>(opcode));
        E2TEST_ASSERT_EQ(static_cast<int>(Cpu65c02::_NOP), static_cast<int>(actual.mnemonic));
        E2TEST_ASSERT_EQ(expectedWidth, static_cast<int>(actual.instrSize));
    }
}

E2TEST_CASE(runtimeCmdProfileUsesHardwareValidatedNopTimingAndLength)
{
    ScopedCwd cwd("release");
    E2TEST_ASSERT_TRUE(cwd.active());

    std::unique_ptr<Memory128k> memory(new Memory128k());
    Cpu65c02 cpu(memory.get(), Cpu65c02::PROFILE_CMD);

    const Uint16 startPc = 0x0200;
    memory->putMem(startPc + 0, 0x03);
    memory->putMem(startPc + 1, 0x5C);
    memory->putMem(startPc + 2, 0x00);
    memory->putMem(startPc + 3, 0x00);
    memory->putMem(startPc + 4, 0xEA);

    const Cpu65c02::OpcodeDescriptor cmd03 =
            Cpu65c02::getOpcodeDescriptorForProfile(Cpu65c02::PROFILE_CMD, 0x03);
    primeCpuAtOpcode(cpu, startPc, 0x03, cmd03.cycleTime);
    const int cycles03 = runUntilProgramCounterChanges(cpu, startPc);
    E2TEST_ASSERT_EQ(1, cycles03);
    E2TEST_ASSERT_EQ(startPc + 1, cpu.getProgramCounter());

    const Cpu65c02::OpcodeDescriptor cmd5c =
            Cpu65c02::getOpcodeDescriptorForProfile(Cpu65c02::PROFILE_CMD, 0x5C);
    primeCpuAtOpcode(cpu, startPc + 1, 0x5C, cmd5c.cycleTime);
    const int cycles5c = runUntilProgramCounterChanges(cpu, startPc + 1);
    E2TEST_ASSERT_EQ(8, cycles5c);
    E2TEST_ASSERT_EQ(startPc + 4, cpu.getProgramCounter());
}

E2TEST_CASE(runtimeCmdEightCycleUndocumentedNopRetiresAfterExactlyEightCycles)
{
    ScopedCwd cwd("release");
    E2TEST_ASSERT_TRUE(cwd.active());

    std::unique_ptr<Memory128k> memory(new Memory128k());
    Cpu65c02 cpu(memory.get(), Cpu65c02::PROFILE_CMD);

    const Uint16 startPc = 0x0200;
    memory->putMem(startPc + 0, 0x5C);
    memory->putMem(startPc + 1, 0x00);
    memory->putMem(startPc + 2, 0x00);
    memory->putMem(startPc + 3, 0xEA);

    const Cpu65c02::OpcodeDescriptor cmd5c =
            Cpu65c02::getOpcodeDescriptorForProfile(Cpu65c02::PROFILE_CMD, 0x5C);
    primeCpuAtOpcode(cpu, startPc, 0x5C, cmd5c.cycleTime);

    for( int i = 0; i<7; ++i ) {
        cpu.cycle();
        E2TEST_ASSERT_EQ(startPc, cpu.getProgramCounter());
    }
    cpu.cycle();
    E2TEST_ASSERT_EQ(startPc + 3, cpu.getProgramCounter());
}

E2TEST_CASE(runtimeJmpAbsIndXHasNoPageCrossPenalty)
{
    ScopedCwd cwd("release");
    E2TEST_ASSERT_TRUE(cwd.active());

    std::unique_ptr<Memory128k> memory(new Memory128k());
    Cpu65c02 cpu(memory.get(), Cpu65c02::PROFILE_CMD);

    const Uint16 startPc = 0x0200;
    memory->putMem(startPc + 0, 0x7C);
    memory->putMem(startPc + 1, 0xFF);
    memory->putMem(startPc + 2, 0x20);
    memory->putMem(0x2100, 0x34);
    memory->putMem(0x2101, 0x12);

    const Cpu65c02::OpcodeDescriptor jmpAbsIndX =
            Cpu65c02::getOpcodeDescriptorForProfile(Cpu65c02::PROFILE_CMD, 0x7C);
    primeCpuState(cpu, startPc, 0x7C, jmpAbsIndX.cycleTime, 0x00, 0x00, 0x01, 0x24, 0xFF);
    const int cycles = runUntilProgramCounterChanges(cpu, startPc);

    E2TEST_ASSERT_EQ(6, cycles);
    E2TEST_ASSERT_EQ(0x1234, cpu.getProgramCounter());
}

E2TEST_CASE(runtimeStaAbsXHasNoExtraPageCrossCycle)
{
    ScopedCwd cwd("release");
    E2TEST_ASSERT_TRUE(cwd.active());

    std::unique_ptr<Memory128k> memory(new Memory128k());
    Cpu65c02 cpu(memory.get(), Cpu65c02::PROFILE_CMD);

    const Uint16 startPc = 0x0200;
    memory->putMem(startPc + 0, 0x9D);
    memory->putMem(startPc + 1, 0xFF);
    memory->putMem(startPc + 2, 0x20);

    const Cpu65c02::OpcodeDescriptor staAbsX =
            Cpu65c02::getOpcodeDescriptorForProfile(Cpu65c02::PROFILE_CMD, 0x9D);
    primeCpuState(cpu, startPc, 0x9D, staAbsX.cycleTime, 0xA5, 0x00, 0x01, 0x24, 0xFF);
    const int cycles = runUntilProgramCounterChanges(cpu, startPc);

    E2TEST_ASSERT_EQ(5, cycles);
    E2TEST_ASSERT_EQ(startPc + 3, cpu.getProgramCounter());
    E2TEST_ASSERT_EQ(0xA5, static_cast<int>(memory->getMem(0x2100)));
}
