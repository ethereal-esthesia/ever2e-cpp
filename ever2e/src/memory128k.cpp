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
 

#include "memory128k.h"


/// Turn this into a giant switch statment for significant speedups


using namespace std;

void Memory128k::setSwitch( Uint32 state )
{
	switchState = Uint32 (switchState | state);
	_commitSwitches();
}

void Memory128k::resetSwitch( Uint32 state )
{
	switchState = Uint32 ( switchState & (~state) );
	_commitSwitches();
}

void Memory128k::coldReset()
{

	// Load language card (LC) ROM from file
	switchState = Uint32(0);
	_commitSwitches();

	ifstream romFile;

	const char fileName[] = "apple2e.rom";

	romFile.open(fileName, ios::in | ios::binary);
	if( !romFile ) {
		cerr << "Could not read " << fileName << endl;
		exit(1);
	}
	if( !romFile.read((char *)rom16k, sizeof(rom16k) ) ) {
		cerr << "Incorrect file length: " << fileName << endl;
		exit(1);
	}
	romFile.close();

	// Clear RAM to inital state (fill with repeating "FF FF 00 00")
	/// Double check that auxiliary RAM is initialized entirely to 0's
	int i = 0;
	for( ; i<0x4000; i++ )
		( (Uint32 *) ramPage64k )[i] = 0x0000ffff;
	for( ; i<(int)sizeof(ramPage64k)>>2; i++ )
		( (Uint32 *) ramPage64k )[i] = 0x00000000;

	// Set initial peripheral ROM state
	for( int i = 0; i<8; i++ )
		slotCard[i] = NULL;

	accessCount = 0;
	toggleMod = 0;
	
}

void Memory128k::warmReset()
{
	// _TEXT and _MIXED statuses are not modified by a reset interrupt
	switchState &= _TEXT | _MIXED;  // Sather 7-3, Sather I-5 suggests the Apple II reset operates differently than the Apple IIe
	_commitSwitches();
}

bool Memory128k::getSwitch( Uint32 state )
{
	return (switchState & state) && 1;
}

void Memory128k::_setMemoryLayout( const MemoryBlock *layout )
{
	int i = 0;
	do {
		for( int k = layout[i].startBlock; k<layout[i+1].startBlock; k++ ) {
			getMemBlockFunc[k] = layout[i].readFunc;
			putMemBlockFunc[k] = layout[i].writeFunc;
		}
		i++;
	} while ( layout[i].startBlock<0x100 );
}

Uint8 Memory128k::_readZpSt( Uint16 address )
{
	// Zero page and stack
	if( getSwitch(_ALTZP) )
		return ramPage64k[0x10000|address];
	else
		return ramPage64k[address];
}

void Memory128k::_writeZpSt( Uint16 address, Uint8 byte )
{
	// Zero page and stack
	if( getSwitch(_ALTZP) )
		ramPage64k[0x10000|address] = byte;
	else
		ramPage64k[address] = byte;
}

Uint8 Memory128k::_readMem( Uint16 address )
{

	// Address RAM 0x200-0xbfff

	// Sather 5-25:
	//   If 80STORE is set, RAMRD and RAMWRT do not affect $400-$7FF
	//   If 80STORE and HIRES are both set, RAMRD and RAMWRT do not affect $400-$7FF or $2000-$3FFF
	// Otherwise the PAGE2 flag is used to indicate auxiliary memory should be used (Sather 5-7, 5-22)
	
	bool auxRead;
	if( getSwitch(_80STORE) ) {
		if( ( address>=0x400 && address<0x800 ) || ( getSwitch(_HIRES) && address>=0x2000 && address<0x4000 ) )
			auxRead = getSwitch(_PAGE2);
		else
			auxRead = getSwitch(_RAMRD);
	}
	else
		auxRead = getSwitch(_RAMRD);

	if( auxRead )
		return ramPage64k[0x10000|address];
	else
		return ramPage64k[address];

}

void Memory128k::_writeMem( Uint16 address, Uint8 byte )
{
	// Address RAM 0x200-0xbfff

	// Sather 5-25:
	//   If 80STORE is set, RAMRD and RAMWRT do not affect $400-$7FF
	//   If 80STORE and HIRES are both set, RAMRD and RAMWRT do not affect $400-$7FF or $2000-$3FFF	
	// Otherwise the PAGE2 flag is used to indicate auxiliary memory should be used (Sather 5-7, 5-22)
	
	bool auxWrite;
	if( getSwitch(_80STORE) ) {
		if( ( address>=0x400 && address<0x800 ) || ( getSwitch(_HIRES) && address>=0x2000 && address<0x4000 ) )
			auxWrite = getSwitch(_PAGE2);
		else
			auxWrite = getSwitch(_RAMWRT);
	}
	else
		auxWrite = getSwitch(_RAMWRT);

	if( auxWrite )
		ramPage64k[0x10000|address] = byte;
	else
		ramPage64k[address] = byte;

}

Uint8 Memory128k::_readIo( Uint16 address )
{

	// Activate software switch

	// See Sather 2-13
	
	switch( address )
	{

		case 0xc000:
			// KEYBOARD
			// Bit 7 indicates key was pressed
			// Bit 0-6 indicate ASCII code
			return keyboard->getKeyboard();

		case 0xc010:
			// STROBE
			// Resets key strobe
			// IIe and later
			// Bit 7 indicates key is currently down
			// Bit 0-6 indicate ASCII code
			keyboard->putStrobe();
			return keyboard->getStrobe();

		case 0xc011:
			// Read inverted BANK1
			return keyboard->getKeyboard() & 0x7f | ((!getSwitch(_BANK1))<<7);
			
		case 0xc012:
			// Read HRAMRD
			return keyboard->getKeyboard() & 0x7f | (getSwitch(_RAMRD)<<7);
			
		case 0xc013:
			// Read RAMRD
			return keyboard->getKeyboard() & 0x7f | (getSwitch(_RAMRD)<<7);

		case 0xc014:
			// Read RAMWRT
			return keyboard->getKeyboard() & 0x7f | (getSwitch(_RAMWRT)<<7);
			
		case 0xc015:
			// Read INTCXROM
			return keyboard->getKeyboard() & 0x7f | (getSwitch(_INTCXROM)<<7);
			break;
			
		case 0xc016:
			// Read ALTZP
			return keyboard->getKeyboard() & 0x7f | (getSwitch(_ALTZP)<<7);
			
		case 0xc017:
			// Read SLOTC3ROM
			return keyboard->getKeyboard() & 0x7f | (getSwitch(_SLOTC3ROM)<<7);
			
		case 0xc018:
			// Read 80STORE
			return keyboard->getKeyboard() & 0x7f | (getSwitch(_80STORE)<<7);
			
		case 0xc019:
			// Read VBL
			return keyboard->getKeyboard() & 0x7f | (monitor->getVbl()<<7);
			
		case 0xc01a:
			// Read TEXT
			return keyboard->getKeyboard() & 0x7f | (getSwitch(_TEXT)<<7);
			
		case 0xc01b:
			// Read MIXED
			return keyboard->getKeyboard() & 0x7f | (getSwitch(_MIXED)<<7);
			
		case 0xc01c:
			// Read PAGE2
			return keyboard->getKeyboard() & 0x7f | (getSwitch(_PAGE2)<<7);
			
		case 0xc01d:
			// Read HIRES
			return keyboard->getKeyboard() & 0x7f | (getSwitch(_HIRES)<<7);
			
		case 0xc01e:
			// Read ALTCHARSET
			return keyboard->getKeyboard() & 0x7f | (getSwitch(_ALTCHRSET)<<7);

		case 0xc01f:
			// Read 80COL
			return keyboard->getKeyboard() & 0x7f | (getSwitch(_80COL)<<7);

		case 0xc030:
		case 0xc031:
		case 0xc032:
		case 0xc033:
		case 0xc034:
		case 0xc035:
		case 0xc036:
		case 0xc037:
		case 0xc038:
		case 0xc039:
		case 0xc03a:
		case 0xc03b:
		case 0xc03c:
		case 0xc03d:
		case 0xc03e:
		case 0xc03f:
			// SPEAKER
			// Toggle speaker diaphragm, less any CPU multiplier to avoid altering pitch on a faster CPU setting
			if( (toggleMod++)%cpu->getMultiplier()==0 )
				speaker->toggle();
			return _randRead();

		case 0xc050:
			// Reset TEXT
			resetSwitch(_TEXT);
			return _randRead();

		case 0xc051:
			// Set TEXT
			setSwitch(_TEXT);
			return _randRead();

		case 0xc052:
			// Reset MIXED
			resetSwitch(_MIXED);
			return _randRead();

		case 0xc053:
			// Set MIXED
			setSwitch(_MIXED);
			return _randRead();

		case 0xc054:
			// Reset PAGE2
			resetSwitch(_PAGE2);
			return _randRead();

		case 0xc055:
			// Set PAGE2
			setSwitch(_PAGE2);
			return _randRead();

		case 0xc056:
			// Reset HIRES
			resetSwitch(_HIRES);
			return _randRead();

		case 0xc057:
			// Set HIRES
			setSwitch(_HIRES);
			return _randRead();

		case 0xc058:
			// Reset AN0
			resetSwitch(_AN3);
			return _randRead();

		case 0xc059:
			// Set AN0
			setSwitch(_AN3);
			return _randRead();

		case 0xc05a:
			// Reset AN1
			resetSwitch(_AN3);
			return _randRead();

		case 0xc05b:
			// Set AN1
			setSwitch(_AN3);
			return _randRead();

		case 0xc05c:
			// Reset AN2
			resetSwitch(_AN3);
			return _randRead();

		case 0xc05d:
			// Set AN2
			setSwitch(_AN3);
			return _randRead();

		case 0xc05e:
			// Reset AN3
			resetSwitch(_AN3);
			return _randRead();

		case 0xc05f:
			// Set AN3
			setSwitch(_AN3);
			return _randRead();

		case 0xc061:
		case 0xc069:
			// OPNAPPLE / PB0
			// Bit 7 is used to indicate if open apple or PB0 game button is pressed
			// Bits 0-6 are undefined
			return _randRead7bit() | ( (keyboard->getOpenApple() ) << 7 );; /// || joystick->getButton(0)
	
		case 0xc062:
		case 0xc06a:
			// CLSAPPLE / PB1
			// Bit 7 is used to indicate if open apple or PB1 game button is pressed
			// Bits 0-6 are undefined
			return _randRead7bit() | ( (keyboard->getClosedApple()) << 7 ); /// || joystick->getButton(1)

		case 0xc063:
		case 0xc06b:
			// PB2
			// Bit 7 is used to indicate if PB2 game button is pressed
			// Bits 0-6 are undefined
			/// return  joystick->getButton(1)) << 7 );
#ifdef _MEMORY_TEST_OUTPUT	
goto warningStub; ///
#endif
			break;
				
		case 0xc080:
			// Reset BANK1
			resetSwitch(_BANK1);
			// Set HRAMRD
			setSwitch(_HRAMRD);
			// Reset PREWRITE
			resetSwitch(_PREWRITE);
			// Reset HRAMWRT
			resetSwitch(_HRAMWRT);
			return _randRead();
			
		case 0xc081:
			// Reset BANK1
			resetSwitch(_BANK1);
			// Reset HRAMRD
			resetSwitch(_HRAMRD);
			// Set PREWRITE
			// Set HRAMWRT if PREWRITE is set / otherwise set PREWRITE
			if( getSwitch(_PREWRITE) )
				setSwitch(_HRAMWRT);
			else
				setSwitch(_PREWRITE);
			return _randRead();
			
		case 0xc082:
			// Reset BANK1
			resetSwitch(_BANK1);
			// Reset HRAMRD
			resetSwitch(_HRAMRD);
			// Reset PREWRITE
			resetSwitch(_PREWRITE);
			// Reset HRAMWRT
			resetSwitch(_HRAMWRT);
			return _randRead();
			
		case 0xc083:
			// Reset BANK1
			resetSwitch(_BANK1);
			// Set HRAMRD
			setSwitch(_HRAMRD);
			// Set HRAMWRT if PREWRITE is set / otherwise set PREWRITE
			if( getSwitch(_PREWRITE) )
				setSwitch(_HRAMWRT);
			else
				setSwitch(_PREWRITE);
			return _randRead();
			
		case 0xc084:
			// Reset BANK1
			resetSwitch(_BANK1);
			// Set HRAMRD
			setSwitch(_HRAMRD);
			// Reset PREWRITE
			resetSwitch(_PREWRITE);
			// Reset HRAMWRT
			resetSwitch(_HRAMWRT);
			return _randRead();
			
		case 0xc085:
			// Reset BANK1
			resetSwitch(_BANK1);
			// Reset HRAMRD
			resetSwitch(_HRAMRD);
			// Set HRAMWRT if PREWRITE is set / otherwise set PREWRITE
			if( getSwitch(_PREWRITE) )
				setSwitch(_HRAMWRT);
			else
				setSwitch(_PREWRITE);
			return _randRead();
			
		case 0xc086:
			// Reset BANK1
			resetSwitch(_BANK1);
			// Reset HRAMRD
			resetSwitch(_HRAMRD);
			// Reset PREWRITE
			resetSwitch(_PREWRITE);
			// Reset HRAMWRT
			resetSwitch(_HRAMWRT);
			return _randRead();
			
		case 0xc087:
			// Reset BANK1
			resetSwitch(_BANK1);
			// Set HRAMRD
			setSwitch(_HRAMRD);
			// Set HRAMWRT if PREWRITE is set / otherwise set PREWRITE
			if( getSwitch(_PREWRITE) )
				setSwitch(_HRAMWRT);
			else
				setSwitch(_PREWRITE);
			return _randRead();
			
		case 0xc088:
			// Set BANK1
			resetSwitch(_BANK1);
			// Set HRAMRD
			setSwitch(_HRAMRD);
			// Reset PREWRITE
			resetSwitch(_PREWRITE);
			// Reset HRAMWRT
			resetSwitch(_HRAMWRT);
			return _randRead();
			
		case 0xc089:
			// Set BANK1
			resetSwitch(_BANK1);
			// Reset HRAMRD
			resetSwitch(_HRAMRD);
			// Set HRAMWRT if PREWRITE is set / otherwise set PREWRITE
			if( getSwitch(_PREWRITE) )
				setSwitch(_HRAMWRT);
			else
				setSwitch(_PREWRITE);
			return _randRead();
			
		case 0xc08a:
			// Set BANK1
			resetSwitch(_BANK1);
			// Reset HRAMRD
			resetSwitch(_HRAMRD);
			// Reset PREWRITE
			resetSwitch(_PREWRITE);
			// Reset HRAMWRT
			resetSwitch(_HRAMWRT);
			return _randRead();
			
		case 0xc08b:
			// Set BANK1
			resetSwitch(_BANK1);
			// Set HRAMRD
			setSwitch(_HRAMRD);
			// Set HRAMWRT if PREWRITE is set / otherwise set PREWRITE
			if( getSwitch(_PREWRITE) )
				setSwitch(_HRAMWRT);
			else
				setSwitch(_PREWRITE);
			return _randRead();
			
		case 0xc08c:
			// Set BANK1
			resetSwitch(_BANK1);
			// Set HRAMRD
			setSwitch(_HRAMRD);
			// Reset PREWRITE
			resetSwitch(_PREWRITE);
			// Reset HRAMWRT
			resetSwitch(_HRAMWRT);
			return _randRead();
			
		case 0xc08d:
			// Set BANK1
			resetSwitch(_BANK1);
			// Reset HRAMRD
			resetSwitch(_HRAMRD);
			// Set HRAMWRT if PREWRITE is set / otherwise set PREWRITE
			if( getSwitch(_PREWRITE) )
				setSwitch(_HRAMWRT);
			else
				setSwitch(_PREWRITE);
			return _randRead();
			
		case 0xc08e:
			// Set BANK1
			resetSwitch(_BANK1);
			// Reset HRAMRD
			resetSwitch(_HRAMRD);
			// Reset PREWRITE
			resetSwitch(_PREWRITE);
			// Reset HRAMWRT
			resetSwitch(_HRAMWRT);
			return _randRead();

		case 0xc08f:
			// Set BANK1
			resetSwitch(_BANK1);
			// Set HRAMRD
			setSwitch(_HRAMRD);
			// Set HRAMWRT if PREWRITE is set / otherwise set PREWRITE
			if( getSwitch(_PREWRITE) )
				setSwitch(_HRAMWRT);
			else
				setSwitch(_PREWRITE);
			return _randRead();
			
		default:
		
#ifdef _MEMORY_TEST_OUTPUT	
warningStub: ///
#endif
			if( address >= 0xC090 ) {
#ifdef _MEMORY_TEST_OUTPUT	
				cerr << "Warning: Peripheral read not implemented at address " << hex << setw(2) << (int) address << "\n";
#endif
				return 0;
			}
			else {
				// Undefined switch
#ifdef _MEMORY_TEST_OUTPUT	
				cerr << "Warning: Read to switch " << hex << setw(2) << (int) address << " not handled\n";
#endif
				return 0;
			}
		
	}

}

void Memory128k::_writeIo( Uint16 address, Uint8 byte )
{

	/// Verify: writes perform a read as well ///

	// Activate software switch

	// See Sather 2-13
	
	switch( address )
	{

		case 0xc000:
			// Reset 80STORE
			resetSwitch(_80STORE);
			break;

		case 0xc001:
			// Set 80STORE
			setSwitch(_80STORE);
			break;

		case 0xc002:
			// Reset RAMRD
			resetSwitch(_RAMRD);
			break;

		case 0xc003:
			// Set RAMRD
			setSwitch(_RAMRD);
			break;
			
		case 0xc004:
			// Reset RAMWRT
			resetSwitch(_RAMWRT);
			break;

		case 0xc005:
			// Set RAMWRT
			setSwitch(_RAMWRT);
			break;
			
		case 0xc006:
			// Reset INTCXROM
			resetSwitch(_INTCXROM);
			break;

		case 0xc007:
			// Set INTCXROM
			setSwitch(_INTCXROM);
			break;

		case 0xc008:
			// ALTZP Off - use primary zero page and stack
			resetSwitch(_ALTZP);
			break;

		case 0xc009:
			// ALTZP On - use auxiliary zero page and stack
			setSwitch(_ALTZP);
			break;

		case 0xc00a:
			// Reset SLOTC3ROM
			resetSwitch(_SLOTC3ROM);
			break;

		case 0xc00b:
			// Set SLOTC3ROM
			setSwitch(_SLOTC3ROM);
			break;

		case 0xc00c:
			// Reset 80COL
			resetSwitch(_80COL);
			break;

		case 0xc00d:
			// Set 80COL
			setSwitch(_80COL);
			break;

		case 0xc00e:
			// Reset ALTCHARSET
			resetSwitch(_ALTCHRSET);
			break;

		case 0xc00f:
			// Set ALTCHARSET
			setSwitch(_ALTCHRSET);
			break;

		case 0xc010:
			// STROBE
			// Clear the high bit in $C000 (KEYBOARD)
			keyboard->putStrobe();
			break;

		case 0xc051:
			// Set TEXT
			setSwitch(_TEXT);
			break;

		case 0xc080:
		case 0xc081:
		case 0xc082:
		case 0xc083:
		case 0xc084:
		case 0xc085:
		case 0xc086:
		case 0xc087:
		case 0xc088:
		case 0xc089:
		case 0xc08a:
		case 0xc08b:
		case 0xc08c:
		case 0xc08d:
		case 0xc08e:
		case 0xc08f:
			// Reset PREWRITE
			resetSwitch(_PREWRITE);
			break;
		
		default:
			// Possibly a read / write switch
			/// Include provisions for double read etc in CPU and do away with this line ///
			_readIo(address);
			break;
		
	}

}
			
Uint8 Memory128k::_readSlot( Uint16 address )
{

	int slot = (address-0xc000)>>8;
	
	if( slot==3 ) {	
	
		// $C3XX

		// Sather 5-28
		// INTC8ROM - Set by access to $C3XX with SLOTC3ROM reset
		//            Reset by access to $CFFF or 'RESET
		if( !getSwitch(_SLOTC3ROM) )
			setSwitch(_INTC8ROM);
		
		if( getSwitch(_INTCXROM) || !getSwitch(_SLOTC3ROM) ) {
		
			// Internal ROM at $C3XX
			return rom16k[address-0xc000];
			
		}
		else {
		
			// Peripheral card ROM at $C3XX
			if( slotCard[3] != NULL )
				return slotCard[slot]->getMem256b(address&0x00ff);
			else {
#ifdef _MEMORY_TEST_OUTPUT	
				cerr << "Warning: invalid read from peripheral memory at " << hex << setw(4) << (int) address << " [ " << cpu->getOpcodeString() << " ] \n";
#endif	
				return _randRead();
				return 0;
			}
			
		}
		
	}
	
	else {

		// $C1XX - $C2XX / $C4XX - $C7XX
		
		if( getSwitch(_INTCXROM) )
			// Internal ROM at $CNXX
			return rom16k[address-0xc000];
		else {
			// Peripheral card ROM at $CNXX
			if( slotCard[slot] != NULL )
				return slotCard[slot]->getMem256b(address&0x00ff);
			else {
#ifdef _MEMORY_TEST_OUTPUT	
				cerr << "Warning: invalid read from peripheral memory at " << hex << setw(4) << (int) address << " [ " << cpu->getOpcodeString() << " ] \n";
#endif	
				return _randRead();
			}
		}
	}
	
}

void Memory128k::_writeSlot( Uint16 address, Uint8 byte )
{		
	// NOP
	_readSlot(address);	/// 
}
	
Uint8 Memory128k::_readExpMem( Uint16 address )
{ 

	// Read from expansion memory 0xc800-0xcffe / soft-switch 0xcfff
	
	// Sather 5-28
	// INTC8ROM - Set by access to $C3XX with SLOTC3ROM reset
	//            Reset by access to $CFFF or 'RESET	
	//            Grants access to internal ROM at $C800-$CFFF

	if( address==0xcfff )
		resetSwitch(_INTC8ROM);
		
	if( getSwitch(_INTC8ROM) || getSwitch(_INTCXROM) )
		return rom16k[address-0xc000];

	/// STUB ///
	
#ifdef _MEMORY_TEST_OUTPUT	
	cerr << "Warning: read from expansion memory at " << hex << setw(4) << (int) address << endl;
#endif
	
	return 0;
	
/*
		
	/// Reading locations 0xcN00-0xcNff will enable the block designated to slot N for reading
	/// Reading 0xcfff disables reading 0xc800-0xcfff for all cards and instead directs reads to the system ROM

	else {
		if( false )//// STUB - LC ROM switch on
			return rom16k[address-0xc000];
		else {
			if( false )//// internal slot 3 test logic
				return expRom2k[0][address&0x7ff];  // Internal slot 3 ROM
			else
				/// STUB - N should be current slot pointer or ??? for no slot selected
				return expRom2k[N][address&0x07ff];
		}
	}
*/
}

void Memory128k::_writeExpMem( Uint16 address, Uint8 byte )
{
	// NOP
	_readExpMem(address);	/// 
}

Uint8 Memory128k::_readUpMem( Uint16 address )
{

	// $D000-$DFFF
	// Sather 5-12

	if( getSwitch(_HRAMRD) ) {

		if( getSwitch(_ALTZP) ) {
			if( address<0xe000 && getSwitch(_BANK1) )
				return ramPage64k[0x10000|(address-0x1000)];
			else
				return ramPage64k[0x10000|address];
		}
		else {
			if( address<0xe000 && getSwitch(_BANK1) )
				return ramPage64k[address-0x1000];
			else
				return ramPage64k[address];
		}
			
	}
	else
		return rom16k[address-0xc000];

}

void Memory128k::_writeUpMem( Uint16 address, Uint8 byte )
{

	// $D000-$DFFF
	// Sather 5-12
	
	// Write banked RAM or ignore write to system ROM

	if( getSwitch(_HRAMWRT) ) {

		if( getSwitch(_ALTZP) ) {
			if( address<0xe000 && getSwitch(_BANK1) )
				ramPage64k[0x10000|(address-0x1000)] = byte;
			else
				ramPage64k[0x10000|address] = byte;
		}
		else {
			if( address<0xe000 && getSwitch(_BANK1) )
				ramPage64k[address-0x1000] = byte;
			else
				ramPage64k[address] = byte;
		}

	}

}

void Memory128k::_writeNull( Uint16 address, Uint8 byte )
{
}

Uint8 Memory128k::_randRead()
{
	/// STUB ///
	/// See Sather 5-29, 5-40 for possible values
	return 0x00;
}

Uint8 Memory128k::_randRead7bit()
{
	/// STUB ///
	/// See Sather 5-29, 5-40 for possible values
	return _randRead()&0x7f;
}

/// A separate command could be used to dump ROM ///

void Memory128k::dumpMem()
{

	// Get/set cout settings
	ios_base::fmtflags coutFlags = cout.flags();	
	cout << hex << uppercase << setfill ('0');

	// Read main memory
	putMem(0xc000, 0x00);  // 80STORE off
	putMem(0xc002, 0x00);  // RAMRD off
	putMem(0xc008, 0x00);  // ALTZP off

	for( int page = 0; page<TOTAL_RAM_PAGES; page++ ) {

		cout << "RAM PAGE " << page << endl;

		Uint16 addr = 0x0000;
		for( int b = 0; b<0x100; b++ ) {
			cout << endl;
			cout << "ADDR   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n";
			for( int l = 0; l<0x10; l++ ) {
				cout << setw(4) << addr << ": ";
				for( int c = 0; c<0x10; c++ )
					cout << setw(2) << (int) ramPage64k[(page<<16)|addr++] << " ";
				cout << endl;
			}
		}

		cout << endl << endl;

	}

	// Restore cout settings
	cout.flags(coutFlags);

}

void Memory128k::_commitSwitches()
{
	if( monitor!=NULL )
		monitor->commitSwitches();
}
	
Memory128k::Memory128k()
{

	// See Sather 5-26 for basic memory layout

	this->monitor = NULL;
	const MemoryBlock MEMORY_LAYOUT[] = 
	{
		{ 0x00,   &Memory128k::_readZpSt,    &Memory128k::_writeZpSt   },  // Zero-page and stack
		{ 0x02,   &Memory128k::_readMem,     &Memory128k::_writeMem    },  // RAM
		{ 0x04,   &Memory128k::_readMem,     &Memory128k::_writeMem    },  // Text and low-resolution graphics RAM page 1
		{ 0x08,   &Memory128k::_readMem,     &Memory128k::_writeMem    },  // Text and low-resolution graphics RAM page 2
		{ 0x0c,   &Memory128k::_readMem,     &Memory128k::_writeMem    },  // RAM
		{ 0x20,   &Memory128k::_readMem,     &Memory128k::_writeMem    },  // High-resolution graphics RAM page 1
		{ 0x40,   &Memory128k::_readMem,     &Memory128k::_writeMem    },  // High-resolution graphics RAM page 2
		{ 0x60,   &Memory128k::_readMem,     &Memory128k::_writeMem    },  // RAM
		{ 0xc0,   &Memory128k::_readIo,      &Memory128k::_writeIo     },  // I/O switches
		{ 0xc1,   &Memory128k::_readSlot,    &Memory128k::_writeSlot   },  // System ROM / Peripheral rom
		{ 0xc8,   &Memory128k::_readExpMem,  &Memory128k::_writeExpMem },  // System ROM / Peripheral expansion ROM
		{ 0xd0,   &Memory128k::_readUpMem,   &Memory128k::_writeUpMem  },  // System ROM / RAM / banked RAM
		{ 0x100,  NULL,                      NULL                      }   // End of list
	};
	_setMemoryLayout(MEMORY_LAYOUT);
	coldReset();
}

void Memory128k::putPeripheral( Cpu65c02* cpu, Monitor560x192* monitor, Speaker1bit* speaker, Keyboard2e* keyboard )
{
	this->cpu = cpu;
	this->monitor = monitor;
	this->speaker = speaker;
	this->keyboard = keyboard;
	coldReset();
}

Memory128k::~Memory128k()
{
#ifdef _MEMORY_TEST_OUTPUT	
	dumpMem();
#endif
}

void Memory128k::putSlot( int slot, PeripheralCard16bit* card )
{
	assert( (unsigned int) slot<8 );
	slotCard[slot] = card;
}

void Memory128k::putMem( Uint16 address, Uint8 byte )
{
	(this->*putMemBlockFunc[address>>8])(address, byte);
}

Uint8 Memory128k::getMem( Uint16 address )
{
	return (this->*getMemBlockFunc[address>>8])(address);
}

void Memory128k::memAccess()
{
	accessCount++;
}

Uint8 Memory128k::getMemPassive( int page, Uint16 address )
{
	assert(page<TOTAL_RAM_PAGES);
	return ramPage64k[(page<<16)|address];
}

void Memory128k::cycle()
{
	assert( accessCount <= 1 );
	accessCount = 0;
}

void Memory128k::store( SaveState &state )
{

	state.startChecksum();
	state.writeCString((char*) ramPage64k, TOTAL_RAM_PAGES<<16);
	state.writeCString((char*) rom16k, sizeof(rom16k));
	state.write16(toggleMod);
	state.write16(accessCount);
	state.write16(switchState);

///	Uint8 *slotRom256b[8];	// Pointer to 256 byte rom for slots 1-7
///	Uint8 *expRom2k[8];		// Pointer to 2k expansion rom for slots 1-7

	state.writeChecksum();
	
}

int Memory128k::restore( SaveState &state )
{
	
	state.startChecksum();
	state.readCString((char*) ramPage64k, TOTAL_RAM_PAGES<<16);
	state.readCString((char*) rom16k, sizeof(rom16k));
	toggleMod = state.read16();
	accessCount = state.read16();
	switchState = (Uint32) state.read16();

///	Uint8 *slotRom256b[8];	// Pointer to 256 byte rom for slots 1-7
///	Uint8 *expRom2k[8];		// Pointer to 2k expansion rom for slots 1-7


	// Check data integrity
	if( state.verifyChecksum() ) {
		coldReset();
		return 1;
	}

	return 0;
	
}