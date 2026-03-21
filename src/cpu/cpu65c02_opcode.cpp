#include "cpu/cpu65c02_opcode.h"

#include <array>

namespace {

using MicroOp = Cpu65c02Opcode::MicroOp;
using AccessType = Cpu65c02Opcode::AccessType;
using Definition = Cpu65c02Opcode::Definition;
using MicroCycleProgram = Cpu65c02Opcode::MicroCycleProgram;

static const std::array<Definition, 177> kDefinitions = {
    {
        Definition{ "LDA_IMM", 0xA9, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_READ_IMM_DATA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_READ_IMM_DATA } } },
        Definition{ "LDA_ZPG", 0xA5, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_EA } } },
        Definition{ "LDA_ZPG_X", 0xB5, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_DUMMY, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_DUMMY, MicroOp::M_READ_EA } } },
        Definition{ "LDA_ABS", 0xAD, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_EA } } },
        Definition{ "LDA_ABS_X", 0xBD, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_DUMMY, MicroOp::M_READ_EA } } },
        Definition{ "LDA_ABS_Y", 0xB9, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_DUMMY, MicroOp::M_READ_EA } } },
        Definition{ "LDA_IND_X", 0xA1, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_DUMMY, MicroOp::M_READ_ZP_PTR_LO, MicroOp::M_READ_ZP_PTR_HI, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_DUMMY, MicroOp::M_READ_ZP_PTR_LO, MicroOp::M_READ_ZP_PTR_HI, MicroOp::M_READ_EA } } },
        Definition{ "LDA_IND_Y", 0xB1, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_ZP_PTR_LO, MicroOp::M_READ_ZP_PTR_HI, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_ZP_PTR_LO, MicroOp::M_READ_ZP_PTR_HI, MicroOp::M_READ_DUMMY, MicroOp::M_READ_EA } } },
        Definition{ "LDA_IND", 0xB2, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_ZP_PTR_LO, MicroOp::M_READ_ZP_PTR_HI, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_ZP_PTR_LO, MicroOp::M_READ_ZP_PTR_HI, MicroOp::M_READ_EA } } },
        Definition{ "STA_ZPG", 0x85, MicroCycleProgram{ AccessType::AT_WRITE, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_WRITE_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_WRITE_EA } } },
        Definition{ "STA_ZPG_X", 0x95, MicroCycleProgram{ AccessType::AT_WRITE, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_DUMMY, MicroOp::M_WRITE_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_DUMMY, MicroOp::M_WRITE_EA } } },
        Definition{ "STA_ABS", 0x8D, MicroCycleProgram{ AccessType::AT_WRITE, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_WRITE_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_WRITE_EA } } },
        Definition{ "STA_ABS_X", 0x9D, MicroCycleProgram{ AccessType::AT_WRITE, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_DUMMY, MicroOp::M_WRITE_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_DUMMY, MicroOp::M_WRITE_EA } } },
        Definition{ "STA_ABS_Y", 0x99, MicroCycleProgram{ AccessType::AT_WRITE, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_DUMMY, MicroOp::M_WRITE_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_DUMMY, MicroOp::M_WRITE_EA } } },
        Definition{ "STA_IND_X", 0x81, MicroCycleProgram{ AccessType::AT_WRITE, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_DUMMY, MicroOp::M_READ_ZP_PTR_LO, MicroOp::M_READ_ZP_PTR_HI, MicroOp::M_WRITE_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_DUMMY, MicroOp::M_READ_ZP_PTR_LO, MicroOp::M_READ_ZP_PTR_HI, MicroOp::M_WRITE_EA } } },
        Definition{ "STA_IND_Y", 0x91, MicroCycleProgram{ AccessType::AT_WRITE, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_ZP_PTR_LO, MicroOp::M_READ_ZP_PTR_HI, MicroOp::M_READ_DUMMY, MicroOp::M_WRITE_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_ZP_PTR_LO, MicroOp::M_READ_ZP_PTR_HI, MicroOp::M_READ_DUMMY, MicroOp::M_WRITE_EA } } },
        Definition{ "STA_IND", 0x92, MicroCycleProgram{ AccessType::AT_WRITE, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_ZP_PTR_LO, MicroOp::M_READ_ZP_PTR_HI, MicroOp::M_WRITE_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_ZP_PTR_LO, MicroOp::M_READ_ZP_PTR_HI, MicroOp::M_WRITE_EA } } },
        Definition{ "INC_ZPG", 0xE6, MicroCycleProgram{ AccessType::AT_RMW, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_EA, MicroOp::M_WRITE_EA_DUMMY, MicroOp::M_WRITE_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_EA, MicroOp::M_WRITE_EA_DUMMY, MicroOp::M_WRITE_EA } } },
        Definition{ "INC_ZPG_X", 0xF6, MicroCycleProgram{ AccessType::AT_RMW, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_DUMMY, MicroOp::M_READ_EA, MicroOp::M_WRITE_EA_DUMMY, MicroOp::M_WRITE_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_DUMMY, MicroOp::M_READ_EA, MicroOp::M_WRITE_EA_DUMMY, MicroOp::M_WRITE_EA } } },
        Definition{ "INC_ABS", 0xEE, MicroCycleProgram{ AccessType::AT_RMW, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_EA, MicroOp::M_WRITE_EA_DUMMY, MicroOp::M_WRITE_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_EA, MicroOp::M_WRITE_EA_DUMMY, MicroOp::M_WRITE_EA } } },
        Definition{ "INC_ABS_X", 0xFE, MicroCycleProgram{ AccessType::AT_RMW, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_DUMMY, MicroOp::M_READ_EA, MicroOp::M_WRITE_EA_DUMMY, MicroOp::M_WRITE_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_DUMMY, MicroOp::M_READ_EA, MicroOp::M_WRITE_EA_DUMMY, MicroOp::M_WRITE_EA } } },
        Definition{ "DEC_ZPG", 0xC6, MicroCycleProgram{ AccessType::AT_RMW, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_EA, MicroOp::M_WRITE_EA_DUMMY, MicroOp::M_WRITE_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_EA, MicroOp::M_WRITE_EA_DUMMY, MicroOp::M_WRITE_EA } } },
        Definition{ "DEC_ZPG_X", 0xD6, MicroCycleProgram{ AccessType::AT_RMW, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_DUMMY, MicroOp::M_READ_EA, MicroOp::M_WRITE_EA_DUMMY, MicroOp::M_WRITE_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_DUMMY, MicroOp::M_READ_EA, MicroOp::M_WRITE_EA_DUMMY, MicroOp::M_WRITE_EA } } },
        Definition{ "DEC_ABS", 0xCE, MicroCycleProgram{ AccessType::AT_RMW, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_EA, MicroOp::M_WRITE_EA_DUMMY, MicroOp::M_WRITE_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_EA, MicroOp::M_WRITE_EA_DUMMY, MicroOp::M_WRITE_EA } } },
        Definition{ "DEC_ABS_X", 0xDE, MicroCycleProgram{ AccessType::AT_RMW, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_DUMMY, MicroOp::M_READ_EA, MicroOp::M_WRITE_EA_DUMMY, MicroOp::M_WRITE_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_DUMMY, MicroOp::M_READ_EA, MicroOp::M_WRITE_EA_DUMMY, MicroOp::M_WRITE_EA } } },
        Definition{ "ASL_ACC", 0x0A, MicroCycleProgram{ AccessType::AT_NONE, { MicroOp::M_FETCH_OPCODE, MicroOp::M_INTERNAL }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_INTERNAL } } },
        Definition{ "ASL_ZPG", 0x06, MicroCycleProgram{ AccessType::AT_RMW, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_EA, MicroOp::M_WRITE_EA_DUMMY, MicroOp::M_WRITE_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_EA, MicroOp::M_WRITE_EA_DUMMY, MicroOp::M_WRITE_EA } } },
        Definition{ "ASL_ZPG_X", 0x16, MicroCycleProgram{ AccessType::AT_RMW, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_DUMMY, MicroOp::M_READ_EA, MicroOp::M_WRITE_EA_DUMMY, MicroOp::M_WRITE_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_DUMMY, MicroOp::M_READ_EA, MicroOp::M_WRITE_EA_DUMMY, MicroOp::M_WRITE_EA } } },
        Definition{ "ASL_ABS", 0x0E, MicroCycleProgram{ AccessType::AT_RMW, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_EA, MicroOp::M_WRITE_EA_DUMMY, MicroOp::M_WRITE_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_EA, MicroOp::M_WRITE_EA_DUMMY, MicroOp::M_WRITE_EA } } },
        Definition{ "ASL_ABS_X", 0x1E, MicroCycleProgram{ AccessType::AT_RMW, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_DUMMY, MicroOp::M_READ_EA, MicroOp::M_WRITE_EA_DUMMY, MicroOp::M_WRITE_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_DUMMY, MicroOp::M_READ_EA, MicroOp::M_WRITE_EA_DUMMY, MicroOp::M_WRITE_EA } } },
        Definition{ "LSR_ACC", 0x4A, MicroCycleProgram{ AccessType::AT_NONE, { MicroOp::M_FETCH_OPCODE, MicroOp::M_INTERNAL }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_INTERNAL } } },
        Definition{ "LSR_ZPG", 0x46, MicroCycleProgram{ AccessType::AT_RMW, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_EA, MicroOp::M_WRITE_EA_DUMMY, MicroOp::M_WRITE_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_EA, MicroOp::M_WRITE_EA_DUMMY, MicroOp::M_WRITE_EA } } },
        Definition{ "LSR_ZPG_X", 0x56, MicroCycleProgram{ AccessType::AT_RMW, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_DUMMY, MicroOp::M_READ_EA, MicroOp::M_WRITE_EA_DUMMY, MicroOp::M_WRITE_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_DUMMY, MicroOp::M_READ_EA, MicroOp::M_WRITE_EA_DUMMY, MicroOp::M_WRITE_EA } } },
        Definition{ "LSR_ABS", 0x4E, MicroCycleProgram{ AccessType::AT_RMW, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_EA, MicroOp::M_WRITE_EA_DUMMY, MicroOp::M_WRITE_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_EA, MicroOp::M_WRITE_EA_DUMMY, MicroOp::M_WRITE_EA } } },
        Definition{ "LSR_ABS_X", 0x5E, MicroCycleProgram{ AccessType::AT_RMW, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_DUMMY, MicroOp::M_READ_EA, MicroOp::M_WRITE_EA_DUMMY, MicroOp::M_WRITE_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_DUMMY, MicroOp::M_READ_EA, MicroOp::M_WRITE_EA_DUMMY, MicroOp::M_WRITE_EA } } },
        Definition{ "ROL_ACC", 0x2A, MicroCycleProgram{ AccessType::AT_NONE, { MicroOp::M_FETCH_OPCODE, MicroOp::M_INTERNAL }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_INTERNAL } } },
        Definition{ "ROL_ZPG", 0x26, MicroCycleProgram{ AccessType::AT_RMW, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_EA, MicroOp::M_WRITE_EA_DUMMY, MicroOp::M_WRITE_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_EA, MicroOp::M_WRITE_EA_DUMMY, MicroOp::M_WRITE_EA } } },
        Definition{ "ROL_ZPG_X", 0x36, MicroCycleProgram{ AccessType::AT_RMW, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_DUMMY, MicroOp::M_READ_EA, MicroOp::M_WRITE_EA_DUMMY, MicroOp::M_WRITE_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_DUMMY, MicroOp::M_READ_EA, MicroOp::M_WRITE_EA_DUMMY, MicroOp::M_WRITE_EA } } },
        Definition{ "ROL_ABS", 0x2E, MicroCycleProgram{ AccessType::AT_RMW, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_EA, MicroOp::M_WRITE_EA_DUMMY, MicroOp::M_WRITE_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_EA, MicroOp::M_WRITE_EA_DUMMY, MicroOp::M_WRITE_EA } } },
        Definition{ "ROL_ABS_X", 0x3E, MicroCycleProgram{ AccessType::AT_RMW, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_DUMMY, MicroOp::M_READ_EA, MicroOp::M_WRITE_EA_DUMMY, MicroOp::M_WRITE_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_DUMMY, MicroOp::M_READ_EA, MicroOp::M_WRITE_EA_DUMMY, MicroOp::M_WRITE_EA } } },
        Definition{ "ROR_ACC", 0x6A, MicroCycleProgram{ AccessType::AT_NONE, { MicroOp::M_FETCH_OPCODE, MicroOp::M_INTERNAL }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_INTERNAL } } },
        Definition{ "ROR_ZPG", 0x66, MicroCycleProgram{ AccessType::AT_RMW, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_EA, MicroOp::M_WRITE_EA_DUMMY, MicroOp::M_WRITE_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_EA, MicroOp::M_WRITE_EA_DUMMY, MicroOp::M_WRITE_EA } } },
        Definition{ "ROR_ZPG_X", 0x76, MicroCycleProgram{ AccessType::AT_RMW, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_DUMMY, MicroOp::M_READ_EA, MicroOp::M_WRITE_EA_DUMMY, MicroOp::M_WRITE_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_DUMMY, MicroOp::M_READ_EA, MicroOp::M_WRITE_EA_DUMMY, MicroOp::M_WRITE_EA } } },
        Definition{ "ROR_ABS", 0x6E, MicroCycleProgram{ AccessType::AT_RMW, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_EA, MicroOp::M_WRITE_EA_DUMMY, MicroOp::M_WRITE_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_EA, MicroOp::M_WRITE_EA_DUMMY, MicroOp::M_WRITE_EA } } },
        Definition{ "ROR_ABS_X", 0x7E, MicroCycleProgram{ AccessType::AT_RMW, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_DUMMY, MicroOp::M_READ_EA, MicroOp::M_WRITE_EA_DUMMY, MicroOp::M_WRITE_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_DUMMY, MicroOp::M_READ_EA, MicroOp::M_WRITE_EA_DUMMY, MicroOp::M_WRITE_EA } } },
        Definition{ "ORA_IMM", 0x09, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_READ_IMM_DATA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_READ_IMM_DATA } } },
        Definition{ "ORA_ZPG", 0x05, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_EA } } },
        Definition{ "ORA_ZPG_X", 0x15, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_DUMMY, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_DUMMY, MicroOp::M_READ_EA } } },
        Definition{ "ORA_ABS", 0x0D, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_EA } } },
        Definition{ "ORA_ABS_X", 0x1D, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_DUMMY, MicroOp::M_READ_EA } } },
        Definition{ "ORA_ABS_Y", 0x19, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_DUMMY, MicroOp::M_READ_EA } } },
        Definition{ "ORA_IND_X", 0x01, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_DUMMY, MicroOp::M_READ_ZP_PTR_LO, MicroOp::M_READ_ZP_PTR_HI, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_DUMMY, MicroOp::M_READ_ZP_PTR_LO, MicroOp::M_READ_ZP_PTR_HI, MicroOp::M_READ_EA } } },
        Definition{ "ORA_IND_Y", 0x11, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_ZP_PTR_LO, MicroOp::M_READ_ZP_PTR_HI, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_ZP_PTR_LO, MicroOp::M_READ_ZP_PTR_HI, MicroOp::M_READ_DUMMY, MicroOp::M_READ_EA } } },
        Definition{ "ORA_IND", 0x12, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_ZP_PTR_LO, MicroOp::M_READ_ZP_PTR_HI, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_ZP_PTR_LO, MicroOp::M_READ_ZP_PTR_HI, MicroOp::M_READ_EA } } },
        Definition{ "AND_IMM", 0x29, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_READ_IMM_DATA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_READ_IMM_DATA } } },
        Definition{ "AND_ZPG", 0x25, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_EA } } },
        Definition{ "AND_ZPG_X", 0x35, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_DUMMY, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_DUMMY, MicroOp::M_READ_EA } } },
        Definition{ "AND_ABS", 0x2D, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_EA } } },
        Definition{ "AND_ABS_X", 0x3D, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_DUMMY, MicroOp::M_READ_EA } } },
        Definition{ "AND_ABS_Y", 0x39, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_DUMMY, MicroOp::M_READ_EA } } },
        Definition{ "AND_IND_X", 0x21, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_DUMMY, MicroOp::M_READ_ZP_PTR_LO, MicroOp::M_READ_ZP_PTR_HI, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_DUMMY, MicroOp::M_READ_ZP_PTR_LO, MicroOp::M_READ_ZP_PTR_HI, MicroOp::M_READ_EA } } },
        Definition{ "AND_IND_Y", 0x31, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_ZP_PTR_LO, MicroOp::M_READ_ZP_PTR_HI, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_ZP_PTR_LO, MicroOp::M_READ_ZP_PTR_HI, MicroOp::M_READ_DUMMY, MicroOp::M_READ_EA } } },
        Definition{ "AND_IND", 0x32, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_ZP_PTR_LO, MicroOp::M_READ_ZP_PTR_HI, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_ZP_PTR_LO, MicroOp::M_READ_ZP_PTR_HI, MicroOp::M_READ_EA } } },
        Definition{ "EOR_IMM", 0x49, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_READ_IMM_DATA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_READ_IMM_DATA } } },
        Definition{ "EOR_ZPG", 0x45, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_EA } } },
        Definition{ "EOR_ZPG_X", 0x55, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_DUMMY, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_DUMMY, MicroOp::M_READ_EA } } },
        Definition{ "EOR_ABS", 0x4D, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_EA } } },
        Definition{ "EOR_ABS_X", 0x5D, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_DUMMY, MicroOp::M_READ_EA } } },
        Definition{ "EOR_ABS_Y", 0x59, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_DUMMY, MicroOp::M_READ_EA } } },
        Definition{ "EOR_IND_X", 0x41, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_DUMMY, MicroOp::M_READ_ZP_PTR_LO, MicroOp::M_READ_ZP_PTR_HI, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_DUMMY, MicroOp::M_READ_ZP_PTR_LO, MicroOp::M_READ_ZP_PTR_HI, MicroOp::M_READ_EA } } },
        Definition{ "EOR_IND_Y", 0x51, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_ZP_PTR_LO, MicroOp::M_READ_ZP_PTR_HI, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_ZP_PTR_LO, MicroOp::M_READ_ZP_PTR_HI, MicroOp::M_READ_DUMMY, MicroOp::M_READ_EA } } },
        Definition{ "EOR_IND", 0x52, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_ZP_PTR_LO, MicroOp::M_READ_ZP_PTR_HI, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_ZP_PTR_LO, MicroOp::M_READ_ZP_PTR_HI, MicroOp::M_READ_EA } } },
        Definition{ "ADC_IMM", 0x69, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_READ_IMM_DATA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_READ_IMM_DATA } } },
        Definition{ "ADC_ZPG", 0x65, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_EA } } },
        Definition{ "ADC_ZPG_X", 0x75, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_DUMMY, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_DUMMY, MicroOp::M_READ_EA } } },
        Definition{ "ADC_ABS", 0x6D, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_EA } } },
        Definition{ "ADC_ABS_X", 0x7D, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_DUMMY, MicroOp::M_READ_EA } } },
        Definition{ "ADC_ABS_Y", 0x79, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_DUMMY, MicroOp::M_READ_EA } } },
        Definition{ "ADC_IND_X", 0x61, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_DUMMY, MicroOp::M_READ_ZP_PTR_LO, MicroOp::M_READ_ZP_PTR_HI, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_DUMMY, MicroOp::M_READ_ZP_PTR_LO, MicroOp::M_READ_ZP_PTR_HI, MicroOp::M_READ_EA } } },
        Definition{ "ADC_IND_Y", 0x71, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_ZP_PTR_LO, MicroOp::M_READ_ZP_PTR_HI, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_ZP_PTR_LO, MicroOp::M_READ_ZP_PTR_HI, MicroOp::M_READ_DUMMY, MicroOp::M_READ_EA } } },
        Definition{ "ADC_IND", 0x72, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_ZP_PTR_LO, MicroOp::M_READ_ZP_PTR_HI, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_ZP_PTR_LO, MicroOp::M_READ_ZP_PTR_HI, MicroOp::M_READ_EA } } },
        Definition{ "SBC_IMM", 0xE9, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_READ_IMM_DATA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_READ_IMM_DATA } } },
        Definition{ "SBC_ZPG", 0xE5, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_EA } } },
        Definition{ "SBC_ZPG_X", 0xF5, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_DUMMY, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_DUMMY, MicroOp::M_READ_EA } } },
        Definition{ "SBC_ABS", 0xED, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_EA } } },
        Definition{ "SBC_ABS_X", 0xFD, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_DUMMY, MicroOp::M_READ_EA } } },
        Definition{ "SBC_ABS_Y", 0xF9, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_DUMMY, MicroOp::M_READ_EA } } },
        Definition{ "SBC_IND_X", 0xE1, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_DUMMY, MicroOp::M_READ_ZP_PTR_LO, MicroOp::M_READ_ZP_PTR_HI, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_DUMMY, MicroOp::M_READ_ZP_PTR_LO, MicroOp::M_READ_ZP_PTR_HI, MicroOp::M_READ_EA } } },
        Definition{ "SBC_IND_Y", 0xF1, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_ZP_PTR_LO, MicroOp::M_READ_ZP_PTR_HI, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_ZP_PTR_LO, MicroOp::M_READ_ZP_PTR_HI, MicroOp::M_READ_DUMMY, MicroOp::M_READ_EA } } },
        Definition{ "SBC_IND", 0xF2, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_ZP_PTR_LO, MicroOp::M_READ_ZP_PTR_HI, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_ZP_PTR_LO, MicroOp::M_READ_ZP_PTR_HI, MicroOp::M_READ_EA } } },
        Definition{ "CMP_IMM", 0xC9, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_READ_IMM_DATA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_READ_IMM_DATA } } },
        Definition{ "CMP_ZPG", 0xC5, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_EA } } },
        Definition{ "CMP_ZPG_X", 0xD5, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_DUMMY, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_DUMMY, MicroOp::M_READ_EA } } },
        Definition{ "CMP_ABS", 0xCD, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_EA } } },
        Definition{ "CMP_ABS_X", 0xDD, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_DUMMY, MicroOp::M_READ_EA } } },
        Definition{ "CMP_ABS_Y", 0xD9, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_DUMMY, MicroOp::M_READ_EA } } },
        Definition{ "CMP_IND_X", 0xC1, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_DUMMY, MicroOp::M_READ_ZP_PTR_LO, MicroOp::M_READ_ZP_PTR_HI, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_DUMMY, MicroOp::M_READ_ZP_PTR_LO, MicroOp::M_READ_ZP_PTR_HI, MicroOp::M_READ_EA } } },
        Definition{ "CMP_IND_Y", 0xD1, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_ZP_PTR_LO, MicroOp::M_READ_ZP_PTR_HI, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_ZP_PTR_LO, MicroOp::M_READ_ZP_PTR_HI, MicroOp::M_READ_DUMMY, MicroOp::M_READ_EA } } },
        Definition{ "CMP_IND", 0xD2, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_ZP_PTR_LO, MicroOp::M_READ_ZP_PTR_HI, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_ZP_PTR_LO, MicroOp::M_READ_ZP_PTR_HI, MicroOp::M_READ_EA } } },
        Definition{ "BIT_IMM", 0x89, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_READ_IMM_DATA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_READ_IMM_DATA } } },
        Definition{ "BIT_ZPG", 0x24, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_EA } } },
        Definition{ "BIT_ZPG_X", 0x34, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_DUMMY, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_DUMMY, MicroOp::M_READ_EA } } },
        Definition{ "BIT_ABS", 0x2C, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_EA } } },
        Definition{ "BIT_ABS_X", 0x3C, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_EA } } },
        Definition{ "LDX_IMM", 0xA2, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_READ_IMM_DATA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_READ_IMM_DATA } } },
        Definition{ "LDX_ZPG", 0xA6, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_EA } } },
        Definition{ "LDX_ZPG_Y", 0xB6, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_DUMMY, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_DUMMY, MicroOp::M_READ_EA } } },
        Definition{ "LDX_ABS", 0xAE, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_EA } } },
        Definition{ "LDX_ABS_Y", 0xBE, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_DUMMY, MicroOp::M_READ_EA } } },
        Definition{ "LDY_IMM", 0xA0, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_READ_IMM_DATA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_READ_IMM_DATA } } },
        Definition{ "LDY_ZPG", 0xA4, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_EA } } },
        Definition{ "LDY_ZPG_X", 0xB4, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_DUMMY, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_DUMMY, MicroOp::M_READ_EA } } },
        Definition{ "LDY_ABS", 0xAC, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_EA } } },
        Definition{ "LDY_ABS_X", 0xBC, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_DUMMY, MicroOp::M_READ_EA } } },
        Definition{ "STX_ZPG", 0x86, MicroCycleProgram{ AccessType::AT_WRITE, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_WRITE_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_WRITE_EA } } },
        Definition{ "STX_ZPG_Y", 0x96, MicroCycleProgram{ AccessType::AT_WRITE, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_DUMMY, MicroOp::M_WRITE_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_DUMMY, MicroOp::M_WRITE_EA } } },
        Definition{ "STX_ABS", 0x8E, MicroCycleProgram{ AccessType::AT_WRITE, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_WRITE_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_WRITE_EA } } },
        Definition{ "STY_ZPG", 0x84, MicroCycleProgram{ AccessType::AT_WRITE, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_WRITE_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_WRITE_EA } } },
        Definition{ "STY_ZPG_X", 0x94, MicroCycleProgram{ AccessType::AT_WRITE, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_DUMMY, MicroOp::M_WRITE_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_DUMMY, MicroOp::M_WRITE_EA } } },
        Definition{ "STY_ABS", 0x8C, MicroCycleProgram{ AccessType::AT_WRITE, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_WRITE_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_WRITE_EA } } },
        Definition{ "CPX_IMM", 0xE0, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_READ_IMM_DATA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_READ_IMM_DATA } } },
        Definition{ "CPX_ZPG", 0xE4, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_EA } } },
        Definition{ "CPX_ABS", 0xEC, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_EA } } },
        Definition{ "CPY_IMM", 0xC0, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_READ_IMM_DATA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_READ_IMM_DATA } } },
        Definition{ "CPY_ZPG", 0xC4, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_EA } } },
        Definition{ "CPY_ABS", 0xCC, MicroCycleProgram{ AccessType::AT_READ, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_EA } } },
        Definition{ "JSR_ABS", 0x20, MicroCycleProgram{ AccessType::AT_NONE, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_INTERNAL, MicroOp::M_INTERNAL, MicroOp::M_INTERNAL, MicroOp::M_FETCH_OPERAND_HI }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_INTERNAL, MicroOp::M_INTERNAL, MicroOp::M_INTERNAL, MicroOp::M_FETCH_OPERAND_HI } } },
        Definition{ "BRK_IMP", 0x00, MicroCycleProgram{ AccessType::AT_NONE, { MicroOp::M_FETCH_OPCODE, MicroOp::M_INTERNAL, MicroOp::M_INTERNAL, MicroOp::M_INTERNAL, MicroOp::M_INTERNAL, MicroOp::M_INTERNAL, MicroOp::M_INTERNAL }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_INTERNAL, MicroOp::M_INTERNAL, MicroOp::M_INTERNAL, MicroOp::M_INTERNAL, MicroOp::M_INTERNAL, MicroOp::M_INTERNAL } } },
        Definition{ "TSB_ZPG", 0x04, MicroCycleProgram{ AccessType::AT_RMW, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_EA, MicroOp::M_WRITE_EA_DUMMY, MicroOp::M_WRITE_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_EA, MicroOp::M_WRITE_EA_DUMMY, MicroOp::M_WRITE_EA } } },
        Definition{ "PHP_IMP", 0x08, MicroCycleProgram{ AccessType::AT_NONE, { MicroOp::M_FETCH_OPCODE, MicroOp::M_INTERNAL, MicroOp::M_INTERNAL }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_INTERNAL, MicroOp::M_INTERNAL } } },
        Definition{ "TSB_ABS", 0x0C, MicroCycleProgram{ AccessType::AT_RMW, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_EA, MicroOp::M_WRITE_EA_DUMMY, MicroOp::M_WRITE_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_EA, MicroOp::M_WRITE_EA_DUMMY, MicroOp::M_WRITE_EA } } },
        Definition{ "BPL_REL", 0x10, MicroCycleProgram{ AccessType::AT_NONE, { MicroOp::M_FETCH_OPCODE, MicroOp::M_READ_IMM_DATA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_READ_IMM_DATA, MicroOp::M_INTERNAL } } },
        Definition{ "TRB_ZPG", 0x14, MicroCycleProgram{ AccessType::AT_RMW, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_EA, MicroOp::M_WRITE_EA_DUMMY, MicroOp::M_WRITE_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_EA, MicroOp::M_WRITE_EA_DUMMY, MicroOp::M_WRITE_EA } } },
        Definition{ "CLC_IMP", 0x18, MicroCycleProgram{ AccessType::AT_NONE, { MicroOp::M_FETCH_OPCODE, MicroOp::M_INTERNAL }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_INTERNAL } } },
        Definition{ "INA_ACC", 0x1A, MicroCycleProgram{ AccessType::AT_NONE, { MicroOp::M_FETCH_OPCODE, MicroOp::M_INTERNAL }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_INTERNAL } } },
        Definition{ "TRB_ABS", 0x1C, MicroCycleProgram{ AccessType::AT_RMW, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_EA, MicroOp::M_WRITE_EA_DUMMY, MicroOp::M_WRITE_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_EA, MicroOp::M_WRITE_EA_DUMMY, MicroOp::M_WRITE_EA } } },
        Definition{ "PLP_IMP", 0x28, MicroCycleProgram{ AccessType::AT_NONE, { MicroOp::M_FETCH_OPCODE, MicroOp::M_INTERNAL, MicroOp::M_INTERNAL, MicroOp::M_INTERNAL }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_INTERNAL, MicroOp::M_INTERNAL, MicroOp::M_INTERNAL } } },
        Definition{ "BMI_REL", 0x30, MicroCycleProgram{ AccessType::AT_NONE, { MicroOp::M_FETCH_OPCODE, MicroOp::M_READ_IMM_DATA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_READ_IMM_DATA, MicroOp::M_INTERNAL } } },
        Definition{ "SEC_IMP", 0x38, MicroCycleProgram{ AccessType::AT_NONE, { MicroOp::M_FETCH_OPCODE, MicroOp::M_INTERNAL }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_INTERNAL } } },
        Definition{ "DEA_ACC", 0x3A, MicroCycleProgram{ AccessType::AT_NONE, { MicroOp::M_FETCH_OPCODE, MicroOp::M_INTERNAL }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_INTERNAL } } },
        Definition{ "RTI_IMP", 0x40, MicroCycleProgram{ AccessType::AT_NONE, { MicroOp::M_FETCH_OPCODE, MicroOp::M_INTERNAL, MicroOp::M_INTERNAL, MicroOp::M_INTERNAL, MicroOp::M_INTERNAL, MicroOp::M_INTERNAL }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_INTERNAL, MicroOp::M_INTERNAL, MicroOp::M_INTERNAL, MicroOp::M_INTERNAL, MicroOp::M_INTERNAL } } },
        Definition{ "PHA_IMP", 0x48, MicroCycleProgram{ AccessType::AT_NONE, { MicroOp::M_FETCH_OPCODE, MicroOp::M_INTERNAL, MicroOp::M_INTERNAL }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_INTERNAL, MicroOp::M_INTERNAL } } },
        Definition{ "JMP_ABS", 0x4C, MicroCycleProgram{ AccessType::AT_NONE, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI } } },
        Definition{ "BVC_REL", 0x50, MicroCycleProgram{ AccessType::AT_NONE, { MicroOp::M_FETCH_OPCODE, MicroOp::M_READ_IMM_DATA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_READ_IMM_DATA, MicroOp::M_INTERNAL } } },
        Definition{ "CLI_IMP", 0x58, MicroCycleProgram{ AccessType::AT_NONE, { MicroOp::M_FETCH_OPCODE, MicroOp::M_INTERNAL }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_INTERNAL } } },
        Definition{ "PHY_IMP", 0x5A, MicroCycleProgram{ AccessType::AT_NONE, { MicroOp::M_FETCH_OPCODE, MicroOp::M_INTERNAL, MicroOp::M_INTERNAL }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_INTERNAL, MicroOp::M_INTERNAL } } },
        Definition{ "RTS_IMP", 0x60, MicroCycleProgram{ AccessType::AT_NONE, { MicroOp::M_FETCH_OPCODE, MicroOp::M_INTERNAL, MicroOp::M_INTERNAL, MicroOp::M_INTERNAL, MicroOp::M_INTERNAL, MicroOp::M_INTERNAL }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_INTERNAL, MicroOp::M_INTERNAL, MicroOp::M_INTERNAL, MicroOp::M_INTERNAL, MicroOp::M_INTERNAL } } },
        Definition{ "STZ_ZPG", 0x64, MicroCycleProgram{ AccessType::AT_WRITE, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_WRITE_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_WRITE_EA } } },
        Definition{ "PLA_IMP", 0x68, MicroCycleProgram{ AccessType::AT_NONE, { MicroOp::M_FETCH_OPCODE, MicroOp::M_INTERNAL, MicroOp::M_INTERNAL, MicroOp::M_INTERNAL }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_INTERNAL, MicroOp::M_INTERNAL, MicroOp::M_INTERNAL } } },
        Definition{ "JMP_ABS_IND", 0x6C, MicroCycleProgram{ AccessType::AT_NONE, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_INTERNAL, MicroOp::M_INTERNAL, MicroOp::M_INTERNAL }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_INTERNAL, MicroOp::M_INTERNAL, MicroOp::M_INTERNAL } } },
        Definition{ "BVS_REL", 0x70, MicroCycleProgram{ AccessType::AT_NONE, { MicroOp::M_FETCH_OPCODE, MicroOp::M_READ_IMM_DATA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_READ_IMM_DATA, MicroOp::M_INTERNAL } } },
        Definition{ "STZ_ZPG_X", 0x74, MicroCycleProgram{ AccessType::AT_WRITE, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_DUMMY, MicroOp::M_WRITE_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_READ_DUMMY, MicroOp::M_WRITE_EA } } },
        Definition{ "SEI_IMP", 0x78, MicroCycleProgram{ AccessType::AT_NONE, { MicroOp::M_FETCH_OPCODE, MicroOp::M_INTERNAL }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_INTERNAL } } },
        Definition{ "PLY_IMP", 0x7A, MicroCycleProgram{ AccessType::AT_NONE, { MicroOp::M_FETCH_OPCODE, MicroOp::M_INTERNAL, MicroOp::M_INTERNAL, MicroOp::M_INTERNAL }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_INTERNAL, MicroOp::M_INTERNAL, MicroOp::M_INTERNAL } } },
        Definition{ "JMP_ABS_IND_X", 0x7C, MicroCycleProgram{ AccessType::AT_NONE, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_INTERNAL, MicroOp::M_INTERNAL, MicroOp::M_INTERNAL }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_INTERNAL, MicroOp::M_INTERNAL, MicroOp::M_INTERNAL } } },
        Definition{ "BRA_REL", 0x80, MicroCycleProgram{ AccessType::AT_NONE, { MicroOp::M_FETCH_OPCODE, MicroOp::M_READ_IMM_DATA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_READ_IMM_DATA, MicroOp::M_INTERNAL } } },
        Definition{ "DEY_IMP", 0x88, MicroCycleProgram{ AccessType::AT_NONE, { MicroOp::M_FETCH_OPCODE, MicroOp::M_INTERNAL }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_INTERNAL } } },
        Definition{ "TXA_IMP", 0x8A, MicroCycleProgram{ AccessType::AT_NONE, { MicroOp::M_FETCH_OPCODE, MicroOp::M_INTERNAL }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_INTERNAL } } },
        Definition{ "BCC_REL", 0x90, MicroCycleProgram{ AccessType::AT_NONE, { MicroOp::M_FETCH_OPCODE, MicroOp::M_READ_IMM_DATA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_READ_IMM_DATA, MicroOp::M_INTERNAL } } },
        Definition{ "TYA_IMP", 0x98, MicroCycleProgram{ AccessType::AT_NONE, { MicroOp::M_FETCH_OPCODE, MicroOp::M_INTERNAL }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_INTERNAL } } },
        Definition{ "TXS_IMP", 0x9A, MicroCycleProgram{ AccessType::AT_NONE, { MicroOp::M_FETCH_OPCODE, MicroOp::M_INTERNAL }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_INTERNAL } } },
        Definition{ "STZ_ABS", 0x9C, MicroCycleProgram{ AccessType::AT_WRITE, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_WRITE_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_WRITE_EA } } },
        Definition{ "STZ_ABS_X", 0x9E, MicroCycleProgram{ AccessType::AT_WRITE, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_DUMMY, MicroOp::M_WRITE_EA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_FETCH_OPERAND_LO, MicroOp::M_FETCH_OPERAND_HI, MicroOp::M_READ_DUMMY, MicroOp::M_WRITE_EA } } },
        Definition{ "TAY_IMP", 0xA8, MicroCycleProgram{ AccessType::AT_NONE, { MicroOp::M_FETCH_OPCODE, MicroOp::M_INTERNAL }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_INTERNAL } } },
        Definition{ "TAX_IMP", 0xAA, MicroCycleProgram{ AccessType::AT_NONE, { MicroOp::M_FETCH_OPCODE, MicroOp::M_INTERNAL }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_INTERNAL } } },
        Definition{ "BCS_REL", 0xB0, MicroCycleProgram{ AccessType::AT_NONE, { MicroOp::M_FETCH_OPCODE, MicroOp::M_READ_IMM_DATA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_READ_IMM_DATA, MicroOp::M_INTERNAL } } },
        Definition{ "CLV_IMP", 0xB8, MicroCycleProgram{ AccessType::AT_NONE, { MicroOp::M_FETCH_OPCODE, MicroOp::M_INTERNAL }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_INTERNAL } } },
        Definition{ "TSX_IMP", 0xBA, MicroCycleProgram{ AccessType::AT_NONE, { MicroOp::M_FETCH_OPCODE, MicroOp::M_INTERNAL }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_INTERNAL } } },
        Definition{ "INY_IMP", 0xC8, MicroCycleProgram{ AccessType::AT_NONE, { MicroOp::M_FETCH_OPCODE, MicroOp::M_INTERNAL }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_INTERNAL } } },
        Definition{ "DEX_IMP", 0xCA, MicroCycleProgram{ AccessType::AT_NONE, { MicroOp::M_FETCH_OPCODE, MicroOp::M_INTERNAL }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_INTERNAL } } },
        Definition{ "BNE_REL", 0xD0, MicroCycleProgram{ AccessType::AT_NONE, { MicroOp::M_FETCH_OPCODE, MicroOp::M_READ_IMM_DATA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_READ_IMM_DATA, MicroOp::M_INTERNAL } } },
        Definition{ "CLD_IMP", 0xD8, MicroCycleProgram{ AccessType::AT_NONE, { MicroOp::M_FETCH_OPCODE, MicroOp::M_INTERNAL }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_INTERNAL } } },
        Definition{ "PHX_IMP", 0xDA, MicroCycleProgram{ AccessType::AT_NONE, { MicroOp::M_FETCH_OPCODE, MicroOp::M_INTERNAL, MicroOp::M_INTERNAL }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_INTERNAL, MicroOp::M_INTERNAL } } },
        Definition{ "INX_IMP", 0xE8, MicroCycleProgram{ AccessType::AT_NONE, { MicroOp::M_FETCH_OPCODE, MicroOp::M_INTERNAL }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_INTERNAL } } },
        Definition{ "BEQ_REL", 0xF0, MicroCycleProgram{ AccessType::AT_NONE, { MicroOp::M_FETCH_OPCODE, MicroOp::M_READ_IMM_DATA }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_READ_IMM_DATA, MicroOp::M_INTERNAL } } },
        Definition{ "SED_IMP", 0xF8, MicroCycleProgram{ AccessType::AT_NONE, { MicroOp::M_FETCH_OPCODE, MicroOp::M_INTERNAL }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_INTERNAL } } },
        Definition{ "PLX_IMP", 0xFA, MicroCycleProgram{ AccessType::AT_NONE, { MicroOp::M_FETCH_OPCODE, MicroOp::M_INTERNAL, MicroOp::M_INTERNAL, MicroOp::M_INTERNAL }, { MicroOp::M_FETCH_OPCODE, MicroOp::M_INTERNAL, MicroOp::M_INTERNAL, MicroOp::M_INTERNAL } } },
    }
};

static const std::array<const Definition*, 256> kByteToDef = []() {
    std::array<const Definition*, 256> map{};
    for( size_t i = 0; i<kDefinitions.size(); ++i ) {
        const Definition& d = kDefinitions[i];
        map[d.opcodeByte] = &d;
    }
    return map;
}();

static const std::vector<Cpu65c02Opcode::Id> kLdaFamily = {
    Cpu65c02Opcode::Id::LDA_IMM,
    Cpu65c02Opcode::Id::LDA_ZPG,
    Cpu65c02Opcode::Id::LDA_ZPG_X,
    Cpu65c02Opcode::Id::LDA_ABS,
    Cpu65c02Opcode::Id::LDA_ABS_X,
    Cpu65c02Opcode::Id::LDA_ABS_Y,
    Cpu65c02Opcode::Id::LDA_IND_X,
    Cpu65c02Opcode::Id::LDA_IND_Y,
    Cpu65c02Opcode::Id::LDA_IND,
};

static const std::vector<Cpu65c02Opcode::Id> kStaFamily = {
    Cpu65c02Opcode::Id::STA_ZPG,
    Cpu65c02Opcode::Id::STA_ZPG_X,
    Cpu65c02Opcode::Id::STA_ABS,
    Cpu65c02Opcode::Id::STA_ABS_X,
    Cpu65c02Opcode::Id::STA_ABS_Y,
    Cpu65c02Opcode::Id::STA_IND_X,
    Cpu65c02Opcode::Id::STA_IND_Y,
    Cpu65c02Opcode::Id::STA_IND,
};

static const std::vector<Cpu65c02Opcode::Id> kIncFamily = {
    Cpu65c02Opcode::Id::INC_ZPG,
    Cpu65c02Opcode::Id::INC_ZPG_X,
    Cpu65c02Opcode::Id::INC_ABS,
    Cpu65c02Opcode::Id::INC_ABS_X,
};

static const std::vector<Cpu65c02Opcode::Id> kDecFamily = {
    Cpu65c02Opcode::Id::DEC_ZPG,
    Cpu65c02Opcode::Id::DEC_ZPG_X,
    Cpu65c02Opcode::Id::DEC_ABS,
    Cpu65c02Opcode::Id::DEC_ABS_X,
};

static const std::vector<Cpu65c02Opcode::Id> kAslFamily = {
    Cpu65c02Opcode::Id::ASL_ACC,
    Cpu65c02Opcode::Id::ASL_ZPG,
    Cpu65c02Opcode::Id::ASL_ZPG_X,
    Cpu65c02Opcode::Id::ASL_ABS,
    Cpu65c02Opcode::Id::ASL_ABS_X,
};

static const std::vector<Cpu65c02Opcode::Id> kLsrFamily = {
    Cpu65c02Opcode::Id::LSR_ACC,
    Cpu65c02Opcode::Id::LSR_ZPG,
    Cpu65c02Opcode::Id::LSR_ZPG_X,
    Cpu65c02Opcode::Id::LSR_ABS,
    Cpu65c02Opcode::Id::LSR_ABS_X,
};

static const std::vector<Cpu65c02Opcode::Id> kRolFamily = {
    Cpu65c02Opcode::Id::ROL_ACC,
    Cpu65c02Opcode::Id::ROL_ZPG,
    Cpu65c02Opcode::Id::ROL_ZPG_X,
    Cpu65c02Opcode::Id::ROL_ABS,
    Cpu65c02Opcode::Id::ROL_ABS_X,
};

static const std::vector<Cpu65c02Opcode::Id> kRorFamily = {
    Cpu65c02Opcode::Id::ROR_ACC,
    Cpu65c02Opcode::Id::ROR_ZPG,
    Cpu65c02Opcode::Id::ROR_ZPG_X,
    Cpu65c02Opcode::Id::ROR_ABS,
    Cpu65c02Opcode::Id::ROR_ABS_X,
};

static const std::vector<Cpu65c02Opcode::Id> kOraFamily = {
    Cpu65c02Opcode::Id::ORA_IMM,
    Cpu65c02Opcode::Id::ORA_ZPG,
    Cpu65c02Opcode::Id::ORA_ZPG_X,
    Cpu65c02Opcode::Id::ORA_ABS,
    Cpu65c02Opcode::Id::ORA_ABS_X,
    Cpu65c02Opcode::Id::ORA_ABS_Y,
    Cpu65c02Opcode::Id::ORA_IND_X,
    Cpu65c02Opcode::Id::ORA_IND_Y,
    Cpu65c02Opcode::Id::ORA_IND,
};

static const std::vector<Cpu65c02Opcode::Id> kAndFamily = {
    Cpu65c02Opcode::Id::AND_IMM,
    Cpu65c02Opcode::Id::AND_ZPG,
    Cpu65c02Opcode::Id::AND_ZPG_X,
    Cpu65c02Opcode::Id::AND_ABS,
    Cpu65c02Opcode::Id::AND_ABS_X,
    Cpu65c02Opcode::Id::AND_ABS_Y,
    Cpu65c02Opcode::Id::AND_IND_X,
    Cpu65c02Opcode::Id::AND_IND_Y,
    Cpu65c02Opcode::Id::AND_IND,
};

static const std::vector<Cpu65c02Opcode::Id> kEorFamily = {
    Cpu65c02Opcode::Id::EOR_IMM,
    Cpu65c02Opcode::Id::EOR_ZPG,
    Cpu65c02Opcode::Id::EOR_ZPG_X,
    Cpu65c02Opcode::Id::EOR_ABS,
    Cpu65c02Opcode::Id::EOR_ABS_X,
    Cpu65c02Opcode::Id::EOR_ABS_Y,
    Cpu65c02Opcode::Id::EOR_IND_X,
    Cpu65c02Opcode::Id::EOR_IND_Y,
    Cpu65c02Opcode::Id::EOR_IND,
};

static const std::vector<Cpu65c02Opcode::Id> kAdcFamily = {
    Cpu65c02Opcode::Id::ADC_IMM,
    Cpu65c02Opcode::Id::ADC_ZPG,
    Cpu65c02Opcode::Id::ADC_ZPG_X,
    Cpu65c02Opcode::Id::ADC_ABS,
    Cpu65c02Opcode::Id::ADC_ABS_X,
    Cpu65c02Opcode::Id::ADC_ABS_Y,
    Cpu65c02Opcode::Id::ADC_IND_X,
    Cpu65c02Opcode::Id::ADC_IND_Y,
    Cpu65c02Opcode::Id::ADC_IND,
};

static const std::vector<Cpu65c02Opcode::Id> kSbcFamily = {
    Cpu65c02Opcode::Id::SBC_IMM,
    Cpu65c02Opcode::Id::SBC_ZPG,
    Cpu65c02Opcode::Id::SBC_ZPG_X,
    Cpu65c02Opcode::Id::SBC_ABS,
    Cpu65c02Opcode::Id::SBC_ABS_X,
    Cpu65c02Opcode::Id::SBC_ABS_Y,
    Cpu65c02Opcode::Id::SBC_IND_X,
    Cpu65c02Opcode::Id::SBC_IND_Y,
    Cpu65c02Opcode::Id::SBC_IND,
};

static const std::vector<Cpu65c02Opcode::Id> kCmpFamily = {
    Cpu65c02Opcode::Id::CMP_IMM,
    Cpu65c02Opcode::Id::CMP_ZPG,
    Cpu65c02Opcode::Id::CMP_ZPG_X,
    Cpu65c02Opcode::Id::CMP_ABS,
    Cpu65c02Opcode::Id::CMP_ABS_X,
    Cpu65c02Opcode::Id::CMP_ABS_Y,
    Cpu65c02Opcode::Id::CMP_IND_X,
    Cpu65c02Opcode::Id::CMP_IND_Y,
    Cpu65c02Opcode::Id::CMP_IND,
};

static const std::vector<Cpu65c02Opcode::Id> kBitFamily = {
    Cpu65c02Opcode::Id::BIT_IMM,
    Cpu65c02Opcode::Id::BIT_ZPG,
    Cpu65c02Opcode::Id::BIT_ZPG_X,
    Cpu65c02Opcode::Id::BIT_ABS,
    Cpu65c02Opcode::Id::BIT_ABS_X,
};

static const std::vector<Cpu65c02Opcode::Id> kLdxFamily = {
    Cpu65c02Opcode::Id::LDX_IMM,
    Cpu65c02Opcode::Id::LDX_ZPG,
    Cpu65c02Opcode::Id::LDX_ZPG_Y,
    Cpu65c02Opcode::Id::LDX_ABS,
    Cpu65c02Opcode::Id::LDX_ABS_Y,
};

static const std::vector<Cpu65c02Opcode::Id> kLdyFamily = {
    Cpu65c02Opcode::Id::LDY_IMM,
    Cpu65c02Opcode::Id::LDY_ZPG,
    Cpu65c02Opcode::Id::LDY_ZPG_X,
    Cpu65c02Opcode::Id::LDY_ABS,
    Cpu65c02Opcode::Id::LDY_ABS_X,
};

static const std::vector<Cpu65c02Opcode::Id> kStxFamily = {
    Cpu65c02Opcode::Id::STX_ZPG,
    Cpu65c02Opcode::Id::STX_ZPG_Y,
    Cpu65c02Opcode::Id::STX_ABS,
};

static const std::vector<Cpu65c02Opcode::Id> kStyFamily = {
    Cpu65c02Opcode::Id::STY_ZPG,
    Cpu65c02Opcode::Id::STY_ZPG_X,
    Cpu65c02Opcode::Id::STY_ABS,
};

static const std::vector<Cpu65c02Opcode::Id> kCpxFamily = {
    Cpu65c02Opcode::Id::CPX_IMM,
    Cpu65c02Opcode::Id::CPX_ZPG,
    Cpu65c02Opcode::Id::CPX_ABS,
};

static const std::vector<Cpu65c02Opcode::Id> kCpyFamily = {
    Cpu65c02Opcode::Id::CPY_IMM,
    Cpu65c02Opcode::Id::CPY_ZPG,
    Cpu65c02Opcode::Id::CPY_ABS,
};

static const std::vector<Cpu65c02Opcode::Id> kJumpFamily = {
    Cpu65c02Opcode::Id::JMP_ABS,
    Cpu65c02Opcode::Id::JMP_ABS_IND,
    Cpu65c02Opcode::Id::JMP_ABS_IND_X,
    Cpu65c02Opcode::Id::JSR_ABS,
};

static const std::vector<Cpu65c02Opcode::Id> kBitTestSetFamily = {
    Cpu65c02Opcode::Id::BIT_IMM,
    Cpu65c02Opcode::Id::BIT_ZPG,
    Cpu65c02Opcode::Id::BIT_ZPG_X,
    Cpu65c02Opcode::Id::BIT_ABS,
    Cpu65c02Opcode::Id::BIT_ABS_X,
    Cpu65c02Opcode::Id::TSB_ZPG,
    Cpu65c02Opcode::Id::TSB_ABS,
    Cpu65c02Opcode::Id::TRB_ZPG,
    Cpu65c02Opcode::Id::TRB_ABS,
};

static const std::vector<Cpu65c02Opcode::Id> kTransferFamily = {
    Cpu65c02Opcode::Id::TAX_IMP,
    Cpu65c02Opcode::Id::TAY_IMP,
    Cpu65c02Opcode::Id::TXA_IMP,
    Cpu65c02Opcode::Id::TYA_IMP,
    Cpu65c02Opcode::Id::TSX_IMP,
    Cpu65c02Opcode::Id::TXS_IMP,
};

static const std::vector<Cpu65c02Opcode::Id> kIndexIncdecFamily = {
    Cpu65c02Opcode::Id::INX_IMP,
    Cpu65c02Opcode::Id::INY_IMP,
    Cpu65c02Opcode::Id::DEX_IMP,
    Cpu65c02Opcode::Id::DEY_IMP,
    Cpu65c02Opcode::Id::INA_ACC,
    Cpu65c02Opcode::Id::DEA_ACC,
};

static const std::vector<Cpu65c02Opcode::Id> kStzFamily = {
    Cpu65c02Opcode::Id::STZ_ZPG,
    Cpu65c02Opcode::Id::STZ_ZPG_X,
    Cpu65c02Opcode::Id::STZ_ABS,
    Cpu65c02Opcode::Id::STZ_ABS_X,
};

static const std::vector<Cpu65c02Opcode::Id> kInterruptControlFlowFamily = {
    Cpu65c02Opcode::Id::BRK_IMP,
    Cpu65c02Opcode::Id::RTI_IMP,
    Cpu65c02Opcode::Id::RTS_IMP,
    Cpu65c02Opcode::Id::JSR_ABS,
    Cpu65c02Opcode::Id::JMP_ABS,
    Cpu65c02Opcode::Id::JMP_ABS_IND,
    Cpu65c02Opcode::Id::JMP_ABS_IND_X,
    Cpu65c02Opcode::Id::BPL_REL,
    Cpu65c02Opcode::Id::BMI_REL,
    Cpu65c02Opcode::Id::BVC_REL,
    Cpu65c02Opcode::Id::BVS_REL,
    Cpu65c02Opcode::Id::BRA_REL,
    Cpu65c02Opcode::Id::BCC_REL,
    Cpu65c02Opcode::Id::BCS_REL,
    Cpu65c02Opcode::Id::BNE_REL,
    Cpu65c02Opcode::Id::BEQ_REL,
};

static const std::vector<Cpu65c02Opcode::Id> kReadModifyWriteFamily = {
    Cpu65c02Opcode::Id::ASL_ACC,
    Cpu65c02Opcode::Id::ASL_ZPG,
    Cpu65c02Opcode::Id::ASL_ZPG_X,
    Cpu65c02Opcode::Id::ASL_ABS,
    Cpu65c02Opcode::Id::ASL_ABS_X,
    Cpu65c02Opcode::Id::LSR_ACC,
    Cpu65c02Opcode::Id::LSR_ZPG,
    Cpu65c02Opcode::Id::LSR_ZPG_X,
    Cpu65c02Opcode::Id::LSR_ABS,
    Cpu65c02Opcode::Id::LSR_ABS_X,
    Cpu65c02Opcode::Id::ROL_ACC,
    Cpu65c02Opcode::Id::ROL_ZPG,
    Cpu65c02Opcode::Id::ROL_ZPG_X,
    Cpu65c02Opcode::Id::ROL_ABS,
    Cpu65c02Opcode::Id::ROL_ABS_X,
    Cpu65c02Opcode::Id::ROR_ACC,
    Cpu65c02Opcode::Id::ROR_ZPG,
    Cpu65c02Opcode::Id::ROR_ZPG_X,
    Cpu65c02Opcode::Id::ROR_ABS,
    Cpu65c02Opcode::Id::ROR_ABS_X,
    Cpu65c02Opcode::Id::INC_ZPG,
    Cpu65c02Opcode::Id::INC_ZPG_X,
    Cpu65c02Opcode::Id::INC_ABS,
    Cpu65c02Opcode::Id::INC_ABS_X,
    Cpu65c02Opcode::Id::DEC_ZPG,
    Cpu65c02Opcode::Id::DEC_ZPG_X,
    Cpu65c02Opcode::Id::DEC_ABS,
    Cpu65c02Opcode::Id::DEC_ABS_X,
    Cpu65c02Opcode::Id::TSB_ZPG,
    Cpu65c02Opcode::Id::TSB_ABS,
    Cpu65c02Opcode::Id::TRB_ZPG,
    Cpu65c02Opcode::Id::TRB_ABS,
};

static const std::vector<Cpu65c02Opcode::Id> kLogicalArithmeticFamily = {
    Cpu65c02Opcode::Id::ORA_IMM,
    Cpu65c02Opcode::Id::ORA_ZPG,
    Cpu65c02Opcode::Id::ORA_ZPG_X,
    Cpu65c02Opcode::Id::ORA_ABS,
    Cpu65c02Opcode::Id::ORA_ABS_X,
    Cpu65c02Opcode::Id::ORA_ABS_Y,
    Cpu65c02Opcode::Id::ORA_IND_X,
    Cpu65c02Opcode::Id::ORA_IND_Y,
    Cpu65c02Opcode::Id::ORA_IND,
    Cpu65c02Opcode::Id::AND_IMM,
    Cpu65c02Opcode::Id::AND_ZPG,
    Cpu65c02Opcode::Id::AND_ZPG_X,
    Cpu65c02Opcode::Id::AND_ABS,
    Cpu65c02Opcode::Id::AND_ABS_X,
    Cpu65c02Opcode::Id::AND_ABS_Y,
    Cpu65c02Opcode::Id::AND_IND_X,
    Cpu65c02Opcode::Id::AND_IND_Y,
    Cpu65c02Opcode::Id::AND_IND,
    Cpu65c02Opcode::Id::EOR_IMM,
    Cpu65c02Opcode::Id::EOR_ZPG,
    Cpu65c02Opcode::Id::EOR_ZPG_X,
    Cpu65c02Opcode::Id::EOR_ABS,
    Cpu65c02Opcode::Id::EOR_ABS_X,
    Cpu65c02Opcode::Id::EOR_ABS_Y,
    Cpu65c02Opcode::Id::EOR_IND_X,
    Cpu65c02Opcode::Id::EOR_IND_Y,
    Cpu65c02Opcode::Id::EOR_IND,
    Cpu65c02Opcode::Id::ADC_IMM,
    Cpu65c02Opcode::Id::ADC_ZPG,
    Cpu65c02Opcode::Id::ADC_ZPG_X,
    Cpu65c02Opcode::Id::ADC_ABS,
    Cpu65c02Opcode::Id::ADC_ABS_X,
    Cpu65c02Opcode::Id::ADC_ABS_Y,
    Cpu65c02Opcode::Id::ADC_IND_X,
    Cpu65c02Opcode::Id::ADC_IND_Y,
    Cpu65c02Opcode::Id::ADC_IND,
    Cpu65c02Opcode::Id::SBC_IMM,
    Cpu65c02Opcode::Id::SBC_ZPG,
    Cpu65c02Opcode::Id::SBC_ZPG_X,
    Cpu65c02Opcode::Id::SBC_ABS,
    Cpu65c02Opcode::Id::SBC_ABS_X,
    Cpu65c02Opcode::Id::SBC_ABS_Y,
    Cpu65c02Opcode::Id::SBC_IND_X,
    Cpu65c02Opcode::Id::SBC_IND_Y,
    Cpu65c02Opcode::Id::SBC_IND,
};

static const std::vector<Cpu65c02Opcode::Id> kStackFamily = {
    Cpu65c02Opcode::Id::BRK_IMP,
    Cpu65c02Opcode::Id::PHP_IMP,
    Cpu65c02Opcode::Id::PLP_IMP,
    Cpu65c02Opcode::Id::RTI_IMP,
    Cpu65c02Opcode::Id::PHA_IMP,
    Cpu65c02Opcode::Id::PLA_IMP,
    Cpu65c02Opcode::Id::RTS_IMP,
    Cpu65c02Opcode::Id::PHY_IMP,
    Cpu65c02Opcode::Id::PLY_IMP,
    Cpu65c02Opcode::Id::PHX_IMP,
    Cpu65c02Opcode::Id::PLX_IMP,
};

static const std::vector<Cpu65c02Opcode::Id> kFlagsFamily = {
    Cpu65c02Opcode::Id::CLC_IMP,
    Cpu65c02Opcode::Id::SEC_IMP,
    Cpu65c02Opcode::Id::CLI_IMP,
    Cpu65c02Opcode::Id::SEI_IMP,
    Cpu65c02Opcode::Id::CLV_IMP,
    Cpu65c02Opcode::Id::CLD_IMP,
    Cpu65c02Opcode::Id::SED_IMP,
};

static const std::vector<Cpu65c02Opcode::Id> kJsrFamily = {
    Cpu65c02Opcode::Id::JSR_ABS,
};

static const std::vector<Cpu65c02Opcode::Id> kBranchFamily = {
    Cpu65c02Opcode::Id::BPL_REL,
    Cpu65c02Opcode::Id::BMI_REL,
    Cpu65c02Opcode::Id::BVC_REL,
    Cpu65c02Opcode::Id::BVS_REL,
    Cpu65c02Opcode::Id::BRA_REL,
    Cpu65c02Opcode::Id::BCC_REL,
    Cpu65c02Opcode::Id::BCS_REL,
    Cpu65c02Opcode::Id::BNE_REL,
    Cpu65c02Opcode::Id::BEQ_REL,
};

static const std::vector<Cpu65c02Opcode::Id> kControlMiscFamily = {
    Cpu65c02Opcode::Id::BRK_IMP,
    Cpu65c02Opcode::Id::TSB_ZPG,
    Cpu65c02Opcode::Id::PHP_IMP,
    Cpu65c02Opcode::Id::TSB_ABS,
    Cpu65c02Opcode::Id::BPL_REL,
    Cpu65c02Opcode::Id::TRB_ZPG,
    Cpu65c02Opcode::Id::CLC_IMP,
    Cpu65c02Opcode::Id::INA_ACC,
    Cpu65c02Opcode::Id::TRB_ABS,
    Cpu65c02Opcode::Id::PLP_IMP,
    Cpu65c02Opcode::Id::BMI_REL,
    Cpu65c02Opcode::Id::SEC_IMP,
    Cpu65c02Opcode::Id::DEA_ACC,
    Cpu65c02Opcode::Id::RTI_IMP,
    Cpu65c02Opcode::Id::PHA_IMP,
    Cpu65c02Opcode::Id::JMP_ABS,
    Cpu65c02Opcode::Id::BVC_REL,
    Cpu65c02Opcode::Id::CLI_IMP,
    Cpu65c02Opcode::Id::PHY_IMP,
    Cpu65c02Opcode::Id::RTS_IMP,
    Cpu65c02Opcode::Id::STZ_ZPG,
    Cpu65c02Opcode::Id::PLA_IMP,
    Cpu65c02Opcode::Id::JMP_ABS_IND,
    Cpu65c02Opcode::Id::BVS_REL,
    Cpu65c02Opcode::Id::STZ_ZPG_X,
    Cpu65c02Opcode::Id::SEI_IMP,
    Cpu65c02Opcode::Id::PLY_IMP,
    Cpu65c02Opcode::Id::JMP_ABS_IND_X,
    Cpu65c02Opcode::Id::BRA_REL,
    Cpu65c02Opcode::Id::DEY_IMP,
    Cpu65c02Opcode::Id::TXA_IMP,
    Cpu65c02Opcode::Id::BCC_REL,
    Cpu65c02Opcode::Id::TYA_IMP,
    Cpu65c02Opcode::Id::TXS_IMP,
    Cpu65c02Opcode::Id::STZ_ABS,
    Cpu65c02Opcode::Id::STZ_ABS_X,
    Cpu65c02Opcode::Id::TAY_IMP,
    Cpu65c02Opcode::Id::TAX_IMP,
    Cpu65c02Opcode::Id::BCS_REL,
    Cpu65c02Opcode::Id::CLV_IMP,
    Cpu65c02Opcode::Id::TSX_IMP,
    Cpu65c02Opcode::Id::INY_IMP,
    Cpu65c02Opcode::Id::DEX_IMP,
    Cpu65c02Opcode::Id::BNE_REL,
    Cpu65c02Opcode::Id::CLD_IMP,
    Cpu65c02Opcode::Id::PHX_IMP,
    Cpu65c02Opcode::Id::INX_IMP,
    Cpu65c02Opcode::Id::BEQ_REL,
    Cpu65c02Opcode::Id::SED_IMP,
    Cpu65c02Opcode::Id::PLX_IMP,
};

} // namespace

const Cpu65c02Opcode::Definition& Cpu65c02Opcode::byId(Id id)
{
    return kDefinitions[static_cast<size_t>(id)];
}

const Cpu65c02Opcode::Definition* Cpu65c02Opcode::fromOpcodeByte(uint8_t opcodeByte)
{
    return kByteToDef[opcodeByte];
}

const std::vector<Cpu65c02Opcode::Id>& Cpu65c02Opcode::values()
{
    static const std::vector<Cpu65c02Opcode::Id> out = []() {
        std::vector<Cpu65c02Opcode::Id> ids;
        ids.reserve(kDefinitions.size());
        for( size_t i = 0; i<kDefinitions.size(); ++i )
            ids.push_back(static_cast<Cpu65c02Opcode::Id>(i));
        return ids;
    }();
    return out;
}

const std::vector<Cpu65c02Opcode::Id>& Cpu65c02Opcode::adcFamily()
{
    return kAdcFamily;
}

std::vector<uint8_t> Cpu65c02Opcode::adcOpcodeBytes()
{
    std::vector<uint8_t> out;
    out.reserve(kAdcFamily.size());
    for( Cpu65c02Opcode::Id id : kAdcFamily )
        out.push_back(byId(id).opcodeByte);
    return out;
}

const std::vector<Cpu65c02Opcode::Id>& Cpu65c02Opcode::andFamily()
{
    return kAndFamily;
}

std::vector<uint8_t> Cpu65c02Opcode::andOpcodeBytes()
{
    std::vector<uint8_t> out;
    out.reserve(kAndFamily.size());
    for( Cpu65c02Opcode::Id id : kAndFamily )
        out.push_back(byId(id).opcodeByte);
    return out;
}

const std::vector<Cpu65c02Opcode::Id>& Cpu65c02Opcode::aslFamily()
{
    return kAslFamily;
}

std::vector<uint8_t> Cpu65c02Opcode::aslOpcodeBytes()
{
    std::vector<uint8_t> out;
    out.reserve(kAslFamily.size());
    for( Cpu65c02Opcode::Id id : kAslFamily )
        out.push_back(byId(id).opcodeByte);
    return out;
}

const std::vector<Cpu65c02Opcode::Id>& Cpu65c02Opcode::bitFamily()
{
    return kBitFamily;
}

std::vector<uint8_t> Cpu65c02Opcode::bitOpcodeBytes()
{
    std::vector<uint8_t> out;
    out.reserve(kBitFamily.size());
    for( Cpu65c02Opcode::Id id : kBitFamily )
        out.push_back(byId(id).opcodeByte);
    return out;
}

const std::vector<Cpu65c02Opcode::Id>& Cpu65c02Opcode::bitTestSetFamily()
{
    return kBitTestSetFamily;
}

std::vector<uint8_t> Cpu65c02Opcode::bitTestSetOpcodeBytes()
{
    std::vector<uint8_t> out;
    out.reserve(kBitTestSetFamily.size());
    for( Cpu65c02Opcode::Id id : kBitTestSetFamily )
        out.push_back(byId(id).opcodeByte);
    return out;
}

const std::vector<Cpu65c02Opcode::Id>& Cpu65c02Opcode::branchFamily()
{
    return kBranchFamily;
}

std::vector<uint8_t> Cpu65c02Opcode::branchOpcodeBytes()
{
    std::vector<uint8_t> out;
    out.reserve(kBranchFamily.size());
    for( Cpu65c02Opcode::Id id : kBranchFamily )
        out.push_back(byId(id).opcodeByte);
    return out;
}

const std::vector<Cpu65c02Opcode::Id>& Cpu65c02Opcode::cmpFamily()
{
    return kCmpFamily;
}

std::vector<uint8_t> Cpu65c02Opcode::cmpOpcodeBytes()
{
    std::vector<uint8_t> out;
    out.reserve(kCmpFamily.size());
    for( Cpu65c02Opcode::Id id : kCmpFamily )
        out.push_back(byId(id).opcodeByte);
    return out;
}

const std::vector<Cpu65c02Opcode::Id>& Cpu65c02Opcode::controlMiscFamily()
{
    return kControlMiscFamily;
}

std::vector<uint8_t> Cpu65c02Opcode::controlMiscOpcodeBytes()
{
    std::vector<uint8_t> out;
    out.reserve(kControlMiscFamily.size());
    for( Cpu65c02Opcode::Id id : kControlMiscFamily )
        out.push_back(byId(id).opcodeByte);
    return out;
}

const std::vector<Cpu65c02Opcode::Id>& Cpu65c02Opcode::cpxFamily()
{
    return kCpxFamily;
}

std::vector<uint8_t> Cpu65c02Opcode::cpxOpcodeBytes()
{
    std::vector<uint8_t> out;
    out.reserve(kCpxFamily.size());
    for( Cpu65c02Opcode::Id id : kCpxFamily )
        out.push_back(byId(id).opcodeByte);
    return out;
}

const std::vector<Cpu65c02Opcode::Id>& Cpu65c02Opcode::cpyFamily()
{
    return kCpyFamily;
}

std::vector<uint8_t> Cpu65c02Opcode::cpyOpcodeBytes()
{
    std::vector<uint8_t> out;
    out.reserve(kCpyFamily.size());
    for( Cpu65c02Opcode::Id id : kCpyFamily )
        out.push_back(byId(id).opcodeByte);
    return out;
}

const std::vector<Cpu65c02Opcode::Id>& Cpu65c02Opcode::decFamily()
{
    return kDecFamily;
}

std::vector<uint8_t> Cpu65c02Opcode::decOpcodeBytes()
{
    std::vector<uint8_t> out;
    out.reserve(kDecFamily.size());
    for( Cpu65c02Opcode::Id id : kDecFamily )
        out.push_back(byId(id).opcodeByte);
    return out;
}

const std::vector<Cpu65c02Opcode::Id>& Cpu65c02Opcode::eorFamily()
{
    return kEorFamily;
}

std::vector<uint8_t> Cpu65c02Opcode::eorOpcodeBytes()
{
    std::vector<uint8_t> out;
    out.reserve(kEorFamily.size());
    for( Cpu65c02Opcode::Id id : kEorFamily )
        out.push_back(byId(id).opcodeByte);
    return out;
}

const std::vector<Cpu65c02Opcode::Id>& Cpu65c02Opcode::flagsFamily()
{
    return kFlagsFamily;
}

std::vector<uint8_t> Cpu65c02Opcode::flagsOpcodeBytes()
{
    std::vector<uint8_t> out;
    out.reserve(kFlagsFamily.size());
    for( Cpu65c02Opcode::Id id : kFlagsFamily )
        out.push_back(byId(id).opcodeByte);
    return out;
}

const std::vector<Cpu65c02Opcode::Id>& Cpu65c02Opcode::incFamily()
{
    return kIncFamily;
}

std::vector<uint8_t> Cpu65c02Opcode::incOpcodeBytes()
{
    std::vector<uint8_t> out;
    out.reserve(kIncFamily.size());
    for( Cpu65c02Opcode::Id id : kIncFamily )
        out.push_back(byId(id).opcodeByte);
    return out;
}

const std::vector<Cpu65c02Opcode::Id>& Cpu65c02Opcode::indexIncDecFamily()
{
    return kIndexIncdecFamily;
}

std::vector<uint8_t> Cpu65c02Opcode::indexIncDecOpcodeBytes()
{
    std::vector<uint8_t> out;
    out.reserve(kIndexIncdecFamily.size());
    for( Cpu65c02Opcode::Id id : kIndexIncdecFamily )
        out.push_back(byId(id).opcodeByte);
    return out;
}

const std::vector<Cpu65c02Opcode::Id>& Cpu65c02Opcode::interruptControlFlowFamily()
{
    return kInterruptControlFlowFamily;
}

std::vector<uint8_t> Cpu65c02Opcode::interruptControlFlowOpcodeBytes()
{
    std::vector<uint8_t> out;
    out.reserve(kInterruptControlFlowFamily.size());
    for( Cpu65c02Opcode::Id id : kInterruptControlFlowFamily )
        out.push_back(byId(id).opcodeByte);
    return out;
}

const std::vector<Cpu65c02Opcode::Id>& Cpu65c02Opcode::jsrFamily()
{
    return kJsrFamily;
}

std::vector<uint8_t> Cpu65c02Opcode::jsrOpcodeBytes()
{
    std::vector<uint8_t> out;
    out.reserve(kJsrFamily.size());
    for( Cpu65c02Opcode::Id id : kJsrFamily )
        out.push_back(byId(id).opcodeByte);
    return out;
}

const std::vector<Cpu65c02Opcode::Id>& Cpu65c02Opcode::jumpFamily()
{
    return kJumpFamily;
}

std::vector<uint8_t> Cpu65c02Opcode::jumpOpcodeBytes()
{
    std::vector<uint8_t> out;
    out.reserve(kJumpFamily.size());
    for( Cpu65c02Opcode::Id id : kJumpFamily )
        out.push_back(byId(id).opcodeByte);
    return out;
}

const std::vector<Cpu65c02Opcode::Id>& Cpu65c02Opcode::ldaFamily()
{
    return kLdaFamily;
}

std::vector<uint8_t> Cpu65c02Opcode::ldaOpcodeBytes()
{
    std::vector<uint8_t> out;
    out.reserve(kLdaFamily.size());
    for( Cpu65c02Opcode::Id id : kLdaFamily )
        out.push_back(byId(id).opcodeByte);
    return out;
}

const std::vector<Cpu65c02Opcode::Id>& Cpu65c02Opcode::ldxFamily()
{
    return kLdxFamily;
}

std::vector<uint8_t> Cpu65c02Opcode::ldxOpcodeBytes()
{
    std::vector<uint8_t> out;
    out.reserve(kLdxFamily.size());
    for( Cpu65c02Opcode::Id id : kLdxFamily )
        out.push_back(byId(id).opcodeByte);
    return out;
}

const std::vector<Cpu65c02Opcode::Id>& Cpu65c02Opcode::ldyFamily()
{
    return kLdyFamily;
}

std::vector<uint8_t> Cpu65c02Opcode::ldyOpcodeBytes()
{
    std::vector<uint8_t> out;
    out.reserve(kLdyFamily.size());
    for( Cpu65c02Opcode::Id id : kLdyFamily )
        out.push_back(byId(id).opcodeByte);
    return out;
}

const std::vector<Cpu65c02Opcode::Id>& Cpu65c02Opcode::logicalArithmeticFamily()
{
    return kLogicalArithmeticFamily;
}

std::vector<uint8_t> Cpu65c02Opcode::logicalArithmeticOpcodeBytes()
{
    std::vector<uint8_t> out;
    out.reserve(kLogicalArithmeticFamily.size());
    for( Cpu65c02Opcode::Id id : kLogicalArithmeticFamily )
        out.push_back(byId(id).opcodeByte);
    return out;
}

const std::vector<Cpu65c02Opcode::Id>& Cpu65c02Opcode::lsrFamily()
{
    return kLsrFamily;
}

std::vector<uint8_t> Cpu65c02Opcode::lsrOpcodeBytes()
{
    std::vector<uint8_t> out;
    out.reserve(kLsrFamily.size());
    for( Cpu65c02Opcode::Id id : kLsrFamily )
        out.push_back(byId(id).opcodeByte);
    return out;
}

const std::vector<Cpu65c02Opcode::Id>& Cpu65c02Opcode::oraFamily()
{
    return kOraFamily;
}

std::vector<uint8_t> Cpu65c02Opcode::oraOpcodeBytes()
{
    std::vector<uint8_t> out;
    out.reserve(kOraFamily.size());
    for( Cpu65c02Opcode::Id id : kOraFamily )
        out.push_back(byId(id).opcodeByte);
    return out;
}

const std::vector<Cpu65c02Opcode::Id>& Cpu65c02Opcode::readModifyWriteFamily()
{
    return kReadModifyWriteFamily;
}

std::vector<uint8_t> Cpu65c02Opcode::readModifyWriteOpcodeBytes()
{
    std::vector<uint8_t> out;
    out.reserve(kReadModifyWriteFamily.size());
    for( Cpu65c02Opcode::Id id : kReadModifyWriteFamily )
        out.push_back(byId(id).opcodeByte);
    return out;
}

const std::vector<Cpu65c02Opcode::Id>& Cpu65c02Opcode::rolFamily()
{
    return kRolFamily;
}

std::vector<uint8_t> Cpu65c02Opcode::rolOpcodeBytes()
{
    std::vector<uint8_t> out;
    out.reserve(kRolFamily.size());
    for( Cpu65c02Opcode::Id id : kRolFamily )
        out.push_back(byId(id).opcodeByte);
    return out;
}

const std::vector<Cpu65c02Opcode::Id>& Cpu65c02Opcode::rorFamily()
{
    return kRorFamily;
}

std::vector<uint8_t> Cpu65c02Opcode::rorOpcodeBytes()
{
    std::vector<uint8_t> out;
    out.reserve(kRorFamily.size());
    for( Cpu65c02Opcode::Id id : kRorFamily )
        out.push_back(byId(id).opcodeByte);
    return out;
}

const std::vector<Cpu65c02Opcode::Id>& Cpu65c02Opcode::sbcFamily()
{
    return kSbcFamily;
}

std::vector<uint8_t> Cpu65c02Opcode::sbcOpcodeBytes()
{
    std::vector<uint8_t> out;
    out.reserve(kSbcFamily.size());
    for( Cpu65c02Opcode::Id id : kSbcFamily )
        out.push_back(byId(id).opcodeByte);
    return out;
}

const std::vector<Cpu65c02Opcode::Id>& Cpu65c02Opcode::staFamily()
{
    return kStaFamily;
}

std::vector<uint8_t> Cpu65c02Opcode::staOpcodeBytes()
{
    std::vector<uint8_t> out;
    out.reserve(kStaFamily.size());
    for( Cpu65c02Opcode::Id id : kStaFamily )
        out.push_back(byId(id).opcodeByte);
    return out;
}

const std::vector<Cpu65c02Opcode::Id>& Cpu65c02Opcode::stackFamily()
{
    return kStackFamily;
}

std::vector<uint8_t> Cpu65c02Opcode::stackOpcodeBytes()
{
    std::vector<uint8_t> out;
    out.reserve(kStackFamily.size());
    for( Cpu65c02Opcode::Id id : kStackFamily )
        out.push_back(byId(id).opcodeByte);
    return out;
}

const std::vector<Cpu65c02Opcode::Id>& Cpu65c02Opcode::stxFamily()
{
    return kStxFamily;
}

std::vector<uint8_t> Cpu65c02Opcode::stxOpcodeBytes()
{
    std::vector<uint8_t> out;
    out.reserve(kStxFamily.size());
    for( Cpu65c02Opcode::Id id : kStxFamily )
        out.push_back(byId(id).opcodeByte);
    return out;
}

const std::vector<Cpu65c02Opcode::Id>& Cpu65c02Opcode::styFamily()
{
    return kStyFamily;
}

std::vector<uint8_t> Cpu65c02Opcode::styOpcodeBytes()
{
    std::vector<uint8_t> out;
    out.reserve(kStyFamily.size());
    for( Cpu65c02Opcode::Id id : kStyFamily )
        out.push_back(byId(id).opcodeByte);
    return out;
}

const std::vector<Cpu65c02Opcode::Id>& Cpu65c02Opcode::stzFamily()
{
    return kStzFamily;
}

std::vector<uint8_t> Cpu65c02Opcode::stzOpcodeBytes()
{
    std::vector<uint8_t> out;
    out.reserve(kStzFamily.size());
    for( Cpu65c02Opcode::Id id : kStzFamily )
        out.push_back(byId(id).opcodeByte);
    return out;
}

const std::vector<Cpu65c02Opcode::Id>& Cpu65c02Opcode::transferFamily()
{
    return kTransferFamily;
}

std::vector<uint8_t> Cpu65c02Opcode::transferOpcodeBytes()
{
    std::vector<uint8_t> out;
    out.reserve(kTransferFamily.size());
    for( Cpu65c02Opcode::Id id : kTransferFamily )
        out.push_back(byId(id).opcodeByte);
    return out;
}
