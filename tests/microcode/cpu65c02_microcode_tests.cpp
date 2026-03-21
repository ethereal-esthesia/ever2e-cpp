#include "test_harness.h"

#include <array>
#include <cstdint>
#include <sstream>
#include <vector>

#include "cpu/cpu65c02_microcode.h"
#include "cpu/cpu65c02_opcode.h"
#include "cpu/cpu65c02_opcode_view.h"
#include "cpu65c02.h"

namespace {

using AccessType = Cpu65c02Opcode::AccessType;
using MicroOp = Cpu65c02Opcode::MicroOp;
using Id = Cpu65c02Opcode::Id;

struct FamilySpec {
    const char* name;
    const std::vector<Id>& (*familyFn)();
    std::vector<uint8_t> (*bytesFn)();
};

static std::vector<FamilySpec> familySpecs()
{
    return {
            {"lda", &Cpu65c02Opcode::ldaFamily, &Cpu65c02Opcode::ldaOpcodeBytes},
            {"sta", &Cpu65c02Opcode::staFamily, &Cpu65c02Opcode::staOpcodeBytes},
            {"inc", &Cpu65c02Opcode::incFamily, &Cpu65c02Opcode::incOpcodeBytes},
            {"dec", &Cpu65c02Opcode::decFamily, &Cpu65c02Opcode::decOpcodeBytes},
            {"asl", &Cpu65c02Opcode::aslFamily, &Cpu65c02Opcode::aslOpcodeBytes},
            {"lsr", &Cpu65c02Opcode::lsrFamily, &Cpu65c02Opcode::lsrOpcodeBytes},
            {"rol", &Cpu65c02Opcode::rolFamily, &Cpu65c02Opcode::rolOpcodeBytes},
            {"ror", &Cpu65c02Opcode::rorFamily, &Cpu65c02Opcode::rorOpcodeBytes},
            {"ora", &Cpu65c02Opcode::oraFamily, &Cpu65c02Opcode::oraOpcodeBytes},
            {"and", &Cpu65c02Opcode::andFamily, &Cpu65c02Opcode::andOpcodeBytes},
            {"eor", &Cpu65c02Opcode::eorFamily, &Cpu65c02Opcode::eorOpcodeBytes},
            {"adc", &Cpu65c02Opcode::adcFamily, &Cpu65c02Opcode::adcOpcodeBytes},
            {"sbc", &Cpu65c02Opcode::sbcFamily, &Cpu65c02Opcode::sbcOpcodeBytes},
            {"cmp", &Cpu65c02Opcode::cmpFamily, &Cpu65c02Opcode::cmpOpcodeBytes},
            {"bit", &Cpu65c02Opcode::bitFamily, &Cpu65c02Opcode::bitOpcodeBytes},
            {"ldx", &Cpu65c02Opcode::ldxFamily, &Cpu65c02Opcode::ldxOpcodeBytes},
            {"ldy", &Cpu65c02Opcode::ldyFamily, &Cpu65c02Opcode::ldyOpcodeBytes},
            {"stx", &Cpu65c02Opcode::stxFamily, &Cpu65c02Opcode::stxOpcodeBytes},
            {"sty", &Cpu65c02Opcode::styFamily, &Cpu65c02Opcode::styOpcodeBytes},
            {"cpx", &Cpu65c02Opcode::cpxFamily, &Cpu65c02Opcode::cpxOpcodeBytes},
            {"cpy", &Cpu65c02Opcode::cpyFamily, &Cpu65c02Opcode::cpyOpcodeBytes},
            {"jump", &Cpu65c02Opcode::jumpFamily, &Cpu65c02Opcode::jumpOpcodeBytes},
            {"bitTestSet", &Cpu65c02Opcode::bitTestSetFamily, &Cpu65c02Opcode::bitTestSetOpcodeBytes},
            {"transfer", &Cpu65c02Opcode::transferFamily, &Cpu65c02Opcode::transferOpcodeBytes},
            {"indexIncDec", &Cpu65c02Opcode::indexIncDecFamily, &Cpu65c02Opcode::indexIncDecOpcodeBytes},
            {"stz", &Cpu65c02Opcode::stzFamily, &Cpu65c02Opcode::stzOpcodeBytes},
            {"interruptControlFlow", &Cpu65c02Opcode::interruptControlFlowFamily, &Cpu65c02Opcode::interruptControlFlowOpcodeBytes},
            {"readModifyWrite", &Cpu65c02Opcode::readModifyWriteFamily, &Cpu65c02Opcode::readModifyWriteOpcodeBytes},
            {"logicalArithmetic", &Cpu65c02Opcode::logicalArithmeticFamily, &Cpu65c02Opcode::logicalArithmeticOpcodeBytes},
            {"stack", &Cpu65c02Opcode::stackFamily, &Cpu65c02Opcode::stackOpcodeBytes},
            {"flags", &Cpu65c02Opcode::flagsFamily, &Cpu65c02Opcode::flagsOpcodeBytes},
            {"jsr", &Cpu65c02Opcode::jsrFamily, &Cpu65c02Opcode::jsrOpcodeBytes},
            {"branch", &Cpu65c02Opcode::branchFamily, &Cpu65c02Opcode::branchOpcodeBytes},
            {"controlMisc", &Cpu65c02Opcode::controlMiscFamily, &Cpu65c02Opcode::controlMiscOpcodeBytes},
    };
}

} // namespace

E2TEST_CASE(microOpNamesUseMPrefixAndStableSequentialCodes)
{
    const std::array<const char*, 11> names = {
            "M_FETCH_OPCODE",
            "M_FETCH_OPERAND_LO",
            "M_FETCH_OPERAND_HI",
            "M_READ_IMM_DATA",
            "M_READ_ZP_PTR_LO",
            "M_READ_ZP_PTR_HI",
            "M_READ_DUMMY",
            "M_READ_EA",
            "M_WRITE_EA_DUMMY",
            "M_WRITE_EA",
            "M_INTERNAL",
    };
    for( const char* name : names )
        E2TEST_ASSERT_TRUE(name[0]=='M' && name[1]=='_');

    E2TEST_ASSERT_EQ(0, static_cast<int>(MicroOp::M_FETCH_OPCODE));
    E2TEST_ASSERT_EQ(1, static_cast<int>(MicroOp::M_FETCH_OPERAND_LO));
    E2TEST_ASSERT_EQ(2, static_cast<int>(MicroOp::M_FETCH_OPERAND_HI));
    E2TEST_ASSERT_EQ(3, static_cast<int>(MicroOp::M_READ_IMM_DATA));
    E2TEST_ASSERT_EQ(4, static_cast<int>(MicroOp::M_READ_ZP_PTR_LO));
    E2TEST_ASSERT_EQ(5, static_cast<int>(MicroOp::M_READ_ZP_PTR_HI));
    E2TEST_ASSERT_EQ(6, static_cast<int>(MicroOp::M_READ_DUMMY));
    E2TEST_ASSERT_EQ(7, static_cast<int>(MicroOp::M_READ_EA));
    E2TEST_ASSERT_EQ(8, static_cast<int>(MicroOp::M_WRITE_EA_DUMMY));
    E2TEST_ASSERT_EQ(9, static_cast<int>(MicroOp::M_WRITE_EA));
    E2TEST_ASSERT_EQ(10, static_cast<int>(MicroOp::M_INTERNAL));
}

E2TEST_CASE(allFamiliesMatchByteListsAndResolvedMicrocode)
{
    const std::vector<FamilySpec> specs = familySpecs();
    for( const FamilySpec& spec : specs ) {
        const std::vector<Id>& family = spec.familyFn();
        const std::vector<uint8_t> bytes = spec.bytesFn();
        E2TEST_ASSERT_EQ(static_cast<int>(family.size()), static_cast<int>(bytes.size()));
        for( size_t i = 0; i<family.size(); ++i ) {
            const Cpu65c02Opcode::Definition& def = Cpu65c02Opcode::byId(family[i]);
            E2TEST_ASSERT_EQ(static_cast<int>(def.opcodeByte), static_cast<int>(bytes[i]));

            const Cpu65c02OpcodeView view = Cpu65c02Microcode::opcodeForByte(def.opcodeByte);
            E2TEST_ASSERT_TRUE(view.getAccessType()==def.microcode.accessType);
            E2TEST_ASSERT_VEC_EQ(def.microcode.noCrossScript, view.getExpectedMnemonicOrder(false));
            E2TEST_ASSERT_VEC_EQ(def.microcode.crossScript, view.getExpectedMnemonicOrder(true));
        }
    }
}

E2TEST_CASE(opcodeByteRoundTripsToExplicitDefinitions)
{
    for( Id id : Cpu65c02Opcode::values() ) {
        const Cpu65c02Opcode::Definition& def = Cpu65c02Opcode::byId(id);
        const Cpu65c02Opcode::Definition* fromByte = Cpu65c02Opcode::fromOpcodeByte(def.opcodeByte);
        E2TEST_ASSERT_TRUE(fromByte!=nullptr);
        E2TEST_ASSERT_EQ(static_cast<int>(def.opcodeByte), static_cast<int>(fromByte->opcodeByte));
    }
}

E2TEST_CASE(everyOpcodeHasMicroInstrEntry)
{
    for( int op = 0; op<256; ++op ) {
        const Cpu65c02OpcodeView view = Cpu65c02Microcode::opcodeForByte(op);
        E2TEST_ASSERT_EQ(op, view.getOpcodeByte());
        const std::vector<MicroOp> noCross = view.getExpectedMnemonicOrder(false);
        E2TEST_ASSERT_TRUE(!noCross.empty());
        E2TEST_ASSERT_TRUE(noCross[0]==MicroOp::M_FETCH_OPCODE);
    }
}

E2TEST_CASE(pageCrossPenaltyReadOpcodesUseSplitScriptWithDummyRead)
{
    const int opcodes[] = {
            0x7D, 0x79, 0x71,
            0x3D, 0x39, 0x31,
            0xDD, 0xD9, 0xD1,
            0x5D, 0x59, 0x51,
            0xBD, 0xB9, 0xB1,
            0x1D, 0x19, 0x11,
            0xFD, 0xF9, 0xF1,
            0xBC,
            0xBE,
    };

    for( int opcode : opcodes ) {
        const Cpu65c02OpcodeView view = Cpu65c02Microcode::opcodeForByte(opcode);
        const std::vector<MicroOp> noCross = view.getExpectedMnemonicOrder(false);
        const std::vector<MicroOp> cross = view.getExpectedMnemonicOrder(true);
        E2TEST_ASSERT_EQ(static_cast<int>(noCross.size()) + 1, static_cast<int>(cross.size()));
        E2TEST_ASSERT_TRUE(cross[cross.size() - 2] == MicroOp::M_READ_DUMMY);
        E2TEST_ASSERT_TRUE(cross[cross.size() - 1] == MicroOp::M_READ_EA);
    }
}

E2TEST_CASE(noPenaltyAbsoluteIndexedReadsDoNotSplit)
{
    const int opcodes[] = { 0x3C, 0x5C, 0xDC, 0xFC };
    for( int opcode : opcodes ) {
        const Cpu65c02OpcodeView view = Cpu65c02Microcode::opcodeForByte(opcode);
        E2TEST_ASSERT_VEC_EQ(view.getExpectedMnemonicOrder(false), view.getExpectedMnemonicOrder(true));
    }
}

E2TEST_CASE(allBranchOpcodesExposeTakenMicrocodeStep)
{
    const int branches[] = { 0x10, 0x30, 0x50, 0x70, 0x80, 0x90, 0xB0, 0xD0, 0xF0 };
    const std::vector<MicroOp> expectedBase = { MicroOp::M_FETCH_OPCODE, MicroOp::M_READ_IMM_DATA };
    const std::vector<MicroOp> expectedTaken = { MicroOp::M_FETCH_OPCODE, MicroOp::M_READ_IMM_DATA, MicroOp::M_INTERNAL };
    for( int opcode : branches ) {
        const Cpu65c02OpcodeView view = Cpu65c02Microcode::opcodeForByte(opcode);
        E2TEST_ASSERT_VEC_EQ(expectedBase, view.getExpectedMnemonicOrder(false));
        E2TEST_ASSERT_VEC_EQ(expectedTaken, view.getExpectedMnemonicOrder(true));
    }
}

E2TEST_CASE(microContextExposesCpuAndInternalRegisters)
{
    Cpu65c02Microcode::MicroContext ctx = Cpu65c02Microcode::newContext();
    E2TEST_ASSERT_EQ(0, ctx.cpu.a);
    E2TEST_ASSERT_EQ(0, ctx.cpu.x);
    E2TEST_ASSERT_EQ(0, ctx.cpu.y);
    E2TEST_ASSERT_EQ(0, ctx.cpu.pc);
    E2TEST_ASSERT_EQ(0, ctx.internal.effectiveAddress);
    E2TEST_ASSERT_EQ(0, ctx.internal.cycleIndex);
    E2TEST_ASSERT_TRUE(!ctx.internal.pageCrossed);

    ctx.cpu.a = 0x42;
    ctx.internal.effectiveAddress = 0xC054;
    ctx.internal.pageCrossed = true;

    E2TEST_ASSERT_EQ(0x42, ctx.cpu.a);
    E2TEST_ASSERT_EQ(0xC054, ctx.internal.effectiveAddress);
    E2TEST_ASSERT_TRUE(ctx.internal.pageCrossed);
}

E2TEST_CASE(allOpcodesUseNonFallbackScriptsBeyondSingleCycle)
{
    for( int opcode = 0; opcode<256; ++opcode ) {
        const Cpu65c02::OpcodeDescriptor cpuOp = Cpu65c02::getOpcodeDescriptor(static_cast<uint16_t>(opcode));
        const Cpu65c02OpcodeView view = Cpu65c02Microcode::opcodeForByte(opcode);
        const std::vector<MicroOp> noCross = view.getExpectedMnemonicOrder(false);
        const std::vector<MicroOp> cross = view.getExpectedMnemonicOrder(true);

        E2TEST_ASSERT_TRUE(!noCross.empty());
        E2TEST_ASSERT_TRUE(!cross.empty());

        if( cpuOp.cycleTime>1 )
            E2TEST_ASSERT_TRUE(!(noCross.size()==1 && noCross[0]==MicroOp::M_FETCH_OPCODE));

        if( cross.size()>noCross.size() )
            E2TEST_ASSERT_EQ(static_cast<int>(noCross.size()) + 1, static_cast<int>(cross.size()));
    }
}

E2TEST_CASE(allOpcodesMicrocodeLengthMatchesCpuCycleTable)
{
    int mismatchCount = 0;
    int firstOpcode = -1;
    int firstExpected = -1;
    int firstActual = -1;
    for( int opcode = 0; opcode<256; ++opcode ) {
        const Cpu65c02::OpcodeDescriptor cpuOp = Cpu65c02::getOpcodeDescriptor(static_cast<uint16_t>(opcode));
        const Cpu65c02OpcodeView view = Cpu65c02Microcode::opcodeForByte(opcode);
        const std::vector<MicroOp> noCross = view.getExpectedMnemonicOrder(false);
        const std::vector<MicroOp> cross = view.getExpectedMnemonicOrder(true);

        if( static_cast<int>(cpuOp.cycleTime)!=static_cast<int>(noCross.size()) ) {
            ++mismatchCount;
            if( firstOpcode<0 ) {
                firstOpcode = opcode;
                firstExpected = static_cast<int>(cpuOp.cycleTime);
                firstActual = static_cast<int>(noCross.size());
            }
        }
        if( cross.size()!=noCross.size() )
            E2TEST_ASSERT_EQ(static_cast<int>(noCross.size()) + 1, static_cast<int>(cross.size()));
    }
    if( mismatchCount>0 ) {
        std::ostringstream oss;
        oss << "cycle length mismatches=" << mismatchCount
            << " first opcode=$" << std::hex << std::uppercase << firstOpcode
            << std::dec << " expected=" << firstExpected
            << " actual=" << firstActual;
        e2test::fail(oss.str());
    }
}
