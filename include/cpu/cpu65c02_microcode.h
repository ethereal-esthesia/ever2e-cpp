#ifndef EVER2E_CPU65C02_MICROCODE_H
#define EVER2E_CPU65C02_MICROCODE_H

#include <array>
#include <cstdint>
#include <vector>

#include "cpu/cpu65c02_opcode.h"

class Cpu65c02OpcodeView;

class Cpu65c02Microcode {
public:
    struct CpuRegs {
        int a;
        int x;
        int y;
        int s;
        int p;
        int pc;
    };

    struct InternalRegs {
        int operandLo;
        int operandHi;
        int effectiveAddress;
        int zpPtrLo;
        int zpPtrHi;
        int temp;
        int dataLatch;
        int cycleIndex;
        bool pageCrossed;
    };

    struct MicroContext {
        CpuRegs cpu;
        InternalRegs internal;
    };

    class MicroInstr {
    public:
        MicroInstr();
        MicroInstr(
                Cpu65c02Opcode::AccessType accessType,
                const std::vector<Cpu65c02Opcode::MicroOp>& noCrossScript,
                const std::vector<Cpu65c02Opcode::MicroOp>& crossScript);

        Cpu65c02Opcode::AccessType getAccessType() const;
        std::vector<Cpu65c02Opcode::MicroOp> getCycleScript(bool pageCrossed) const;
        bool usesMemoryDataRead() const;
        int getOperandReadCycleOffset(bool pageCrossed) const;

    private:
        int indexOfFirstReadDataCycle(bool pageCrossed) const;

        Cpu65c02Opcode::AccessType accessType_;
        std::vector<Cpu65c02Opcode::MicroOp> noCrossScript_;
        std::vector<Cpu65c02Opcode::MicroOp> crossScript_;
    };

    static const MicroInstr& microInstrForByte(int opcodeByte);
    static Cpu65c02OpcodeView opcodeForByte(int opcodeByte);
    static MicroContext newContext();

private:
    static std::array<MicroInstr, 256> buildTable();
};

#endif
