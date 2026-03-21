#ifndef EVER2E_CPU65C02_OPCODE_VIEW_H
#define EVER2E_CPU65C02_OPCODE_VIEW_H

#include <cstdint>
#include <vector>

#include "cpu/cpu65c02_opcode.h"

class Cpu65c02OpcodeView {
public:
    explicit Cpu65c02OpcodeView(int opcodeByte);

    int getOpcodeByte() const;
    Cpu65c02Opcode::AccessType getAccessType() const;
    std::vector<Cpu65c02Opcode::MicroOp> getExpectedMnemonicOrder(bool pageCrossed) const;
    bool usesMemoryDataRead() const;
    int getOperandReadCycleOffset(bool pageCrossed) const;

private:
    uint8_t opcodeByte_;
};

#endif
