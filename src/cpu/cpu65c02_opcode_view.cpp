#include "cpu/cpu65c02_opcode_view.h"

#include "cpu/cpu65c02_microcode.h"

Cpu65c02OpcodeView::Cpu65c02OpcodeView(int opcodeByte)
    : opcodeByte_(static_cast<uint8_t>(opcodeByte & 0xff))
{
}

int Cpu65c02OpcodeView::getOpcodeByte() const
{
    return opcodeByte_;
}

Cpu65c02Opcode::AccessType Cpu65c02OpcodeView::getAccessType() const
{
    return Cpu65c02Microcode::microInstrForByte(opcodeByte_).getAccessType();
}

std::vector<Cpu65c02Opcode::MicroOp> Cpu65c02OpcodeView::getExpectedMnemonicOrder(bool pageCrossed) const
{
    return Cpu65c02Microcode::microInstrForByte(opcodeByte_).getCycleScript(pageCrossed);
}

bool Cpu65c02OpcodeView::usesMemoryDataRead() const
{
    return Cpu65c02Microcode::microInstrForByte(opcodeByte_).usesMemoryDataRead();
}

int Cpu65c02OpcodeView::getOperandReadCycleOffset(bool pageCrossed) const
{
    return Cpu65c02Microcode::microInstrForByte(opcodeByte_).getOperandReadCycleOffset(pageCrossed);
}
