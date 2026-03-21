#include "cpu/cpu65c02_cmd_profile.h"

#include <array>

namespace {

std::array<Cpu65c02::OpcodeDescriptor, 256> buildCmdTable()
{
    std::array<Cpu65c02::OpcodeDescriptor, 256> cmd{};
    for( int i = 0; i<256; ++i )
        cmd[i] = Cpu65c02::getOpcodeDescriptor(static_cast<uint16_t>(i));

    const int nopWidthOverrides[][2] = {
            { 0x07, 2 }, { 0x0F, 3 }, { 0x17, 2 }, { 0x1F, 3 }, { 0x27, 2 }, { 0x2F, 3 }, { 0x37, 2 }, { 0x3F, 3 },
            { 0x47, 2 }, { 0x4F, 3 }, { 0x57, 2 }, { 0x5F, 3 }, { 0x67, 2 }, { 0x6F, 3 }, { 0x77, 2 }, { 0x7F, 3 },
            { 0x87, 2 }, { 0x8F, 3 }, { 0x97, 2 }, { 0x9F, 3 }, { 0xA7, 2 }, { 0xAF, 3 }, { 0xB7, 2 }, { 0xBF, 3 },
            { 0xC7, 2 }, { 0xCF, 3 }, { 0xD7, 2 }, { 0xDF, 3 }, { 0xE7, 2 }, { 0xEF, 3 }, { 0xF7, 2 }, { 0xFF, 3 },
    };

    for( const auto& pair : nopWidthOverrides ) {
        const int opcode = pair[0] & 0xFF;
        const int width = pair[1];
        Cpu65c02::OpcodeDescriptor d = cmd[opcode];
        d.mnemonic = static_cast<uint8_t>(Cpu65c02::_NOP);
        d.addressMode = static_cast<uint8_t>(Cpu65c02::_IMP);
        d.instrSize = static_cast<uint8_t>(width);
        cmd[opcode] = d;
    }

    {
        Cpu65c02::OpcodeDescriptor d = cmd[0x03];
        d.mnemonic = static_cast<uint8_t>(Cpu65c02::_NOP);
        d.addressMode = static_cast<uint8_t>(Cpu65c02::_IMP);
        d.instrSize = 1;
        d.cycleTime = 1;
        cmd[0x03] = d;
    }

    {
        Cpu65c02::OpcodeDescriptor d = cmd[0x5C];
        d.mnemonic = static_cast<uint8_t>(Cpu65c02::_NOP);
        d.addressMode = static_cast<uint8_t>(Cpu65c02::_IMP);
        d.instrSize = 3;
        d.cycleTime = 8;
        cmd[0x5C] = d;
    }

    return cmd;
}

} // namespace

const std::array<Cpu65c02::OpcodeDescriptor, 256>& Cpu65c02CmdProfile::opcodeTable()
{
    static const std::array<Cpu65c02::OpcodeDescriptor, 256> table = buildCmdTable();
    return table;
}

Cpu65c02::OpcodeDescriptor Cpu65c02CmdProfile::opcodeAt(uint8_t opcodeByte)
{
    return opcodeTable()[opcodeByte];
}
