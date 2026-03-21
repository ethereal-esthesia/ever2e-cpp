#ifndef EVER2E_CPU65C02_CMD_PROFILE_H
#define EVER2E_CPU65C02_CMD_PROFILE_H

#include <array>
#include <cstdint>

#include "cpu65c02.h"

class Cpu65c02CmdProfile {
public:
    static const std::array<Cpu65c02::OpcodeDescriptor, 256>& opcodeTable();
    static Cpu65c02::OpcodeDescriptor opcodeAt(uint8_t opcodeByte);
};

#endif
