/*************************************************************************
 *                                                                       *
 *   Copyright (C) 2012 Shane Reilly                                     *
 *                                                                       *
 *   This file is part of the Ever2e Application.                        *
 *                                                                       *
 *   This file is free software: it may be redistributed and/or          *
 *   modified under the terms of the GNU General Public License as       *
 *   published by the Free Software Foundation, either version 3 of the  *
 *   License, or any later version.                                      *
 *                                                                       *
 *   This file is distributed in the hope that it will be useful, but    *
 *   without any warranty including the implied warranty of              *
 *   merchantability or fitness for a particular purpose. See the        *
 *   GNU General Public License for more details.                        *
 *                                                                       *
 *   A copy of the GNU General Public License should accompany this      *
 *   file. If it does not, it may be found at                            *
 *   <http://www.gnu.org/licenses/>.                                     *
 *                                                                       *
 *   Shane Reilly                                                        *
 *   shane@cursorcorner.net                                              *
 *                                                                       *
 *************************************************************************/
 

#include "cpu65c02.h"


using namespace std;


/// TODO: Sather 4-27 and C-15 contains more information on per-cycle instruction effects and double / triple / quadruple strobe effects ///

#define P_REG_ALT(arg, flag)  { if(arg) _P |= (flag); else _P &= ~(flag); }

#define BRANCH(condition)  { if(condition) { operandValue = _getMem8(operandPtr); Uint8 oldPage = newPc>>8; newPc += (Sint8) operandValue; cycleCount++; if( oldPage != newPc>>8 ) cycleCount++; } }

#define PTR_ADD(reg)  { Uint16 oldPage = operandPtr>>8; operandPtr += (reg); if( (operandPtr>>8) != (oldPage) ) cycleCount++; }


Uint8 Cpu65c02::_getMem8( Uint16 address )
{
	return memory->getMem(address);
}

Uint16 Cpu65c02::_getMem16( Uint16 address )
{
	return memory->getMem(address) | ( memory->getMem(address+1) << 8 );
}

Uint16 Cpu65c02::_getMemZeroPage16( Uint8 address )
{
	return memory->getMem( address ) | ( memory->getMem( Uint8(address+1) ) << 8 );
}

Uint8 Cpu65c02::_pullStack()
{
	/// Double check that underflow is ignored in real CPU ///
	return memory->getMem( 0x100 | ++_S );
}

void Cpu65c02::_pushStack( Uint8 value )
{
	/// Double check that overflow is ignored in real CPU ///
	return memory->putMem( 0x100 | _S--, value );
}

const char Cpu65c02::OPCODE_NAME[66+4][5] =
{
	"ADC", "AND", "ASL", "BCC", "BCS", "BEQ",
	"BIT", "BMI", "BNE", "BPL", "BRA", "BRK",
	"BVC", "BVS", "CLC", "CLD", "CLI", "CLV",
	"CMP", "CPX", "CPY", "DEA", "DEC", "DEX",
	"DEY", "EOR", "INA", "INC", "INX", "INY",
	"JMP", "JSR", "LDA", "LDX", "LDY", "LSR",
	"NOP", "ORA", "PHA", "PHP", "PHX", "PHY",
	"PLA", "PLP", "PLX", "PLY", "ROL", "ROR",
	"RTI", "RTS", "SBC", "SEC", "SED", "SEI",
	"STA", "STX", "STY", "STZ", "TAX", "TAY",
	"TRB", "TSB", "TSX", "TXA", "TXS", "TYA",
	"IRQ", "NMI", "RES", "NOI"
};

const char Cpu65c02::ADDRESS_MODE_NAME[15][13] =
{
	"imm         ", "abs         ", "zpg         ", 
	"acc         ", "imp         ", "(ind, X)    ",
	"(ind), Y    ", "zpg, X      ", "zpg, Y      ",
	"abs, X      ", "abs, Y      ", "rel         ",
	"(abs)       ", "abs (ind, X)", "(zpg)       "
};

// See Sather 4-27 for complete instruction cycle breakdown
// See Sather C-10 for reference to NOP cycle times / sizes

const Cpu65c02::OpcodeTable Cpu65c02::OPCODE_65C02[0x105] =
{
	{ _BRK,       _IMP,       2,          7 },  // 0x00
	{ _ORA,       _IND_X,     2,          6 },  // 0x01
	{ _NOP,       _IMP,       2,          2 },  // 0x02
	{ _NOP,       _IMP,       1,          1 },  // 0x03
	{ _TSB,       _ZPG,       2,          5 },  // 0x04
	{ _ORA,       _ZPG,       2,          3 },  // 0x05
	{ _ASL,       _ZPG,       2,          5 },  // 0x06
	{ _NOP,       _IMP,       1,          1 },  // 0x07
	{ _PHP,       _IMP,       1,          3 },  // 0x08
	{ _ORA,       _IMM,       2,          2 },  // 0x09
	{ _ASL,       _ACC,       1,          2 },  // 0x0a
	{ _NOP,       _IMP,       1,          1 },  // 0x0b
	{ _TSB,       _ABS,       3,          6 },  // 0x0c
	{ _ORA,       _ABS,       3,          4 },  // 0x0d
	{ _ASL,       _ABS,       3,          6 },  // 0x0e
	{ _NOP,       _IMP,       1,          1 },  // 0x0f
	{ _BPL,       _REL,       2,          2 },  // 0x10
	{ _ORA,       _IND_Y,     2,          5 },  // 0x11
	{ _ORA,       _ZPG_IND,   2,          5 },  // 0x12
	{ _NOP,       _IMP,       1,          1 },  // 0x13
	{ _TRB,       _ZPG,       2,          5 },  // 0x14
	{ _ORA,       _ZPG_X,     2,          4 },  // 0x15
	{ _ASL,       _ZPG_X,     2,          6 },  // 0x16
	{ _NOP,       _IMP,       1,          1 },  // 0x17
	{ _CLC,       _IMP,       1,          2 },  // 0x18
	{ _ORA,       _ABS_Y,     3,          4 },  // 0x19
	{ _INA,       _ACC,       1,          2 },  // 0x1a
	{ _NOP,       _IMP,       1,          1 },  // 0x1b
	{ _TRB,       _ABS,       3,          6 },  // 0x1c
	{ _ORA,       _ABS_X,     3,          4 },  // 0x1d
	{ _ASL,       _ABS_X,     3,          6 },  // 0x1e
	{ _NOP,       _IMP,       1,          1 },  // 0x1f
	{ _JSR,       _ABS,       3,          6 },  // 0x20
	{ _AND,       _IND_X,     2,          6 },  // 0x21
	{ _NOP,       _IMP,       2,          2 },  // 0x22
	{ _NOP,       _IMP,       1,          1 },  // 0x23
	{ _BIT,       _ZPG,       2,          3 },  // 0x24
	{ _AND,       _ZPG,       2,          3 },  // 0x25
	{ _ROL,       _ZPG,       2,          5 },  // 0x26
	{ _NOP,       _IMP,       1,          1 },  // 0x27
	{ _PLP,       _IMP,       1,          4 },  // 0x28
	{ _AND,       _IMM,       2,          2 },  // 0x29
	{ _ROL,       _ACC,       1,          2 },  // 0x2a
	{ _NOP,       _IMP,       1,          1 },  // 0x2b
	{ _BIT,       _ABS,       3,          4 },  // 0x2c
	{ _AND,       _ABS,       3,          4 },  // 0x2d
	{ _ROL,       _ABS,       3,          6 },  // 0x2e
	{ _NOP,       _IMP,       1,          1 },  // 0x2f
	{ _BMI,       _REL,       2,          2 },  // 0x30
	{ _AND,       _IND_Y,     2,          5 },  // 0x31
	{ _AND,       _ZPG_IND,   2,          5 },  // 0x32
	{ _NOP,       _IMP,       1,          1 },  // 0x33
	{ _BIT,       _ZPG_X,     2,          4 },  // 0x34
	{ _AND,       _ZPG_X,     2,          4 },  // 0x35
	{ _ROL,       _ZPG_X,     2,          6 },  // 0x36
	{ _NOP,       _IMP,       1,          1 },  // 0x37
	{ _SEC,       _IMP,       1,          2 },  // 0x38
	{ _AND,       _ABS_Y,     3,          4 },  // 0x39
	{ _DEA,       _ACC,       1,          2 },  // 0x3a
	{ _NOP,       _IMP,       1,          1 },  // 0x3b
	{ _BIT,       _ABS_X,     3,          4 },  // 0x3c
	{ _AND,       _ABS_X,     3,          4 },  // 0x3d
	{ _ROL,       _ABS_X,     3,          6 },  // 0x3e
	{ _NOP,       _IMP,       1,          1 },  // 0x3f
	{ _RTI,       _IMP,       1,          6 },  // 0x40
	{ _EOR,       _IND_X,     2,          6 },  // 0x41
	{ _NOP,       _IMP,       2,          2 },  // 0x42
	{ _NOP,       _IMP,       1,          1 },  // 0x43
	{ _NOP,       _IMP,       3,          3 },  // 0x44
	{ _EOR,       _ZPG,       2,          3 },  // 0x45
	{ _LSR,       _ZPG,       2,          5 },  // 0x46
	{ _NOP,       _IMP,       1,          1 },  // 0x47
	{ _PHA,       _IMP,       1,          3 },  // 0x48
	{ _EOR,       _IMM,       2,          2 },  // 0x49
	{ _LSR,       _ACC,       1,          2 },  // 0x4a
	{ _NOP,       _IMP,       1,          1 },  // 0x4b
	{ _JMP,       _ABS,       3,          3 },  // 0x4c
	{ _EOR,       _ABS,       3,          4 },  // 0x4d
	{ _LSR,       _ABS,       3,          6 },  // 0x4e
	{ _NOP,       _IMP,       1,          1 },  // 0x4f
	{ _BVC,       _REL,       2,          2 },  // 0x50
	{ _EOR,       _IND_Y,     2,          5 },  // 0x51
	{ _EOR,       _ZPG_IND,   2,          5 },  // 0x52
	{ _NOP,       _IMP,       1,          1 },  // 0x53
	{ _NOP,       _IMP,       4,          4 },  // 0x54
	{ _EOR,       _ZPG_X,     2,          4 },  // 0x55
	{ _LSR,       _ZPG_X,     2,          6 },  // 0x56
	{ _NOP,       _IMP,       1,          1 },  // 0x57
	{ _CLI,       _IMP,       1,          2 },  // 0x58
	{ _EOR,       _ABS_Y,     3,          4 },  // 0x59
	{ _PHY,       _IMP,       1,          3 },  // 0x5a
	{ _NOP,       _IMP,       1,          1 },  // 0x5b
	{ _NOP,       _IMP,       8,          8 },  // 0x5c
	{ _EOR,       _ABS_X,     3,          4 },  // 0x5d
	{ _LSR,       _ABS_X,     3,          6 },  // 0x5e
	{ _NOP,       _IMP,       1,          1 },  // 0x5f
	{ _RTS,       _IMP,       1,          6 },  // 0x60
	{ _ADC,       _IND_X,     2,          6 },  // 0x61
	{ _NOP,       _IMP,       2,          2 },  // 0x62
	{ _NOP,       _IMP,       1,          1 },  // 0x63
	{ _STZ,       _ZPG,       2,          3 },  // 0x64
	{ _ADC,       _ZPG,       2,          3 },  // 0x65
	{ _ROR,       _ZPG,       2,          5 },  // 0x66
	{ _NOP,       _IMP,       1,          1 },  // 0x67
	{ _PLA,       _IMP,       1,          4 },  // 0x68
	{ _ADC,       _IMM,       2,          2 },  // 0x69
	{ _ROR,       _ACC,       1,          2 },  // 0x6a
	{ _NOP,       _IMP,       1,          1 },  // 0x6b
	{ _JMP,       _ABS_IND,   3,          6 },  // 0x6c
	{ _ADC,       _ABS,       3,          4 },  // 0x6d
	{ _ROR,       _ABS,       3,          6 },  // 0x6e
	{ _NOP,       _IMP,       1,          1 },  // 0x6f
	{ _BVS,       _REL,       2,          2 },  // 0x70
	{ _ADC,       _IND_Y,     2,          5 },  // 0x71
	{ _ADC,       _ZPG_IND,   2,          5 },  // 0x72
	{ _NOP,       _IMP,       1,          1 },  // 0x73
	{ _STZ,       _ZPG_X,     2,          4 },  // 0x74
	{ _ADC,       _ZPG_X,     2,          4 },  // 0x75
	{ _ROR,       _ZPG_X,     2,          6 },  // 0x76
	{ _NOP,       _IMP,       1,          1 },  // 0x77
	{ _SEI,       _IMP,       1,          2 },  // 0x78
	{ _ADC,       _ABS_Y,     3,          4 },  // 0x79
	{ _PLY,       _IMP,       1,          4 },  // 0x7a
	{ _NOP,       _IMP,       1,          1 },  // 0x7b
	{ _JMP,       _ABS_IND_X, 3,          6 },  // 0x7c
	{ _ADC,       _ABS_X,     3,          4 },  // 0x7d
	{ _ROR,       _ABS_X,     3,          6 },  // 0x7e
	{ _NOP,       _IMP,       1,          1 },  // 0x7f
	{ _BRA,       _REL,       2,          2 },  // 0x80
	{ _STA,       _IND_X,     2,          6 },  // 0x81
	{ _NOP,       _IMP,       2,          2 },  // 0x82
	{ _NOP,       _IMP,       1,          1 },  // 0x83
	{ _STY,       _ZPG,       2,          3 },  // 0x84
	{ _STA,       _ZPG,       2,          3 },  // 0x85
	{ _STX,       _ZPG,       2,          3 },  // 0x86
	{ _NOP,       _IMP,       1,          1 },  // 0x87
	{ _DEY,       _IMP,       1,          2 },  // 0x88
	{ _BIT,       _IMM,       2,          2 },  // 0x89
	{ _TXA,       _IMP,       1,          2 },  // 0x8a
	{ _NOP,       _IMP,       1,          1 },  // 0x8b
	{ _STY,       _ABS,       3,          4 },  // 0x8c
	{ _STA,       _ABS,       3,          4 },  // 0x8d
	{ _STX,       _ABS,       3,          4 },  // 0x8e
	{ _NOP,       _IMP,       1,          1 },  // 0x8f
	{ _BCC,       _REL,       2,          2 },  // 0x90
	{ _STA,       _IND_Y,     2,          6 },  // 0x91
	{ _STA,       _ZPG_IND,   2,          5 },  // 0x92
	{ _NOP,       _IMP,       1,          1 },  // 0x93
	{ _STY,       _ZPG_X,     2,          4 },  // 0x94
	{ _STA,       _ZPG_X,     2,          4 },  // 0x95
	{ _STX,       _ZPG_Y,     2,          4 },  // 0x96
	{ _NOP,       _IMP,       1,          1 },  // 0x97
	{ _TYA,       _IMP,       1,          2 },  // 0x98
	{ _STA,       _ABS_Y,     3,          5 },  // 0x99
	{ _TXS,       _IMP,       1,          2 },  // 0x9a
	{ _NOP,       _IMP,       1,          1 },  // 0x9b
	{ _STZ,       _ABS,       3,          4 },  // 0x9c
	{ _STA,       _ABS_X,     3,          5 },  // 0x9d
	{ _STZ,       _ABS_X,     3,          5 },  // 0x9e
	{ _NOP,       _IMP,       1,          1 },  // 0x9f
	{ _LDY,       _IMM,       2,          2 },  // 0xa0
	{ _LDA,       _IND_X,     2,          6 },  // 0xa1
	{ _LDX,       _IMM,       2,          2 },  // 0xa2
	{ _NOP,       _IMP,       1,          1 },  // 0xa3
	{ _LDY,       _ZPG,       2,          3 },  // 0xa4
	{ _LDA,       _ZPG,       2,          3 },  // 0xa5
	{ _LDX,       _ZPG,       2,          3 },  // 0xa6
	{ _NOP,       _IMP,       1,          1 },  // 0xa7
	{ _TAY,       _IMP,       1,          2 },  // 0xa8
	{ _LDA,       _IMM,       2,          2 },  // 0xa9
	{ _TAX,       _IMP,       1,          2 },  // 0xaa
	{ _NOP,       _IMP,       1,          1 },  // 0xab
	{ _LDY,       _ABS,       3,          4 },  // 0xac
	{ _LDA,       _ABS,       3,          4 },  // 0xad
	{ _LDX,       _ABS,       3,          4 },  // 0xae
	{ _NOP,       _IMP,       1,          1 },  // 0xaf
	{ _BCS,       _REL,       2,          2 },  // 0xb0
	{ _LDA,       _IND_Y,     2,          5 },  // 0xb1
	{ _LDA,       _ZPG_IND,   2,          5 },  // 0xb2
	{ _NOP,       _IMP,       1,          1 },  // 0xb3
	{ _LDY,       _ZPG_X,     2,          4 },  // 0xb4
	{ _LDA,       _ZPG_X,     2,          4 },  // 0xb5
	{ _LDX,       _ZPG_Y,     2,          4 },  // 0xb6
	{ _NOP,       _IMP,       1,          1 },  // 0xb7
	{ _CLV,       _IMP,       1,          2 },  // 0xb8
	{ _LDA,       _ABS_Y,     3,          4 },  // 0xb9
	{ _TSX,       _IMP,       1,          2 },  // 0xba
	{ _NOP,       _IMP,       1,          1 },  // 0xbb
	{ _LDY,       _ABS_X,     3,          4 },  // 0xbc
	{ _LDA,       _ABS_X,     3,          4 },  // 0xbd
	{ _LDX,       _ABS_Y,     3,          4 },  // 0xbe
	{ _NOP,       _IMP,       1,          1 },  // 0xbf
	{ _CPY,       _IMM,       2,          2 },  // 0xc0
	{ _CMP,       _IND_X,     2,          6 },  // 0xc1
	{ _NOP,       _IMP,       2,          2 },  // 0xc2
	{ _NOP,       _IMP,       1,          1 },  // 0xc3
	{ _CPY,       _ZPG,       2,          3 },  // 0xc4
	{ _CMP,       _ZPG,       2,          3 },  // 0xc5
	{ _DEC,       _ZPG,       2,          5 },  // 0xc6
	{ _NOP,       _IMP,       1,          1 },  // 0xc7
	{ _INY,       _IMP,       1,          2 },  // 0xc8
	{ _CMP,       _IMM,       2,          2 },  // 0xc9
	{ _DEX,       _IMP,       1,          2 },  // 0xca
	{ _NOP,       _IMP,       1,          1 },  // 0xcb
	{ _CPY,       _ABS,       3,          4 },  // 0xcc
	{ _CMP,       _ABS,       3,          4 },  // 0xcd
	{ _DEC,       _ABS,       3,          6 },  // 0xce
	{ _NOP,       _IMP,       1,          1 },  // 0xcf
	{ _BNE,       _REL,       2,          2 },  // 0xd0
	{ _CMP,       _IND_Y,     2,          5 },  // 0xd1
	{ _CMP,       _ZPG_IND,   2,          5 },  // 0xd2
	{ _NOP,       _IMP,       1,          1 },  // 0xd3
	{ _NOP,       _IMP,       4,          4 },  // 0xd4
	{ _CMP,       _ZPG_X,     2,          4 },  // 0xd5
	{ _DEC,       _ZPG_X,     2,          6 },  // 0xd6
	{ _NOP,       _IMP,       1,          1 },  // 0xd7
	{ _CLD,       _IMP,       1,          2 },  // 0xd8
	{ _CMP,       _ABS_Y,     3,          4 },  // 0xd9
	{ _PHX,       _IMP,       1,          3 },  // 0xda
	{ _NOP,       _IMP,       1,          1 },  // 0xdb
	{ _NOP,       _IMP,       4,          4 },  // 0xdc
	{ _CMP,       _ABS_X,     3,          4 },  // 0xdd
	{ _DEC,       _ABS_X,     3,          6 },  // 0xde
	{ _NOP,       _IMP,       1,          1 },  // 0xdf
	{ _CPX,       _IMM,       2,          2 },  // 0xe0
	{ _SBC,       _IND_X,     2,          6 },  // 0xe1
	{ _NOP,       _IMP,       2,          2 },  // 0xe2
	{ _NOP,       _IMP,       1,          1 },  // 0xe3
	{ _CPX,       _ZPG,       2,          3 },  // 0xe4
	{ _SBC,       _ZPG,       2,          3 },  // 0xe5
	{ _INC,       _ZPG,       2,          5 },  // 0xe6
	{ _NOP,       _IMP,       1,          1 },  // 0xe7
	{ _INX,       _IMP,       1,          2 },  // 0xe8
	{ _SBC,       _IMM,       2,          2 },  // 0xe9
	{ _NOP,       _IMP,       1,          2 },  // 0xea
	{ _NOP,       _IMP,       1,          1 },  // 0xeb
	{ _CPX,       _ABS,       3,          4 },  // 0xec
	{ _SBC,       _ABS,       3,          4 },  // 0xed
	{ _INC,       _ABS,       3,          6 },  // 0xee
	{ _NOP,       _IMP,       1,          1 },  // 0xef
	{ _BEQ,       _REL,       2,          2 },  // 0xf0
	{ _SBC,       _IND_Y,     2,          5 },  // 0xf1
	{ _SBC,       _ZPG_IND,   2,          5 },  // 0xf2
	{ _NOP,       _IMP,       1,          1 },  // 0xf3
	{ _NOP,       _IMP,       4,          4 },  // 0xf4
	{ _SBC,       _ZPG_X,     2,          4 },  // 0xf5
	{ _INC,       _ZPG_X,     2,          6 },  // 0xf6
	{ _NOP,       _IMP,       1,          1 },  // 0xf7
	{ _SED,       _IMP,       1,          2 },  // 0xf8
	{ _SBC,       _ABS_Y,     3,          4 },  // 0xf9
	{ _PLX,       _IMP,       1,          4 },  // 0xfa
	{ _NOP,       _IMP,       1,          1 },  // 0xfb
	{ _NOP,       _IMP,       4,          4 },  // 0xfc
	{ _SBC,       _ABS_X,     3,          4 },  // 0xfd
	{ _INC,       _ABS_X,     3,          6 },  // 0xfe
	{ _NOP,       _IMP,       1,          1 },  // 0xff
	
	{ _IRQ,       _IMP,       0,          6 },  /// TODO: Verify cycle time
	{ _NMI,       _IMP,       0,          6 },  /// TODO: Verify cycle time
	{ _RES,       _IMP,       0,          6 },
	
	{ _HLT,       _IMP,       0,          1 },
	{ _NOI,       _IMP,       0,          1 }
	
};

void Cpu65c02::_playSoundCycle()
{

	if( !soundDuration )
		return;

	if( soundWaitLength == soundTimer ) {
		memory->getMem(0xc030);		
		soundDuration--;
		soundTimer = 0;
	}
	else
		soundTimer++;

}

void Cpu65c02::_cycle()
	// Complete 1 cycle of simulated 65C02 application
{

	assert( cycleCount > 0 );
	
	// Check whether current instruction is complete before applying changes
	if( --cycleCount )
		return;

#ifdef _CPU_TEST_OUTPUT	
	static bool YES = false;
	if( _PC == 0x800 )
		YES = true;
	if( YES ) {
		cout << getOpcodeString();
		cout << "   ";
		cout << getRegisterString() << endl;
	}
	if( opcode->mnemonic == _BRA )
		YES = false;
#endif

	// Update results of completed instruction
 
	Uint16 operandCounter = _PC+1;
	
	// Default instruction position
	Uint16 newPc = _PC + opcode->instrSize;

	/// TEST:
	///  Dereferenced addresses on page zero forces both low and high bytes to be pulled from page zero, even if the page boundary is crossed
	///  According to the NCR 65C02 Specs, all zero-page indirect opcodes use this convention except "zero-page indirect addressing" itself
	///  It would seem to follow that zero-page indirect addressing would also follow this convention

	/// TODO: These take one additional clock if the effective address page and given address page are different ///

	// Find these values by their addressing mode
	Uint16 operandPtr = 0x0000;
	Uint8 operandValue = 0x00;

	switch( opcode->addressMode ) {

		case _IMM:
			// Immediate
			// Literal value
			// +1 byte
			operandPtr = operandCounter;
			break;

		case _ABS:
			// Absolute
			// ADL:ADH
			// +2 byte
			operandPtr = _getMem16(operandCounter);
			break;

		case _ZPG:
			// Zero-page
			// $00:ADL
			// +1 byte
			operandPtr = _getMem8(operandCounter);
			break;

		case _ACC:
			// Accumulator
			// A
			// +0 bytes
			break;

		case _IMP:
			// Implied
			// No address
			// PC increment varies for NOP's
			break;
		
		case _IND_X:
			// (Indirect, X)
			// ($00:[ADL+X]) – carries discarded for low and high addr bytes
			// +1 byte
			operandPtr = Uint8( _getMem8(operandCounter) + _X );
			operandPtr = _getMemZeroPage16(operandPtr);
			break;

		case _IND_Y:
			// (Indirect), Y
			// ($00:ADL)+Y
			// +1 byte
			operandPtr = _getMem8(operandCounter);
			operandPtr = _getMemZeroPage16(operandPtr);
			PTR_ADD(_Y);
			break;

		case _ZPG_X:
			// Zero-page, X
			// $00:[ADL+X]
			// +1 byte
			operandPtr = Uint8( _getMem8(operandCounter) + _X );
			break;

		case _ZPG_Y:
			// Zero-page, Y
			// $00:[ADL+Y]
			// +1 byte
			operandPtr = Uint8( _getMem8(operandCounter) + _Y );
			break;
		
		case _ABS_X:
			// Absolute, X
			// [ADL:ADH] + X
			// +2 byte
			operandPtr = _getMem16(operandCounter);
			PTR_ADD(_X);
			break;

		case _ABS_Y:
			// Absolute, Y
			// [ADL:ADH] + Y
			// +2 byte
			operandPtr = _getMem16(operandCounter);
			PTR_ADD(_Y);
			break;
		
		case _REL:
			// Relative
			// PCL:PCH + ADL
			// +1 byte
			operandPtr = operandCounter;
			break;

		case _ABS_IND:
			// (Absolute)
			// PCL:PCH = (ADL:ADH)
			// +2 byte
			operandPtr = _getMem16(operandCounter);
			operandPtr = _getMem16(operandPtr);
			break;

		case _ABS_IND_X:
			// Absolute (indirect, X)
			// PCL:PCH = (ADL:ADH + X)
			// +2 byte
			operandPtr = _getMem16(operandCounter);
			PTR_ADD(_X);
			operandPtr = _getMem16(operandPtr);
			break;

		case _ZPG_IND:
			// (Zero-page)
			// ($00:ADL)
			// +1 byte
			operandPtr = _getMem8(operandCounter);
			operandPtr = _getMemZeroPage16(operandPtr);
			break;

	};

	switch( opcode->mnemonic ) {

		case _ADC:
			// Add 1 cycle for decimal mode
			// A + M + C -> A
			if( _P & _D ) {
				assert( false );  /// Dec mode not yet implemented ///
			}
			else {
				operandValue = _getMem8(operandPtr);
				Sint16 val16 = (Sint16) _A + operandValue + (_P&_C);
				_A = val16;
				P_REG_ALT(_A & 0x80, _N);
				P_REG_ALT(val16 < -0x80 || val16 > 0x7f, _V);
				P_REG_ALT(!_A, _Z);
				P_REG_ALT(val16 > 0xff, _C);
			}
			break;

		case _AND:
			// A & M -> A
			operandValue = _getMem8(operandPtr);
			_A &= operandValue;
			P_REG_ALT(_A & 0x80, _N);
			P_REG_ALT(!_A, _Z);
			break;

		case _ASL:
			if( opcode->addressMode == _ACC ) {
				// C << A << 0
				P_REG_ALT(_A & 0x80, _C);
				_A <<= 1;
				P_REG_ALT(_A & 0x80, _N);
				P_REG_ALT(!_A, _Z);
			}
			else {
				// C << M << 0
				operandValue = _getMem8(operandPtr);
				P_REG_ALT(operandValue & 0x80, _C);
				operandValue <<= 1;
				P_REG_ALT(operandValue & 0x80, _N);
				P_REG_ALT(!operandValue, _Z);
				memory->putMem(operandPtr, operandValue);
			}
			break;

		case _BCC:
			BRANCH( !(_P&_C) );
			break;

		case _BCS:
			BRANCH( _P&_C );
			break;

		case _BEQ:
			BRANCH( _P&_Z );
			break;

		case _BIT:
			// A & M
			// Stores memory bits 6 and 7 in bits 6 and 7 of the processor register
			// If immediate addressing mode is used, BIT does not effect bit 6 or 7 (4-22 Sather)
			operandValue = _getMem8(operandPtr);
			if( opcode->addressMode != _IMM ) {
				_P &= 0x3f;
				_P |= operandValue&0xc0;
			}
			P_REG_ALT(!(_A & operandValue), _Z);
			break;

		case _BMI:
			BRANCH( _P&_N );
			break;

		case _BNE:
			BRANCH( !(_P&_Z) );
			break;

		case _BPL:
			BRANCH( !(_P&_N) );
			break;

		case _BRA:
			BRANCH( true );
			break;

		case _BRK:
			/// Verify that B is not set, just pushed in P register ///
			_pushStack(newPc>>8);
			_pushStack(newPc);
			_pushStack(_P | _B);
			_P |= _I;   // Set interrupt disable
			_P &= ~_D;  // Clear decimal flag
			newPc = _getMem16(INT_BRK_VECTOR);
			break;

		case _BVC:
			BRANCH( !(_P&_V) );
			break;

		case _BVS:
			BRANCH( _P&_V );
			break;

		case _CLC:
			// 0 -> P.C;
			_P &= ~_C;
			break;

		case _CLD:
			// 0 -> P.D;
			_P &= ~_D;
			break;

		case _CLI:
			// 0 -> P.I;
			_P &= ~_I;
			break;

		case _CLV:
			// 0 -> P.V;
			_P &= ~_V;
			break;

		case _CMP:
		{
			// A - M
			operandValue = _getMem8(operandPtr);
			Sint16 val16 = (Sint16) _A - operandValue;
			P_REG_ALT(val16 & 0x80, _N);
			P_REG_ALT(!val16, _Z);
			P_REG_ALT(val16 >= 0, _C);
			break;
		}
		
		case _CPX:
		{
			// X - M
			operandValue = _getMem8(operandPtr);
			Sint16 val16 = (Sint16) _X - operandValue;
			P_REG_ALT(val16 & 0x80, _N);
			P_REG_ALT(!val16, _Z);
			P_REG_ALT(val16 >= 0, _C);
			break;
		}

		case _CPY:
		{
			// Y - M
			operandValue = _getMem8(operandPtr);
			Sint16 val16 = (Sint16) _Y - operandValue;
			P_REG_ALT(val16 & 0x80, _N);
			P_REG_ALT(!val16, _Z);
			P_REG_ALT(val16 >= 0, _C);
			break;
		}

		case _DEA:
			// A - 1 -> A
			_A--;
			P_REG_ALT(_A & 0x80, _N);
			P_REG_ALT(!_A, _Z);
			break;

		case _DEC:
			// M - 1 -> M
			operandValue = _getMem8(operandPtr);
			operandValue--;
			P_REG_ALT(operandValue & 0x80, _N);
			P_REG_ALT(!operandValue, _Z);
			memory->putMem(operandPtr, operandValue);
			break;

		case _DEX:
			// X - 1 -> X
			_X--;
			P_REG_ALT(_X & 0x80, _N);
			P_REG_ALT(!_X, _Z);
			break;

		case _DEY:
			// Y - 1 -> Y
			_Y--;
			P_REG_ALT(_Y & 0x80, _N);
			P_REG_ALT(!_Y, _Z);
			break;

		case _EOR:
			// A ^ M -> A
			operandValue = _getMem8(operandPtr);
			_A ^= operandValue;
			P_REG_ALT(_A & 0x80, _N);
			P_REG_ALT(!_A, _Z);
			break;

		case _INA:
			// A + 1 -> A
			_A++;
			P_REG_ALT(_A & 0x80, _N);
			P_REG_ALT(!_A, _Z);
			break;

		case _INC:
			// M + 1 -> M
			operandValue = _getMem8(operandPtr);
			operandValue++;
			P_REG_ALT(operandValue & 0x80, _N);
			P_REG_ALT(!operandValue, _Z);
			memory->putMem(operandPtr, operandValue);
			break;

		case _INX:
			// X + 1 -> X
			_X++;
			P_REG_ALT(_X & 0x80, _N);
			P_REG_ALT(!_X, _Z);
			break;

		case _INY:
			// Y + 1 -> Y
			_Y++;
			P_REG_ALT(_Y & 0x80, _N);
			P_REG_ALT(!_Y, _Z);
			break;

		case _JMP:
			// M -> PC
			newPc = operandPtr;
			break;

		case _JSR:
			// PC-1 -> (S)
			// M -> PC
			_pushStack((newPc-1)>>8);
			_pushStack(newPc-1);
			newPc = operandPtr;
			break;

		case _LDA:
			// M -> A
			operandValue = _getMem8(operandPtr);
			_A = operandValue;
			P_REG_ALT(_A&0x80, _N);
			P_REG_ALT(!_A, _Z);
			break;

		case _LDX:
			// M -> X
			operandValue = _getMem8(operandPtr);
			_X = operandValue;
			P_REG_ALT(_X&0x80, _N);
			P_REG_ALT(!_X, _Z);
			break;

		case _LDY:
			// M -> Y
			operandValue = _getMem8(operandPtr);
			_Y = operandValue;
			P_REG_ALT(_Y&0x80, _N);
			P_REG_ALT(!_Y, _Z);
			break;

		case _LSR:
			if( opcode->addressMode == _ACC ) {
				// 0 >> A >> C
				P_REG_ALT(_A & 0x01, _C);
				_A >>= 1;
				P_REG_ALT(0, _N);
				P_REG_ALT(!_A, _Z);
			}
			else {
				operandValue = _getMem8(operandPtr);
				// 0 >> M >> C
				P_REG_ALT(operandValue & 0x01, _C);
				operandValue >>= 1;
				P_REG_ALT(0, _N);
				P_REG_ALT(!operandValue, _Z);
				memory->putMem(operandPtr, operandValue);
			}
			break;

		case _NOP:
			// No operation - $EA / undocumented NOP
			break;

		case _ORA:
			// A | M -> A
			operandValue = _getMem8(operandPtr);
			_A |= operandValue;
			P_REG_ALT(_A & 0x80, _N);
			P_REG_ALT(!_A, _Z);
			break;

		case _PHA:
			// A -> (S)
			_pushStack(_A);
			break;

		case _PHP:
			// P -> (S)
			_pushStack(_P);
			break;

		case _PHX:
			// X -> (S)
			_pushStack(_X);
			break;

		case _PHY:
			// Y -> (S)
			_pushStack(_Y);
			break;

		case _PLA:
			// (S) -> A
			_A = _pullStack();
			P_REG_ALT(_A&0x80, _N);
			P_REG_ALT(!_A, _Z);
			break;

		case _PLP:
			// (S) -> P
			_P = _pullStack() | _B;
			break;

		case _PLX:
			// (S) -> X
			_X = _pullStack();
			P_REG_ALT(_X&0x80, _N);
			P_REG_ALT(!_X, _Z);
			break;

		case _PLY:
			// (S) -> Y
			_Y = _pullStack();
			P_REG_ALT(_Y&0x80, _N);
			P_REG_ALT(!_Y, _Z);
			break;

		case _ROL:
			if( opcode->addressMode == _ACC ) {
				// C << A << C
				bool carry = _P&_C;
				P_REG_ALT(_A & 0x80, _C);
				_A <<= 1;
				_A |= carry; 
				P_REG_ALT(_A & 0x80, _N);
				P_REG_ALT(!_A, _Z);
			}
			else {
				// C << M << C
				operandValue = _getMem8(operandPtr);
				bool carry = _P&_C;
				P_REG_ALT(operandValue & 0x80, _C);
				operandValue <<= 1;
				operandValue |= carry; 
				P_REG_ALT(operandValue & 0x80, _N);
				P_REG_ALT(!operandValue, _Z);
				memory->putMem(operandPtr, operandValue);
			}
			break;

		case _ROR:
			if( opcode->addressMode == _ACC ) {
				// C >> A >> C
				bool carry = _P&_C;
				P_REG_ALT(_A & 0x01, _C);
				_A >>= 1;
				_A |= carry<<7;
				P_REG_ALT(_A & 0x80, _N);
				P_REG_ALT(!_A, _Z);
			}
			else {
				// C >> M >> C
				operandValue = _getMem8(operandPtr);
				bool carry = _P&_C;
				P_REG_ALT(operandValue & 0x01, _C);
				operandValue >>= 1;
				operandValue |= carry<<7;
				P_REG_ALT(operandValue & 0x80, _N);
				P_REG_ALT(!operandValue, _Z);
				memory->putMem(operandPtr, operandValue);
			}
			break;

		case _RTI:
			// (S) -> PC
			_P = _pullStack() | _B;
			newPc = _pullStack();
			newPc |= _pullStack() << 8;
			break;

		case _RTS:
			// (S)+1 -> PC
			newPc = _pullStack();
			newPc |= _pullStack() << 8;
			newPc++;
			break;

		case _SBC:
			operandValue = _getMem8(operandPtr);
			if( _P & _D ) {
				// A - M - !C -> A
				assert( false );  /// Dec mode not yet implemented ///
/*				
				// Add 1 cycle for decimal mode
				cycleCount++;
				Uint16 val;
				Uint8 ah = a&0xf0;
				Uint8 al = a&0x0f;
				Uint8 vh = operandValue&0xf0;
				Uint8 vl = operandValue&0x0f;
				val = al + vl + (_P&_C);
				if( val>=0xa0 )
					val += 0x06;
					val &= 0x0f;
					val |= 0x10;
				}
				val += ah;
				val += vh;
				val += al;
*/
			}
			else {
				Sint16 val16 = (Sint16) _A - operandValue - !(_P&_C);
				_A = val16;
				P_REG_ALT(_A & 0x80, _N);
				P_REG_ALT(val16 <-0x80 || val16 > 0x7f, _V);
				P_REG_ALT(!_A, _Z);
				P_REG_ALT(val16 >= 0, _C);
			}
			break;

		case _SEC:
			// 1 -> P.C;
			_P |= _C;
			break;

		case _SED:
			// 1 -> P.D;
			_P |= _D;
			break;

		case _SEI:
			// 1 -> P.I;
			_P |= _I;
			break;

		case _STA:
			// A -> M
			memory->putMem(operandPtr, _A);
			break;

		case _STX:
			// X -> M
			memory->putMem(operandPtr, _X);
			break;

		case _STY:
			// Y -> M
			memory->putMem(operandPtr, _Y);
			break;

		case _STZ:
			// $00 -> M
			memory->putMem(operandPtr, 0x00);
			break;

		case _TAX:
			// A -> X
			_X = _A;
			P_REG_ALT(!_X, _Z);
			P_REG_ALT(_X & 0x80, _N);
			break;

		case _TAY:
			// A -> Y
			_Y = _A;
			P_REG_ALT(!_Y, _Z);
			P_REG_ALT(_Y & 0x80, _N);
			break;

		case _TRB:
			// A & M -> M
			operandValue = _getMem8(operandPtr);
			P_REG_ALT(_A & operandValue, _Z);
			operandValue &= ~_A;
			memory->putMem(operandPtr, operandValue);
			break;

		case _TSB:
			// A | M -> M
			operandValue = _getMem8(operandPtr);
			P_REG_ALT(!(_A & operandValue), _Z);
			operandValue |= _A;
			memory->putMem(operandPtr, operandValue);
			break;

		case _TSX:
			// S -> X
			_X = _S;
			P_REG_ALT(!_X, _Z);
			P_REG_ALT(_X & 0x80, _N);
			break;

		case _TXA:
			// X -> A
			_A = _X;
			P_REG_ALT(!_A, _Z);
			P_REG_ALT(_A & 0x80, _N);
			break;

		case _TXS:
			// X -> S
			_S = _X;
			break;

		case _TYA:
			// Y -> A
			_A = _Y;
			P_REG_ALT(!_A, _Z);
			P_REG_ALT(_A & 0x80, _N);
			break;

		case _IRQ:
			// IRQ's are given priority starting with slot 1 to 7 (4-16 of Sather)
			/// Verify this ///
			_pushStack(newPc>>8);
			_pushStack(newPc);
			_pushStack(_P);
			_P |= _I;          // Set interrupt disable
			_P &= ~(_B | _D);  // Clear break and decimal flag
			newPc = _getMem16(INT_IRQ_VECTOR);
			break;

		case _NMI:
			/// Verify this ///
			_pushStack(newPc>>8);
			_pushStack(newPc);
			_pushStack(_P);
			_P |= _I;          // Set interrupt disable
			_P &= ~(_B | _D);  // Clear break and decimal flag
			newPc = _getMem16(INT_NMI_VECTOR);
			break;

		case _RES:
			// Reset - reset key up / cold boot
			// This sequence lasts 6 cycles (NCR 65C02 Datasheet)
			// Also pulls 3 values from the stack and resets all, but 2 switches in the MMU
			/// Should emulate reset signature recognition described in 4-14 and 5-29 of Sather ///
			/// _S = 0;  /// Verify
///			_pullStack();
///			_pullStack();
///			_pullStack();
/// Compatibility with test emulator
_pushStack(0x00);
_pushStack(0x00);
_pushStack(0x00);
			_P |= _I;   // Set interrupt disable
			_P &= ~_D;  // Clear decimal flag
			newPc = _getMem16(INT_RES_VECTOR);
			break;

		case _HLT:
			// Halt execution (reset key down)
			memory->warmReset();
			break;

		case _NOI:
			// No interrupt
			assert(false);  // Not an instruction
			break;

	}

	// Supress maskable interrupts if P.I is set
	if( interruptPending==_IRQ && (_P&_I) )
		interruptPending = _NOI;

	// Get next instruction / interrupt call
	switch( interruptPending )
	{

		case _NOI:
			// No interrupt pending - get next memory instruction
			opcode = &OPCODE_65C02[memory->getMem(newPc)];
			break;

		case _RES:
			opcode = &OPCODE_65C02[TABLE_RES];
			interruptPending = _NOI;
			break;

		case _IRQ:			
			opcode = &OPCODE_65C02[TABLE_IRQ];
			interruptPending = _NOI;
			break;

		case _NMI:
			opcode = &OPCODE_65C02[TABLE_NMI];
			interruptPending = _NOI;
			break;

		case _HLT:
			opcode = &OPCODE_65C02[TABLE_HLT];
			// Pending interrupt can only be changed by host (e.g. creating reset interrupt or restoring NOI state)
			break;

		default:
			cerr << "Cpu65c02::_cycle - Unsupported interrupt type requested.\n";
			exit(1);
			
	}
	cycleCount += opcode->cycleTime;

	_PC = newPc;

}

int Cpu65c02::_decodeTextByte( char c, TextType type, Uint8 & byte )
{

	Uint8 chr = _transliterate(c);

	switch( chr ) {

		case '\001':
			textType = NORMAL;
			byte = 0x00;
			return 0;

		case '\002':
			textType = INVERSE;
			byte = 0x00;
			return 0;

		case '\003':
			textType = FLASH;
			byte = 0x00;
			return 0;

		case '\007':
			playSound();
			byte = 0x00;
			return 0;

	}

	if( chr<0x20 || chr>0x7f ) {
		byte = 0x00;
		return 0;
	}
	
	if( type == ANY )
		type = textType;

	Uint8 typeInc;

	switch( type ) {

		case FLASH:
		
			if( memory->getMem(0xc01e) & 0x80 ) {

				// Mouse text
				if( (chr < ASC_A || chr > ASC_Z) && (chr < ASC_a || chr > ASC_z) && (chr < ASC_0 || chr > (ASC_0+5)) ) {
					chr = 0xa0;
					typeInc = 0x00;
				}
				else {
					if( chr >= ASC_0 && chr <= ASC_9 )  // Chr is a number
						chr += 11;
					chr &= 0x1f;
					typeInc = 0x40;
				}
				
			}
			else {

					typeInc = 0x00;
				// Original text mode
//if( chr >= ASC_0 && chr <= ASC_9 )  // Chr is a number
//	chr &= 0x3f;
				if( chr>=0x60 )
					chr -= 0x20;
					
			}
			break;
			
		case INVERSE:
		
			typeInc = 0x00;
			
			if( memory->getMem(0xc01e) & 0x80 ) {
			
				// Mouse text
				
				if( chr >= ASC_a-1 )
					typeInc = 0x40;
				chr &= 0x3f;
				
			}
			else {
			
				// Original text mode
				if( chr >= ASC_a-1 )
					chr -= 0x20;
			
			}
			break;
			
		default:
		
			chr &= 0x7f;
			typeInc = 0x80;
			break;
			
	}

	byte = chr+typeInc;
	return 1;
	
}

int Cpu65c02::_putText40( int page, int row, int col, char c, TextType type )
{
	page = _pageTranslate(page);
	assert( (unsigned int) row < 24);
	assert( (unsigned int) col < 40);
	Uint8 byte;
	if( _decodeTextByte(c, type, byte) ) {
		putByteLo40(page, row, col, byte);
		return 1;
	}
	else
		return 0;
}

int Cpu65c02::_putText40( int page, int row, int col, const string &str, TextType type )
{
	int len = str.length();
	int c = 0;
	for( int i = 0; i<len; i++ )
		c += _putText40(page, row, col+c, str[i], type);
	return c;
}

int Cpu65c02::_putText80( int page, int row, int col, char c, TextType type )
{
	page = _pageTranslate(page);
	Uint8 byte;
	if( _decodeTextByte(c, type, byte) ) {
		putByteLo80(page, row, col, byte);
		return 1;
	}
	else
		return 0;
}

int Cpu65c02::_putText80( int page, int row, int col, const string &str, TextType type )
{
	int len = str.length();
	int c = 0;
	for( int i = 0; i<len; i++ )
		c += _putText40(page, row, col+c, str[i], type);
	return c;
}

int Cpu65c02::_pageTranslate( int page )
{
	if( page<1 || page>2 )
		return (memory->getMem(0xc01c)&0x80) ? 2 : 1;
	else return page;
}

Uint8 Cpu65c02::_transliterate( char c )
{
	/// Modify to work for multi-byte characters ///
	switch( c )
	{
		case '\x00': return 0x00;
		case '\x01': return 0x01;
		case '\x02': return 0x02;
		case '\x03': return 0x03;
		case '\x04': return 0x04;
		case '\x05': return 0x05;
		case '\x06': return 0x06;
		case '\x07': return 0x07;
		case '\x08': return 0x08;
		case '\x09': return 0x09;
		case '\x0A': return 0x0A;
		case '\x0B': return 0x0B;
		case '\x0C': return 0x0C;
		case '\x0D': return 0x0D;
		case '\x0E': return 0x0E;
		case '\x0F': return 0x0F;
		case '\x10': return 0x10;
		case '\x11': return 0x11;
		case '\x12': return 0x12;
		case '\x13': return 0x13;
		case '\x14': return 0x14;
		case '\x15': return 0x15;
		case '\x16': return 0x16;
		case '\x17': return 0x17;
		case '\x18': return 0x18;
		case '\x19': return 0x19;
		case '\x1A': return 0x1A;
		case '\x1B': return 0x1B;
		case '\x1C': return 0x1C;
		case '\x1D': return 0x1D;
		case '\x1E': return 0x1E;
		case '\x1F': return 0x1F;
		case ' ': return 0x20;
		case '!': return 0x21;
		case '"': return 0x22;
		case '#': return 0x23;
		case '$': return 0x24;
		case '%': return 0x25;
		case '&': return 0x26;
		case '\'': return 0x27;
		case '(': return 0x28;
		case ')': return 0x29;
		case '*': return 0x2A;
		case '+': return 0x2B;
		case ',': return 0x2C;
		case '-': return 0x2D;
		case '.': return 0x2E;
		case '/': return 0x2F;
		case '0': return 0x30;
		case '1': return 0x31;
		case '2': return 0x32;
		case '3': return 0x33;
		case '4': return 0x34;
		case '5': return 0x35;
		case '6': return 0x36;
		case '7': return 0x37;
		case '8': return 0x38;
		case '9': return 0x39;
		case ':': return 0x3A;
		case ';': return 0x3B;
		case '<': return 0x3C;
		case '=': return 0x3D;
		case '>': return 0x3E;
		case '?': return 0x3F;
		case '@': return 0x40;
		case 'A': return 0x41;
		case 'B': return 0x42;
		case 'C': return 0x43;
		case 'D': return 0x44;
		case 'E': return 0x45;
		case 'F': return 0x46;
		case 'G': return 0x47;
		case 'H': return 0x48;
		case 'I': return 0x49;
		case 'J': return 0x4A;
		case 'K': return 0x4B;
		case 'L': return 0x4C;
		case 'M': return 0x4D;
		case 'N': return 0x4E;
		case 'O': return 0x4F;
		case 'P': return 0x50;
		case 'Q': return 0x51;
		case 'R': return 0x52;
		case 'S': return 0x53;
		case 'T': return 0x54;
		case 'U': return 0x55;
		case 'V': return 0x56;
		case 'W': return 0x57;
		case 'X': return 0x58;
		case 'Y': return 0x59;
		case 'Z': return 0x5A;
		case '[': return 0x5B;
		case '\\': return 0x5C;
		case ']': return 0x5D;
		case '^': return 0x5E;
		case '_': return 0x5F;
		case '`': return 0x60;
		case 'a': return 0x61;
		case 'b': return 0x62;
		case 'c': return 0x63;
		case 'd': return 0x64;
		case 'e': return 0x65;
		case 'f': return 0x66;
		case 'g': return 0x67;
		case 'h': return 0x68;
		case 'i': return 0x69;
		case 'j': return 0x6A;
		case 'k': return 0x6B;
		case 'l': return 0x6C;
		case 'm': return 0x6D;
		case 'n': return 0x6E;
		case 'o': return 0x6F;
		case 'p': return 0x70;
		case 'q': return 0x71;
		case 'r': return 0x72;
		case 's': return 0x73;
		case 't': return 0x74;
		case 'u': return 0x75;
		case 'v': return 0x76;
		case 'w': return 0x77;
		case 'x': return 0x78;
		case 'y': return 0x79;
		case 'z': return 0x7A;
		case '{': return 0x7B;
		case '|': return 0x7C;
		case '}': return 0x7D;
		case '~': return 0x7E;
		case '\x7F': return 0x7F;
		default: return 0x2A;
	}
	
}

Cpu65c02::Cpu65c02( Memory128k* memory )
{

	this->memory = memory;
	
	// Startup register values
	/// Verify values for real CPU ///
	_A = 0xff;
	_Y = 0xff;
	_X = 0xff;
	_PC = 0xffff;
	_S = 0xff;
	_P = 0xff;

	// Default CPU speed
	multiplier = 1;

	// This variable is used to suspend CPU access to memory by hardware
	idleCycle = 0;

	// First instruction is a reset interrupt
	opcode = &OPCODE_65C02[TABLE_RES];
	cycleCount = opcode->cycleTime;
	
	// Turn off interrupt line to CPU
	interruptPending = _NOI;

	// Default host text display settings
	textType = NORMAL;

	// Host-accessable sound routine variables
	soundTimer = 0;
	soundWaitLength = 0;
	soundDuration = 0;

}

Cpu65c02::~Cpu65c02()
{
}

void Cpu65c02::setMultiplier( int multiplier )
{
	assert( multiplier>0 );
	this->multiplier = multiplier;
}

int Cpu65c02::getMultiplier()
{
	return multiplier;
}

void Cpu65c02::interrupt( OpcodeMnemonic type )
{
	interruptPending = type;	
}

void Cpu65c02::cycleSteal( int cycles )
{
	assert( cycles>0 && cycles<=5 );
	assert( idleCycle==-1 );
	idleCycle = cycles;
}
	
void Cpu65c02::cycle()
	// Complete cycle(s) of simulated 65C02 application
{

	if( idleCycle>=0 )
		idleCycle--;
	if( idleCycle<=0 ) {
		memory->memAccess();  // Indicate that an emulated CPU access to memory is being made
		for( int c = 0; c<multiplier; c++ ) {
			if( !isPlayingSound() )
				_cycle();
			else
				_playSoundCycle();  // Emulate sounds that have been triggered by a call to "beep"
		}
	}
	
}

string Cpu65c02::getOpcodeString()
{

	stringstream out;

	Uint16 address = _PC;
	Uint16 machineCode = ((int)opcode - (int)(&OPCODE_65C02)) / sizeof(OpcodeTable);
	Uint8 operandLow = 0;
	Uint8 operandHigh = 0;
	
	out << hex << uppercase << setfill ('0');

	out << setw(4) << address << ": ";

	if( machineCode < 0x100 )
		out << setw(2) << (int) machineCode;
	
	if( opcode->instrSize == 0 )
		out << "         ";
	else if( opcode->instrSize == 1 )
		out << "       ";
	else if( opcode->instrSize == 2 ) {
		operandLow = memory->getMem(_PC+1);
		out << " " << setw(2) << (int) operandLow << "    ";
	}
	else if( opcode->instrSize == 3 ) {
		operandLow = memory->getMem(_PC+1);
		operandHigh = memory->getMem(_PC+2);
		out << " " << setw(2) << (int) operandLow << " " << setw(2) << (int) operandHigh << " ";
	}
	else
		assert( opcode->instrSize>=0 && opcode->instrSize<=3 );

	out << "  " << OPCODE_NAME[opcode->mnemonic] << ' ' << ADDRESS_MODE_NAME[opcode->addressMode];

	if( opcode->instrSize==2 )
		out << "   " << setw(2) << int(operandLow);
	else if( opcode->instrSize==3 )
		out << " " << setw(4) << int ( operandLow | ( operandHigh << 8) );
	else
		out << "     ";

	return out.str();
	
/*
	switch( opcode->addressMode ) {
	
		case _IMM
			// Immediate
			break;
		
		case _ABS:
			// Absolute
			break;
		
		case _ZPG:
			// Zero-page
			break;
		
		case _ACC:
			// Accumulator
			break;
		
		case _IMP:
			// Implied
			break;
		
		case _IND_X:
			// (Indirect, X)
			break;

		case _IND_Y:
			// (Indirect), Y
			break;
		
		case _ZPG_X:
			// Zero-page, X
			break;
		
		case _ZPG_Y:
			// Zero-page, Y
			break;
		
		case _ABS_X:
			// Absolute, X
			break;
		
		case _ABS_Y:
			// Absolute, Y
			break;
		
		case _REL:
			// Relative
			break;
		
		case _ABS_IND:
			// (Absolute)
			break;
		
		case _ABS_IND_X:
			// Absolute (indirect, X)
			break;
		
		case _ZPG_IND:
			// (Zero-page)
			break;

	};*/
		
}

string Cpu65c02::getRegisterString()
{

	stringstream out;

	out << hex << uppercase << setfill ('0');
    
	out << "A: " << setw(2) << (int) _A << " ";
	out << "Y: " << setw(2) << (int) _Y << " ";
	out << "X: " << setw(2) << (int) _X << " ";
	out << "PC: " << setw(4) << (int) _PC << " ";
	out << "S: " << setw(2) << (int) _S << " ";
	out << "P: ";
	out << (_P&_N ? 'N':'.');
	out << (_P&_V ? 'V':'.');
	out << (_P&_1 ? '1':'.');
	out << (_P&_B ? 'B':'.');
	out << (_P&_D ? 'D':'.');
	out << (_P&_I ? 'I':'.');
	out << (_P&_Z ? 'Z':'.');
	out << (_P&_C ? 'C':'.');
	
	return out.str();
	
}

double Cpu65c02::noteToFreq( double p )
{
	return 440. * pow(2., (p-69.)/12.);
}

void Cpu65c02::playSound( float freq, float duration )
{
	Sint32 avgClock = Cpu65c02::CPU_TIMING_NANOSECONDS + Monitor560x192::HORIZONTAL_CPU_SYNC_TIMING/Monitor560x192::HORIZONTAL_SCANS;
	soundWaitLength = 1000000000./avgClock/freq/2.;
	soundDuration = duration*1000000./soundWaitLength;
}

void Cpu65c02::stopSound()
{
	soundDuration = 0;
}

bool Cpu65c02::isPlayingSound()
{
	return soundDuration!=0;
}

int Cpu65c02::putText( int page, int row, int col, char c, TextType type )
{
	if( memory->getMem(0xc01f) )       // 80-col mode is on
		return _putText80(page, row, col, c, type);
	else
		return _putText40(page, row, col, c, type);
}
	
int Cpu65c02::putText( int page, int row, int col, const string &str, TextType type )
{
	if( memory->getMem(0xc01f) )       // 80-col mode is on
		return _putText80(page, row, col, str, type);
	else
		return _putText40(page, row, col, str, type);
}

void Cpu65c02::putLores( int page, int x, int y, Uint8 color )
{
	assert( (unsigned int) x<Monitor560x192::HORIZONTAL_BLANK );
	assert( (unsigned int) y<(Monitor560x192::VERTICAL_BLANK>>2) );
	color &= 0x0f;
	setPage(page);

	Uint8 colorByte = getByteLo40(page, y>>1, x);
	if( y&0x01 )
		colorByte = (colorByte&0x0f)|(color<<4);
	else
		colorByte = (colorByte&0xf0)|color;
	putByteLo40(page, y>>1, x, colorByte);
}

void Cpu65c02::putDoubleLores( int page, int x, int y, Uint8 color )
{
	assert( (unsigned int) x<Monitor560x192::HORIZONTAL_BLANK<<1 );
	assert( (unsigned int) y<(Monitor560x192::VERTICAL_BLANK>>2) );
	color &= 0x0f;
	setPage(page);
	
	// Rotate color for even rows to match standard colors
	if( !(x&0x01) )
		color = ( 0x0f&(color<<3) ) | (color>>1);
		
	Uint8 colorByte = getByteLo80(page, y>>1, x);
	if( y&0x01 )
		colorByte = (colorByte&0x0f)|(color<<4);
	else
		colorByte = (colorByte&0xf0)|color;
		
	putByteLo80(page, y>>1, x, colorByte);
}

void Cpu65c02::putHiresMono( int page, int x, int y, bool color, int palette )
{

	assert( (unsigned int) color<16 );
	assert( (unsigned int) x<Monitor560x192::HORIZONTAL_BLANK*7 );
	assert( (unsigned int) y<Monitor560x192::VERTICAL_BLANK );
	setPage(page);

	Uint8 colorByte = getByteHi40(page, y, x/7);
	int bitOffset = x%7;

	// Change palette
	if( palette==1 )
		colorByte &= 0x7f;
	else if( palette==2 )
		colorByte |= 0x80;

	// Change bit
	if( color )
		colorByte |= 0x01<<bitOffset;
	else
		colorByte &= ~(0x01<<bitOffset);

	// Write byte to memory
	putByteHi40(page, y, x/7, colorByte);

	// Set neighboring palette if pixel lies on a border
	if( palette && bitOffset==6 && x+1<Monitor560x192::HORIZONTAL_BLANK*7 ) {
	
		colorByte = getByteHi40(page, y, (x+1)/7);
		if( palette==1 )
			colorByte &= 0x7f;
		else
			colorByte |= 0x80;
		putByteHi40(page, y, (x+1)/7, colorByte);

	}
	
}

void Cpu65c02::putHiresColor( int page, int x, int y, int color )
{	
//	assert( (unsigned int) color<6 );
	color %= 6;
	if( color == 0 || color == 5 ) {		
		putHiresMono(page, x<<1, y, color, 0);
		putHiresMono(page, (x<<1)+1, y, color, 0);
		return;
	}
	color--;
	bool oddPixel = color&0x01;
	int palette = (color>>1)+1;
	putHiresMono(page, x<<1, y, !oddPixel, palette);
	putHiresMono(page, (x<<1)+1, y, oddPixel, palette);
}

void Cpu65c02::putDoubleHiresMono( int page, int x, int y, bool color )
{
	assert( (unsigned int) color<16 );
	assert( (unsigned int) x<Monitor560x192::HORIZONTAL_BLANK*14 );
	assert( (unsigned int) y<Monitor560x192::VERTICAL_BLANK );
	setPage(page);

	Uint8 colorByte = getByteHi80(page, y, x/7);
	int bitOffset = x%7;

	// Change bit
	if( color )
		colorByte |= 0x01<<bitOffset;
	else
		colorByte &= ~(0x01<<bitOffset);

	// Write byte to memory
	putByteHi80(page, y, x/7, colorByte);
}

void Cpu65c02::putDoubleHiresColor( int page, int x, int y, int color )
{
	//assert( (unsigned int) color<16 );
	color &= 0x0f;
	// Transform palette to lores colors
	color = (color>>1) | ( (color<<3)&0x0f );
	int bitMask = 0x01;
	for( int pixel = 0; pixel<4; pixel++, bitMask<<=1 )
		if( bitMask&color ) 
			putDoubleHiresMono(page, (x<<2)+pixel, y, 1);		
		else
			putDoubleHiresMono(page, (x<<2)+pixel, y, 0);
}

void Cpu65c02::putByteLo40( int page, int row, int col, Uint8 byte )
{
	setMemoryPage(1);
	Uint16 addr = Monitor560x192::getAddressLo40(page, row, col);
	memory->putMem(addr, byte);
}

void Cpu65c02::putByteHi40( int page, int row, int col, Uint8 byte )
{
	setMemoryPage(1);
	Uint16 addr = Monitor560x192::getAddressHi40(page, row, col);
	memory->putMem(addr, byte);
}

void Cpu65c02::putByteLo80( int page, int row, int col, Uint8 byte )
{
	setMemoryPage(col&0x01);
	Uint16 addr = Monitor560x192::getAddressLo40(page, row, col>>1);
	memory->putMem(addr, byte);
}

void Cpu65c02::putByteHi80( int page, int row, int col, Uint8 byte )
{
	setMemoryPage(col&0x01);
	Uint16 addr = Monitor560x192::getAddressHi40(page, row, col>>1);
	memory->putMem(addr, byte);
}

Uint8 Cpu65c02::getByteLo40( int page, int row, int col )
{
	setMemoryPage(1);
	Uint16 addr = Monitor560x192::getAddressLo40(page, row, col);
	return memory->getMem(addr);
}

Uint8 Cpu65c02::getByteHi40( int page, int row, int col )
{
	setMemoryPage(1);
	Uint16 addr = Monitor560x192::getAddressHi40(page, row, col);
	return memory->getMem(addr);
}

Uint8 Cpu65c02::getByteLo80( int page, int row, int col )
{
	setMemoryPage(col&0x01);
	Uint16 addr = Monitor560x192::getAddressLo40(page, row, col>>1);
	return memory->getMem(addr);
}

Uint8 Cpu65c02::getByteHi80( int page, int row, int col )
{
	setMemoryPage(col&0x01);
	Uint16 addr = Monitor560x192::getAddressHi40(page, row, col>>1);
	return memory->getMem(addr);
}

void Cpu65c02::setModeText40()
{
	// Sather 8-20
	memory->getMem(0xc051);        // Select text mode
	memory->putMem(0xc00c, 0x00);  // 80-col off (single-res)
	setModeFullScreen();
	setMouseText();
	setPage(1);
	clearDisplay();
}

void Cpu65c02::setModeText80()
{
	// Sather 8-20
	memory->getMem(0xc051);        // Select text mode
	memory->putMem(0xc00d, 0x00);  // 80-col on (double-res)
	setModeFullScreen();
	setMouseText();
	setPage(1);
	clearDisplay();
}

void Cpu65c02::setModeLores()
{
	// Sather 8-20
	memory->getMem(0xc050);  // Graphics mode
	memory->getMem(0xc056);  // Lores on
	memory->getMem(0xc05f);  // AN3 on - turning this off forces 7Mhz lo-res pattern interpretation
	memory->putMem(0xc00c, 0x00);  // 80-col off (single-res)
	setModeFullScreen();
	setPage(1);
	clearDisplay();
}

void Cpu65c02::setModeDoubleLores()
{
	// Sather 8-29
	memory->getMem(0xc050);  // Graphics mode
	memory->getMem(0xc056);  // Lores on
	memory->getMem(0xc05e);  // AN3 off
	memory->putMem(0xc00d, 0x00);  // 80-col on (double-res)
	setModeFullScreen();
	setPage(1);
	clearDisplay();
}

void Cpu65c02::setModeHires()
{
	// Sather 8-20
	memory->getMem(0xc050);  // Graphics mode
	memory->getMem(0xc057);  // Hires on
	memory->getMem(0xc05f);  // AN3 on - turning this off disables pattern shifting
	memory->putMem(0xc00c, 0x00);  // 80-col off (single-res)
	setModeFullScreen();
	setPage(1);
	clearDisplay();
}

void Cpu65c02::setModeDoubleHires()
{
	// Sather 8-20
	memory->getMem(0xc050);  // Graphics mode
	memory->getMem(0xc057);  // Hires on
	memory->getMem(0xc05e);  // AN3 off
	memory->putMem(0xc00d, 0x00);  // 80-col on (double-res)
	setModeFullScreen();
	setPage(1);
	clearDisplay();
}

void Cpu65c02::setModeSplitScreen()
{
	memory->getMem(0xc053);    // Turn split screen on
	clearDisplay();
}

void Cpu65c02::setModeFullScreen()
{
	memory->getMem(0xc052);    // Turn split screen off
	clearDisplay();
}

void Cpu65c02::setPage( int page )
{

	page = _pageTranslate(page);

	assert(!(memory->getMem(0xc018)&0x80));  // Visible page can not be altered when 80STORE is off
	
	if( page==1 )
		memory->getMem(0xc054);  // Reset PAGE2
	else if( page==2 )
		memory->getMem(0xc055);  // Set PAGE2
	
}

void Cpu65c02::setMemoryPage( bool main )
{

	if( memory->getMem(0xc018)&0x80 ) {
		// 80-store is on - auxiliary memory writes are selected by PAGE2 switch
		// Sather 5-7
		if( main )
			memory->getMem(0x0c054);  // PAGE2 off - select main memory for reading/writing
		else 
			memory->getMem(0x0c055);  // PAGE2 on - select auxiliary memory for reading/writing
	}
	else {
		// 80-store is off - auxiliary memory accesses are selected by RAMWRT / RAMRD switches
		// Sather 5-22
		if( main ) {
			memory->putMem(0xc002, 0x00);  // RAMRD off - select main memory for reading
			memory->putMem(0xc004, 0x00);  // RAMWRT off - select main memory for writing
		}
		else {
			memory->putMem(0xc003, 0x00);  // RAMRD on - select auxiliary memory for reading
			memory->putMem(0xc005, 0x00);  // RAMWRT on - select auxiliary memory for writing
		}
	}
	
}

void Cpu65c02::setOriginalText()
{
	memory->putMem(0xc00e, 0x00);
}
		
void Cpu65c02::setMouseText()
{
	memory->putMem(0xc00f, 0x00);
}

void Cpu65c02::clearDisplay( int page )
{

	if( page < 1 || page > 2 ) {
		clearDisplay(1);
		clearDisplay(2);
		return;
	}

	// Turn 80STORE off to enable visible page flipping
	memory->putMem(0xc000, 0x00);

	if( ( memory->getMem(0xc01d) & 0x80 ) && !( memory->getMem(0xc01a) & 0x80 ) ) {
	
		// Hires on and text mode off
	
		if( memory->getMem(0xc01f) & 0x80)  // 80-col mode
			for( int y = 0; y<192; y++ )
				for( int x = 0; x<80; x++ )
					putByteHi80(page, y, x, 0);
		else  // 40-col mode
			for( int y = 0; y<192; y++ )
				for( int x = 0; x<40; x++ )
					putByteHi40(page, y, x, 0);
		
	}
	else {
	
		// Text or lores graphics mode on
		
		Uint8 clearByte = ( memory->getMem(0xc01a) & 0x80 ) ? 0xa0 : 0x00;  // TEXT ? $A0 : $00
		
		if( memory->getMem(0xc01f) & 0x80)  // 80-col mode
			for( int y = 0; y<24; y++ )
				for( int x = 0; x<80; x++ )
					putByteLo80(page, y, x, clearByte);
		else  // 40-col mode
			for( int y = 0; y<24; y++ )
				for( int x = 0; x<40; x++ )
					putByteLo40(page, y, x, clearByte);
	}
	
	clearPanel(page);

}

void Cpu65c02::clearPanel( int page )
{

	if( page < 1 || page > 2 ) {
		clearPanel(1);
		clearPanel(2);
		return;
	}

	if( memory->getMem(0xc01b) & 0x80 ) {
	
		// Mixed-mode on
		
		if( memory->getMem(0xc01f) & 0x80)         // 80-col mode
			for( int y = 20; y<24; y++ )
				for( int x = 0; x<80; x++ )
					putByteLo80(page, y, x, 0xa0);
		else                                       // 40-col mode
			for( int y = 20; y<24; y++ )
				for( int x = 0; x<40; x++ )
					putByteLo40(page, y, x, 0xa0);

	}
	
}
