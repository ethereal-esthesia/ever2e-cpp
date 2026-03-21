#include "cpu/cpu65c02_microcode.h"

#include <algorithm>
#include <array>
#include <vector>

#include "cpu/cpu65c02_opcode_view.h"
#include "cpu65c02.h"

namespace {

using AccessType = Cpu65c02Opcode::AccessType;
using MicroOp = Cpu65c02Opcode::MicroOp;
using MicroInstr = Cpu65c02Microcode::MicroInstr;

void set(
        std::array<MicroInstr, 256>& table,
        int opcodeByte,
        AccessType accessType,
        const std::vector<MicroOp>& noCross,
        const std::vector<MicroOp>& cross)
{
    table[opcodeByte & 0xff] = MicroInstr(accessType, noCross, cross);
}

AccessType inferAccessType(Cpu65c02::OpcodeMnemonic mnemonic)
{
    switch( mnemonic ) {
        case Cpu65c02::_STA:
        case Cpu65c02::_STX:
        case Cpu65c02::_STY:
        case Cpu65c02::_STZ:
            return AccessType::AT_WRITE;
        case Cpu65c02::_ASL:
        case Cpu65c02::_LSR:
        case Cpu65c02::_ROL:
        case Cpu65c02::_ROR:
        case Cpu65c02::_INC:
        case Cpu65c02::_DEC:
        case Cpu65c02::_TRB:
        case Cpu65c02::_TSB:
            return AccessType::AT_RMW;
        case Cpu65c02::_ADC:
        case Cpu65c02::_AND:
        case Cpu65c02::_BIT:
        case Cpu65c02::_CMP:
        case Cpu65c02::_CPX:
        case Cpu65c02::_CPY:
        case Cpu65c02::_EOR:
        case Cpu65c02::_LDA:
        case Cpu65c02::_LDX:
        case Cpu65c02::_LDY:
        case Cpu65c02::_ORA:
        case Cpu65c02::_SBC:
            return AccessType::AT_READ;
        default:
            return AccessType::AT_NONE;
    }
}

bool hasPageCrossReadVariant(const Cpu65c02::OpcodeDescriptor& opcode, AccessType accessType)
{
    if( accessType!=AccessType::AT_READ )
        return false;

    const Cpu65c02::AddressMode mode = static_cast<Cpu65c02::AddressMode>(opcode.addressMode);
    const Cpu65c02::OpcodeMnemonic mnemonic = static_cast<Cpu65c02::OpcodeMnemonic>(opcode.mnemonic);

    switch( mode ) {
        case Cpu65c02::_ABS_X:
            switch( mnemonic ) {
                case Cpu65c02::_ADC:
                case Cpu65c02::_AND:
                case Cpu65c02::_CMP:
                case Cpu65c02::_EOR:
                case Cpu65c02::_LDA:
                case Cpu65c02::_LDY:
                case Cpu65c02::_ORA:
                case Cpu65c02::_SBC:
                    return true;
                default:
                    return false;
            }
        case Cpu65c02::_ABS_Y:
            switch( mnemonic ) {
                case Cpu65c02::_ADC:
                case Cpu65c02::_AND:
                case Cpu65c02::_CMP:
                case Cpu65c02::_EOR:
                case Cpu65c02::_LDA:
                case Cpu65c02::_LDX:
                case Cpu65c02::_ORA:
                case Cpu65c02::_SBC:
                    return true;
                default:
                    return false;
            }
        case Cpu65c02::_IND_Y:
            return true;
        default:
            return false;
    }
}

void appendEaAccess(std::vector<MicroOp>& ops, AccessType accessType)
{
    switch( accessType ) {
        case AccessType::AT_READ:
            ops.push_back(MicroOp::M_READ_EA);
            break;
        case AccessType::AT_WRITE:
            ops.push_back(MicroOp::M_WRITE_EA);
            break;
        case AccessType::AT_RMW:
            ops.push_back(MicroOp::M_READ_EA);
            ops.push_back(MicroOp::M_WRITE_EA_DUMMY);
            ops.push_back(MicroOp::M_WRITE_EA);
            break;
        case AccessType::AT_NONE:
        default:
            ops.push_back(MicroOp::M_INTERNAL);
            break;
    }
}

std::vector<MicroOp> buildGenericScript(
        const Cpu65c02::OpcodeDescriptor& opcode,
        AccessType accessType,
        bool pageCrossed)
{
    std::vector<MicroOp> ops;
    ops.push_back(MicroOp::M_FETCH_OPCODE);

    const Cpu65c02::AddressMode mode = static_cast<Cpu65c02::AddressMode>(opcode.addressMode);
    switch( mode ) {
        case Cpu65c02::_IMM:
            ops.push_back(MicroOp::M_READ_IMM_DATA);
            break;
        case Cpu65c02::_ZPG:
            ops.push_back(MicroOp::M_FETCH_OPERAND_LO);
            appendEaAccess(ops, accessType);
            break;
        case Cpu65c02::_ZPG_X:
        case Cpu65c02::_ZPG_Y:
            ops.push_back(MicroOp::M_FETCH_OPERAND_LO);
            ops.push_back(MicroOp::M_READ_DUMMY);
            appendEaAccess(ops, accessType);
            break;
        case Cpu65c02::_ABS:
            ops.push_back(MicroOp::M_FETCH_OPERAND_LO);
            ops.push_back(MicroOp::M_FETCH_OPERAND_HI);
            appendEaAccess(ops, accessType);
            break;
        case Cpu65c02::_ABS_X:
        case Cpu65c02::_ABS_Y:
            ops.push_back(MicroOp::M_FETCH_OPERAND_LO);
            ops.push_back(MicroOp::M_FETCH_OPERAND_HI);
            if( accessType==AccessType::AT_WRITE || accessType==AccessType::AT_RMW || pageCrossed )
                ops.push_back(MicroOp::M_READ_DUMMY);
            appendEaAccess(ops, accessType);
            break;
        case Cpu65c02::_IND_X:
            ops.push_back(MicroOp::M_FETCH_OPERAND_LO);
            ops.push_back(MicroOp::M_READ_DUMMY);
            ops.push_back(MicroOp::M_READ_ZP_PTR_LO);
            ops.push_back(MicroOp::M_READ_ZP_PTR_HI);
            appendEaAccess(ops, accessType);
            break;
        case Cpu65c02::_IND_Y:
            ops.push_back(MicroOp::M_FETCH_OPERAND_LO);
            ops.push_back(MicroOp::M_READ_ZP_PTR_LO);
            ops.push_back(MicroOp::M_READ_ZP_PTR_HI);
            if( accessType==AccessType::AT_WRITE || accessType==AccessType::AT_RMW || pageCrossed )
                ops.push_back(MicroOp::M_READ_DUMMY);
            appendEaAccess(ops, accessType);
            break;
        case Cpu65c02::_ZPG_IND:
            ops.push_back(MicroOp::M_FETCH_OPERAND_LO);
            ops.push_back(MicroOp::M_READ_ZP_PTR_LO);
            ops.push_back(MicroOp::M_READ_ZP_PTR_HI);
            appendEaAccess(ops, accessType);
            break;
        case Cpu65c02::_ABS_IND:
            ops.push_back(MicroOp::M_FETCH_OPERAND_LO);
            ops.push_back(MicroOp::M_FETCH_OPERAND_HI);
            ops.push_back(MicroOp::M_READ_ZP_PTR_LO);
            ops.push_back(MicroOp::M_READ_ZP_PTR_HI);
            break;
        case Cpu65c02::_ABS_IND_X:
            ops.push_back(MicroOp::M_FETCH_OPERAND_LO);
            ops.push_back(MicroOp::M_FETCH_OPERAND_HI);
            ops.push_back(MicroOp::M_READ_DUMMY);
            ops.push_back(MicroOp::M_READ_ZP_PTR_LO);
            ops.push_back(MicroOp::M_READ_ZP_PTR_HI);
            break;
        case Cpu65c02::_REL:
            ops.push_back(MicroOp::M_FETCH_OPERAND_LO);
            break;
        case Cpu65c02::_ACC:
            ops.push_back(MicroOp::M_INTERNAL);
            break;
        case Cpu65c02::_IMP:
        default:
            break;
    }

    const int targetCycles = std::max(1, static_cast<int>(opcode.cycleTime));
    while( static_cast<int>(ops.size())<targetCycles )
        ops.push_back(MicroOp::M_INTERNAL);
    if( static_cast<int>(ops.size())>targetCycles )
        ops.resize(targetCycles);
    return ops;
}

MicroInstr buildGenericInstruction(int opcodeByte)
{
    const Cpu65c02::OpcodeDescriptor opcode = Cpu65c02::getOpcodeDescriptor(static_cast<uint16_t>(opcodeByte & 0xff));
    const AccessType accessType = inferAccessType(static_cast<Cpu65c02::OpcodeMnemonic>(opcode.mnemonic));
    const bool pageCrossVariant = hasPageCrossReadVariant(opcode, accessType);
    const std::vector<MicroOp> noCross = buildGenericScript(opcode, accessType, false);
    const std::vector<MicroOp> cross = pageCrossVariant ? buildGenericScript(opcode, accessType, true) : noCross;
    return MicroInstr(accessType, noCross, cross);
}

void installFamilyPrograms(
        std::array<MicroInstr, 256>& table,
        const std::vector<Cpu65c02Opcode::Id>& family)
{
    for( Cpu65c02Opcode::Id id : family ) {
        const Cpu65c02Opcode::Definition& def = Cpu65c02Opcode::byId(id);
        set(table,
                def.opcodeByte,
                def.microcode.accessType,
                def.microcode.noCrossScript,
                def.microcode.crossScript);
    }
}

} // namespace

Cpu65c02Microcode::MicroInstr::MicroInstr()
    : accessType_(AccessType::AT_NONE)
    , noCrossScript_(1, MicroOp::M_FETCH_OPCODE)
    , crossScript_(1, MicroOp::M_FETCH_OPCODE)
{
}

Cpu65c02Microcode::MicroInstr::MicroInstr(
        AccessType accessType,
        const std::vector<MicroOp>& noCrossScript,
        const std::vector<MicroOp>& crossScript)
    : accessType_(accessType)
    , noCrossScript_(noCrossScript)
    , crossScript_(crossScript)
{
}

AccessType Cpu65c02Microcode::MicroInstr::getAccessType() const
{
    return accessType_;
}

std::vector<MicroOp> Cpu65c02Microcode::MicroInstr::getCycleScript(bool pageCrossed) const
{
    return pageCrossed ? crossScript_ : noCrossScript_;
}

int Cpu65c02Microcode::MicroInstr::indexOfFirstReadDataCycle(bool pageCrossed) const
{
    const std::vector<MicroOp>& script = pageCrossed ? crossScript_ : noCrossScript_;
    for( size_t i = 0; i<script.size(); ++i ) {
        if( script[i]==MicroOp::M_READ_IMM_DATA || script[i]==MicroOp::M_READ_EA )
            return static_cast<int>(i);
    }
    return -1;
}

bool Cpu65c02Microcode::MicroInstr::usesMemoryDataRead() const
{
    return indexOfFirstReadDataCycle(false)>=0 || indexOfFirstReadDataCycle(true)>=0;
}

int Cpu65c02Microcode::MicroInstr::getOperandReadCycleOffset(bool pageCrossed) const
{
    return indexOfFirstReadDataCycle(pageCrossed);
}

std::array<Cpu65c02Microcode::MicroInstr, 256> Cpu65c02Microcode::buildTable()
{
    std::array<MicroInstr, 256> table;
    for( int i = 0; i<256; ++i )
        table[i] = buildGenericInstruction(i);

    installFamilyPrograms(table, Cpu65c02Opcode::ldaFamily());
    installFamilyPrograms(table, Cpu65c02Opcode::staFamily());
    installFamilyPrograms(table, Cpu65c02Opcode::incFamily());
    installFamilyPrograms(table, Cpu65c02Opcode::decFamily());
    installFamilyPrograms(table, Cpu65c02Opcode::aslFamily());
    installFamilyPrograms(table, Cpu65c02Opcode::lsrFamily());
    installFamilyPrograms(table, Cpu65c02Opcode::rolFamily());
    installFamilyPrograms(table, Cpu65c02Opcode::rorFamily());
    installFamilyPrograms(table, Cpu65c02Opcode::oraFamily());
    installFamilyPrograms(table, Cpu65c02Opcode::andFamily());
    installFamilyPrograms(table, Cpu65c02Opcode::eorFamily());
    installFamilyPrograms(table, Cpu65c02Opcode::adcFamily());
    installFamilyPrograms(table, Cpu65c02Opcode::sbcFamily());
    installFamilyPrograms(table, Cpu65c02Opcode::cmpFamily());
    installFamilyPrograms(table, Cpu65c02Opcode::bitFamily());
    installFamilyPrograms(table, Cpu65c02Opcode::ldxFamily());
    installFamilyPrograms(table, Cpu65c02Opcode::ldyFamily());
    installFamilyPrograms(table, Cpu65c02Opcode::stxFamily());
    installFamilyPrograms(table, Cpu65c02Opcode::styFamily());
    installFamilyPrograms(table, Cpu65c02Opcode::cpxFamily());
    installFamilyPrograms(table, Cpu65c02Opcode::cpyFamily());
    installFamilyPrograms(table, Cpu65c02Opcode::jsrFamily());
    installFamilyPrograms(table, Cpu65c02Opcode::branchFamily());
    installFamilyPrograms(table, Cpu65c02Opcode::controlMiscFamily());
    return table;
}

const Cpu65c02Microcode::MicroInstr& Cpu65c02Microcode::microInstrForByte(int opcodeByte)
{
    static const std::array<MicroInstr, 256> table = buildTable();
    return table[opcodeByte & 0xff];
}

Cpu65c02OpcodeView Cpu65c02Microcode::opcodeForByte(int opcodeByte)
{
    return Cpu65c02OpcodeView(opcodeByte & 0xff);
}

Cpu65c02Microcode::MicroContext Cpu65c02Microcode::newContext()
{
    MicroContext out;
    out.cpu.a = 0;
    out.cpu.x = 0;
    out.cpu.y = 0;
    out.cpu.s = 0;
    out.cpu.p = 0;
    out.cpu.pc = 0;

    out.internal.operandLo = 0;
    out.internal.operandHi = 0;
    out.internal.effectiveAddress = 0;
    out.internal.zpPtrLo = 0;
    out.internal.zpPtrHi = 0;
    out.internal.temp = 0;
    out.internal.dataLatch = 0;
    out.internal.cycleIndex = 0;
    out.internal.pageCrossed = false;
    return out;
}
