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


#ifndef _CPU65C02_H_
#define _CPU65C02_H_


#include <iostream>
#include <iomanip>
#include <string>
#include <cstdlib>
#include <cassert>
#include <cmath>
#include "SDL.h"
#include "mon560x192.h"
#include "memory128k.h"


#define _CPU_TEST_OUTPUT	


class Cpu65c02
{

	class Memory128k* memory;

	// Registers
	Uint8   _A;   // Accumulator
	Uint8   _Y;   // Index registers y and x
	Uint8   _X;   
	Uint16  _PC;  // Program counter (PCL:PCH)
	Uint8   _S;   // Stack pointer (0x01NN)
	Uint8   _P;   // Processor status

	// Status Registers
	enum StatReg
	{
		_N = 0x80,    // N  Negative flag            - 0: last compare negative, 1: otherwise
		_V = 0x40,    // V  Overflow flag            - 0: no overflow on last result, 1: otherwise
		_1 = 0x20,    // 1  Unused                   - Always 1
		_B = 0x10,    // B  Break status             - 1: after BRK, 0: after IRQB
		_D = 0x08,    // D  Decimal mode             - 0: set by CLD, 1: set by SED
		_I = 0x04,    // I  Interrupt-disable flag   - 0: set by CLI, 1: set by SEI
		_Z = 0x02,    // Z  Zero flag                - 0: last result non-zero, 1: otherwise
		_C = 0x01     // C  Carry bit                - 0: no carry on last result or CLC, 1: carry on last result or SEC
	};

public:

	enum OpcodeMnemonic
	{
		_ADC, _AND, _ASL, _BCC, _BCS, _BEQ,
		_BIT, _BMI, _BNE, _BPL, _BRA, _BRK,
		_BVC, _BVS, _CLC, _CLD, _CLI, _CLV,
		_CMP, _CPX, _CPY, _DEA, _DEC, _DEX,
		_DEY, _EOR, _INA, _INC, _INX, _INY,
		_JMP, _JSR, _LDA, _LDX, _LDY, _LSR,
		_NOP, _ORA, _PHA, _PHP, _PHX, _PHY,
		_PLA, _PLP, _PLX, _PLY, _ROL, _ROR,
		_RTI, _RTS, _SBC, _SEC, _SED, _SEI,
		_STA, _STX, _STY, _STZ, _TAX, _TAY,
		_TRB, _TSB, _TSX, _TXA, _TXS, _TYA,

		// Pending interrupts
		_IRQ,  // Interrupt request (made by hardware)
		_NMI,  // Non-maskable interrupt (hardware failure)
		_RES,  // Reset interrupt (ctrl-reset / boot handler)
		_HLT,  // Halt CPU (non-standard - called by host application)

		// No instruction
		_NOI
	};

	enum AddressMode
	{
		_IMM,           _ABS,           _ZPG,
		_ACC,           _IMP,           _IND_X,
		_IND_Y,         _ZPG_X,         _ZPG_Y,
		_ABS_X,         _ABS_Y,         _REL,
		_ABS_IND,       _ABS_IND_X,     _ZPG_IND
	};

	static const int TABLE_IRQ = 0x100;
	static const int TABLE_NMI = 0x101;
	static const int TABLE_RES = 0x102;
	static const int TABLE_HLT = 0x103;
	
	static const int INT_NMI_VECTOR = 0xfffa;
	static const int INT_RES_VECTOR = 0xfffc;
	static const int INT_IRQ_VECTOR = 0xfffe;
	static const int INT_BRK_VECTOR = 0xfffe;

	enum TextType { ANY, NORMAL, INVERSE, FLASH } textType;
	
	// Common ASCII values
	static const Uint8 ASC_A =       0x41;
	static const Uint8 ASC_a =       0x61;
	static const Uint8 ASC_Z =       0x5a;
	static const Uint8 ASC_z =       0x7a;
	static const Uint8 ASC_0 =       0x30;
	static const Uint8 ASC_9 =       0x39;
	static const Uint8 ASC_BELL =    0x07;
	static const Uint8 ASC_LEFT =    0x08;
	static const Uint8 ASC_TAB =     0x09;
	static const Uint8 ASC_DOWN =    0x0a;
	static const Uint8 ASC_UP =      0x0b;
	static const Uint8 ASC_ENTER =   0x0d;
	static const Uint8 ASC_RIGHT =   0x15;
	static const Uint8 ASC_ESCAPE =  0x1b;
	static const Uint8 ASC_DELETE =  0x7f;
	
private:

	static const char OPCODE_NAME[66+4][5];

	static const char ADDRESS_MODE_NAME[15][13];

	enum OpcodeFlag {
		INST_CROSS_PAGE_INC = 0x01,
		INST_BRANCH_INC     = 0x02,
		INST_DEC_INC        = 0x04
	};

	struct OpcodeTable
	{
		Uint8 mnemonic;
		Uint8 addressMode;
		Uint8 instrSize;
		Uint8 cycleTime;
	};

	int multiplier;
	int cycleCount;
	int idleCycle;

	const OpcodeTable* opcode;
	static const OpcodeTable OPCODE_65C02[0x105];

	OpcodeMnemonic interruptPending;
	
	void _playSoundCycle();
	Sint32 soundTimer;
	int soundWaitLength;
	unsigned int soundDuration;
	
	void _cycle();
		// Complete 1 cycle of simulated 65C02 application

	Uint8 _getMem8( Uint16 address );

	Uint8 _getMemZeroPage8( Uint8 address );

	Uint16 _getMem16( Uint16 address );

	Uint16 _getMemZeroPage16( Uint8 address );

	Uint8 _pullStack();

	void _pushStack( Uint8 value );
		
	int _decodeTextByte( char c, TextType type, Uint8 & byte );

	int _putText40( int page, int row, int col, char c, TextType = ANY );
	
	int _putText40( int page, int row, int col, const std::string &str, TextType = ANY );
	
	int _putText80( int page, int row, int col, char c, TextType = ANY );
	
	int _putText80( int page, int row, int col, const std::string &str, TextType = ANY );
	
	int _pageTranslate( int page );
	
	Uint8 _transliterate( char c );
	
public:

	// Used to define clock rate and to sync other devices
	static const Sint32 CPU_TIMING_NANOSECONDS = 978;  // Sather 3-5

	Cpu65c02( class Memory128k* memory );

	~Cpu65c02();

	void setMultiplier( int multiplier );
		// Set speed of CPU to 1x, 2x, 3x, or 4x
		// This will not effect other hardware cycling or speaker pitch

	int getMultiplier();
	
	void interrupt( OpcodeMnemonic type );
		// Inserts an instruction to be executed at the end of the current instruction cycle
		// "type" must be one of the following:
		//   _RES - reset interrupt
		//   _IRQ - interrupt request
		//   _NMI - non-maskable interrupt
		//   _NOI - turn off 65C02 interrupt line

	void cycleSteal( int cycles );
		// Stop the CPU clock for given "cycles" (1-5)
		// In a real apple this allows peripheral cards to access memory without CPU conflict
		//   Wozniak suggests a maximum of 5 cycle pauses before the CPU may crash (Understanding the Apple by Jim Sather, section 4-12)
		// If 2 requests are made simultaneously, program terminates to highlight incompatibility
		/// need to double check real result ///

	void cycle();
		// Complete cycle of simulated 65C02 application

	std::string getOpcodeString();
		// Returns decoded opcode and operands
		
	std::string getRegisterString();
		// Returns list of all operands

	double noteToFreq( double p );

	void playSound( float freq = 936, float duration = 0.103 );
		// Plays sound regardless of other CPU functions currently running for the duration specified in seconds and frequency in MHz
		// Can be interrupted by subsequent calls or a call to "stopSound"
		// Defaults to standard Apple IIe beep (192 speaker toggles with a 546 cycle / 534 ns delay)

	void stopSound();
		// Stops forced sound playback

	bool isPlayingSound();
		// Indicates if previously initiated sound has finished playing

	// The following routines use Apple IIe standard I/O memory writes to directly change the emulation state
	// If the alterations are expected to be temporary, the state should be saved and reloaded after these routines are called

	int putText( int page, int row, int col, char c, TextType = ANY );
		// Returns the number of displayed characters (either 0 or 1)
		// Application terminates with an error message if "row" or "col" falls outside of the current addressable window
		// The top / left corner is at position 0, 0
		// The start of the text field in split graphics/text mode is located at rows 20-23
		// "type" can be used to override the default display type:
		//   ANY      default display type
		//   NORMAL   white text on black background
		//   INVERSE  black text on white background
		//            If inverse lower case characters are available (mouse-text mode), they are used
		//              otherwise capital letters are substituted
		//   FLASH    flashing characters (or nearest equivelant flashing character) are used when in "original" text mode vs. "mouse" text
		//            when in "mouse" text mode, the following characters are used:
		//              A/a: open apple
		//              B/b: mouse pointer - arrow
		//              C/c: mouse pointer - hour glass
		//              D/d: check-mark
		//              E/e: inverse check-mark
		//              F/f: man running - left half
		//              G/g: man running - right half
		//              H/h: left arrow
		//              I/i: ellipsis
		//              J/j: down arrow
		//              K/k: up arrow
		//              L/l: border - top
		//              M/m: carriage return
		//              N/n: inverse border - right / inverse custom text cursor
		//              O/o: scroll bar - left arrowhead
		//              P/p: scroll bar - right arrowhead
		//              Q/q: scroll bar - down arrowhead
		//              R/r: scroll bar - up arrowhead
		//              S/s: border - centered horizontal
		//              T/t: border - bottom left corner
		//              U/u: right arrow
		//              V/v: 50% fill - even column
		//              W/w: 50% fill - odd column
		//              X/x: folder - left half
		//              Y/y: folder - right half
		//              Z/z: border - right (used in vertical scroll bar) / custom text cursor
		//              0:   diamond
		//              1:   border - top and bottom (used in horizontal scroll bar)
		//              2:   mouse pointer - cross
		//              3:   scroll bar - scroll thumb
		//              4:   border - left
		//              5:   closed apple
		// The following escape sequences have the following effects:
		//   '\001' - change default to normal text
		//   '\002' - change default to inverse text
		//   '\003' - change default to flashing / mouse text
		//   '\007' - trigger a bell sound
		// Other escape sequences are reserved / ignored
	
	int putText( int page, int row, int col, const std::string &str, TextType type = ANY );
		// Returns the number of displayed characters
		// See overloaded "putText" function above for detailed description
	
	void putLores( int page, int x, int y, Uint8 color );
		// x is a value from 0 to 39
		// y is a value from 0 to 47
		// Colors values are from 0 to 15
		//   ind  apple        categorical
		//   -----------------------------------------------------------------
		//     0  black        black
		//     1  magenta      dark magenta
		//     2  dark blue    dark blue
		//     3  purple       medium violet (dark magenta + dark blue)
		//     4  dark green   dark green
		//     5  grey         medium grey (dark magenta + dark green)
		//     6  medium blue  medium teal (dark blue + dark green)
		//     7  light blue   pastel blue
		//     8  brown        dark yellow (brown)
		//     9  orange       medium orange (dark magenta + dark yellow)
		//    10  grey         medium grey (dark blue + dark yellow)
		//    11  pink         pastel magenta (pink)
		//    12  green        medium apple green (dark green + dark yellow)
		//    13  yellow       pastel yellow
		//    14  aqua         pastel green
		//    15  white        white

	void putDoubleLores( int page, int x, int y, Uint8 color );
		// x is a value from 0 to 80
		// y is a value from 0 to 48
		// Colors values are from 0 to 15
		//   ind  apple        categorical
		//   -----------------------------------------------------------------
		//     0  black        black
		//     1  magenta      dark magenta
		//     2  dark blue    dark blue
		//     3  purple       medium violet (dark magenta + dark blue)
		//     4  dark green   dark green
		//     5  grey         medium grey (dark magenta + dark green)
		//     6  medium blue  medium teal (dark blue + dark green)
		//     7  light blue   pastel blue
		//     8  brown        dark yellow (brown)
		//     9  orange       medium orange (dark magenta + dark yellow)
		//    10  grey         medium grey (dark blue + dark yellow)
		//    11  pink         pastel magenta (pink)
		//    12  green        medium apple green (dark green + dark yellow)
		//    13  yellow       pastel yellow
		//    14  aqua         pastel green
		//    15  white        white

	void putHiresMono( int page, int x, int y, bool color, int palette );
		// x is a value from 0 to 280
		// y is a value from 0 to 192
		// Colors values are from 0 to 1
		//   0 black
		//   1 color
		// Palette values are from 0 to 2
		//   0 no palette change
		//   1 purple/green palette
		//   2 red/blue palette
		// Adjusts palette as needed for neighboring pixel block if pixel lies on a border

	void putHiresColor( int page, int x, int y, int color );
		// Colors are from 0 to 5
		// x is a value from 0 to 140
		// y is a value from 0 to 192
		//   ind  apple            categorical
		//   -----------------------------------------
		//     0  black (0/4)      back
		//     1  purple (1)       medium violet
		//     2  green (2)        medium apple green
		//     3  medium blue (5)  medium teal
		//     4  orange (6)       medium orange
		//     5  white (3/7)      white
		// Resolves palette conflicts by placing preference on the most recent write
		// White or black writes do not alter the palette
		// Adjusts palette as needed for neighboring pixel block if pixel lies on a border

	void putDoubleHiresMono( int page, int x, int y, bool color );
		// x is a value from 0 to 560
		// y is a value from 0 to 192
		// Colors values are from 0 to 1
		//   0 black
		//   1 color
	
	void putDoubleHiresColor( int page, int x, int y, int color );
		// x is a value from 0 to 140
		// y is a value from 0 to 192
		// Colors values are from 0 to 15
		//   ind  apple        categorical
		//   -----------------------------------------------------------------
		//     0  black        black
		//     1  magenta      dark magenta
		//     2  dark blue    dark blue
		//     3  purple       medium violet (dark magenta + dark blue)
		//     4  dark green   dark green
		//     5  grey         medium grey (dark magenta + dark green)
		//     6  medium blue  medium teal (dark blue + dark green)
		//     7  light blue   pastel blue
		//     8  brown        dark yellow (brown)
		//     9  orange       medium orange (dark magenta + dark yellow)
		//    10  grey         medium grey (dark blue + dark yellow)
		//    11  pink         pastel magenta (pink)
		//    12  green        medium apple green (dark green + dark yellow)
		//    13  yellow       pastel yellow
		//    14  aqua         pastel green
		//    15  white        white

	void putByteLo40( int page, int row, int col, Uint8 byte );

	void putByteHi40( int page, int row, int col, Uint8 byte );

	void putByteLo80( int page, int row, int col, Uint8 byte );

	void putByteHi80( int page, int row, int col, Uint8 byte );

	Uint8 getByteLo40( int page, int row, int col );

	Uint8 getByteHi40( int page, int row, int col );

	Uint8 getByteLo80( int page, int row, int col );

	Uint8 getByteHi80( int page, int row, int col );

	void setModeText40();
		// Turns 40-column text mode on in full-screen mode
		// Clears visible window for page 1 and 2

	void setModeText80();
		// Turns 80-column text mode on in full-screen mode
		// Clears visible window for page 1 and 2

	void setModeLores();
		// Turns lo-res graphics on in full-screen mode
		// Clears visible window for page 1 and 2

	void setModeDoubleLores();
		// Turns double lo-res graphics on in full-screen mode
		// Clears visible window for page 1 and 2

	void setModeHires();
		// Turns hi-res graphics on in full-screen mode
		// Clears visible window for page 1 and 2
	
	void setModeDoubleHires();
		// Turns double hi-res graphics on in full-screen mode
		// Clears visible window for page 1 and 2
	
	void setModeSplitScreen();
		// Turns split graphics/text screen on
		// Must be called after graphics window is set
		// Clears visible window for page 1 and 2

	void setModeFullScreen();
		// Turns split graphics/text screen off
		// Must be called after graphics window is set
		// Clears visible window for page 1 and 2

	void setPage( int page );
		// Sets emulated display page to "page" if "page" is 1 or 2
		// If "page" is 0, page is left unchanged
		// Other values for "page" are reserved and have no effect

	void setMemoryPage( bool main = true );
		// Sets emulated memory page to main memory when "main" is true
		// Otherwise, sets auxilliary memory writes

	void setOriginalText();
		// Switches to original Apple II flashing text font (prior to mouse-text enhancement)
		
	void setMouseText();
		// Switches to Apple IIe mouse-text font
		
	void clearDisplay( int page = 0 );
		// Clears display page specified according to current graphics and text settings
		// If "page" is invalid, not specified, or 0, both display pages are cleared

	void clearPanel( int page = 0 );
		// Clears display panel on page specified according to current graphics and text settings when in mixed mode
		// If "page" is invalid, not specified, or 0, both display pages are cleared

};


#endif /* _CPU65C02_H_ */
