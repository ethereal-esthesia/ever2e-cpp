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


#ifndef _MEMORY128K_H_
#define _MEMORY128K_H_

#include <iostream>
#include <fstream>
#include <cassert>
#include "cpu65c02.h"
#include "mon560x192.h"
#include "speaker1bit.h"
#include "keyboard2e.h"
#include "savestate.h"
#include "card16bit.h"


#define _MEMORY_TEST_OUTPUT	


class Memory128k
{

	static const int TOTAL_RAM_PAGES = 2;  // Main and auxilliary RAM supported

	Uint32 switchState;

	struct MemoryBlock
	{
		int startBlock;
		Uint8 (Memory128k::*readFunc)( Uint16 address );
		void (Memory128k::*writeFunc)( Uint16 address, Uint8 byte );
	};

	Uint8 ramPage64k[TOTAL_RAM_PAGES<<16];
	PeripheralCard16bit* slotCard[8];   // Pointers to cards in slots 1-7
	Uint8 rom16k[0x4000];  // 16k language card (LC) ROM 

	Uint8 (Memory128k::*getMemBlockFunc[0x100])( Uint16 address );
	void (Memory128k::*putMemBlockFunc[0x100])( Uint16 address, Uint8 byte );

	class Monitor560x192* monitor;
	class Cpu65c02* cpu;
	class Speaker1bit* speaker;
	class Keyboard2e* keyboard;

	unsigned int toggleMod;

	int accessCount;

	void _setMemoryLayout( const MemoryBlock *layout );

	Uint8 _readZpSt( Uint16 address );

	void _writeZpSt( Uint16 address, Uint8 byte );

	Uint8 _readMem( Uint16 address );

	void _writeMem( Uint16 address, Uint8 byte );

	Uint8 _readIo( Uint16 address );
	
	void _writeIo( Uint16 address, Uint8 byte );

	Uint8 _readSlot( Uint16 address );

	void _writeSlot( Uint16 address, Uint8 byte );

	Uint8 _readExpMem( Uint16 address );

	void _writeExpMem( Uint16 address, Uint8 byte );

	Uint8 _readUpMem( Uint16 address );

	void _writeUpMem( Uint16 address, Uint8 byte );

	void _writeNull( Uint16 address, Uint8 byte );

	Uint8 _randRead();
		// Generate random default read to simulate Apple IIe undefined address read
		
	Uint8 _randRead7bit();
		// Generate 7 low bits of random default read to simulate Apple IIe undefined address read
		
	void _commitSwitches();
	
public:

	Memory128k();

	~Memory128k();

	// Description: 6-1 and 7-2 of Sather
	// HRAM - Sather 5-23
	/// Add KEYSTROBE, KBD, AKD, PB0, PB1, PB2 ///
	static const Uint32 _80COL =      0x00000001;  // Double-resolution display
	static const Uint32 _ALTCHRSET =  0x00000002;  // Uses "mouse-text"
	static const Uint32 _TEXT =       0x00000004;  // All-text display
	static const Uint32 _MIXED =      0x00000008;  // Bottom panel display is text
	static const Uint32 _HIRES =      0x00000010;  // Selects hi-res graphics when in graphics mode
	static const Uint32 _AN0 =        0x00000020;  // Annunciator 0
	static const Uint32 _AN1 =        0x00000040;  // Annunciator 1
	static const Uint32 _AN2 =        0x00000080;  // Annunciator 2
	static const Uint32 _AN3 =        0x00000100;  // Annunciator 3 - video switching
	static const Uint32 _80STORE =    0x00000200;  // Force PAGE2 flag to select aux memory at 0x200-0xbfff
	static const Uint32 _RAMRD =      0x00000400;  // Read aux memory $200-$CFFF depending on 80STORE and HIRES switches
	static const Uint32 _RAMWRT =     0x00000800;  // Write aux memory $200-$CFFF depending on 80STORE and HIRES switches
	static const Uint32 _INTCXROM =   0x00001000;  // Switches $Cl00-$CFFF from internal to slot ROM
	static const Uint32 _ALTZP =      0x00002000;  // Read / write aux memory on zero page and stack as well as $D000-$FFFF RAM access
	static const Uint32 _SLOTC3ROM =  0x00004000;  // Switches $C3XX from slot ROM to internal ROM
	static const Uint32 _PAGE2 =      0x00008000;  // Selects visible display window if 80STORE is off
	static const Uint32 _BANK1 =      0x00010000;  // Switches $D000-$DFFF RAM accesses from $D000-$DFFF to $C000-$CFFF RAM addresses
	static const Uint32 _HRAMRD =     0x00020000;  // Switches reads at $D000-$FFFF from internal ROM to $C000-$FFFF RAM accesses
	static const Uint32 _PREWRITE =   0x00040000;  // Used by HRAMWRT to keep track of double-read accesses
	static const Uint32 _HRAMWRT =    0x00080000;  // Switches writes at $D000-$FFFF from internal ROM to $C000-$FFFF RAM accesses
	static const Uint32 _INTC8ROM =   0x00100000;  // Switches $C800-$CFFF from peripheral card I/O to internal ROM


	void coldReset();
		// Restores memory / switch state to status associated with a cold startup

	void warmReset();
		// Restores memory / switch state to status associated with a reset interrupt

	void setSwitch( Uint32 state );

	void resetSwitch( Uint32 state );

	bool getSwitch( Uint32 state );

	void putPeripheral( class Cpu65c02* cpu, class Monitor560x192* monitor, class Speaker1bit* speaker, class Keyboard2e* keyboard );

	void putSlot( int slot, PeripheralCard16bit* card );
		// "slot" is the slot number 1-7
		// "card" is the card object assigned to that slot

	void putMem( Uint16 address, Uint8 byte );
		// "poke" memory
		// "noCount" should be used by CPU or diagnostic routines only to combine multiple accesses and save on design complexity
	
	Uint8 getMem( Uint16 address );
		// "peek" memory
		// "noCount" should be used by CPU or diagnostic routines only to combine multiple accesses and save on design complexity

	void memAccess();
		// Should be used by CPU and hardware routines to simulate a RAM access request once per emulated access
		// This will cause an error if more than one access is made per cycle allowing proper hardware integration to be verified

	Uint8 getMemPassive( int page, Uint16 address );
		// Direct RAM read bypassing normal memory bus routines
		// This routine will not access ROM
		// Should only be used by video reads which are allowed unfettered access to RAM by the other IIe hardware
		//   or by diagnostic routines
		// Peripheral cards and CPU should only read memory with "getMem"

	void cycle();
		// Checks for memory access conflicts between CPU and peripheral cards
		// Only 1 access through getMem/putMem is allowed per cycle

	void store( SaveState &state );
		// Stores full memory state and bus switches to "state"
		// A backup state can be created beforehand if there is a chance that read data is corrupted
		//   either due to a version change, altered data, or a damaged file
		// A call to "restore" will indicate if there is any unreadable data

	int restore( SaveState &state );
		// Restores full memory state and bus switches from "state"
		// Returns 0 if successful
		// Returns 1 if state data is corrupted
		// Upon finding a read error, all state values are restored to their defaults
		
	void dumpMem();
	
};
	

#endif  // _MEMORY128K_H_
