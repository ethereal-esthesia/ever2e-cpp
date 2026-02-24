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
 

#include "keyboard2e.h"


using namespace std;


Keyboard2e::Keyboard2e( Monitor560x192* monitor, Cpu65c02* cpu )
{

#ifdef _KEY_TEST_OUTPUT
	kx = 0;
	sx = 0;
	ox = 0;
	cx = 0;
#endif
		
	this->monitor = monitor;
	this->cpu = cpu;
	keyCode = 0;         /// Likely to be 0 on bootup, but this needs verification ///
	ctrlKeyState = NO_CTRL_KEY;
	longDelay = false;        
	delayCounter = 0;      
	keyPressed = false;       
	keyConsumed = false;
	consumeCounter = 0;
	keyDown = 0;        
	overflowByteLast = monitor->getOverflowCount();
	resetActive = false;

}

Keyboard2e::~Keyboard2e()
{
}

void Keyboard2e::keyPress( Uint8 key )
{
	keyDown++;
	keyCode = key;
	keyPressed = true;
	keyConsumed = false;
	longDelay = true;
	delayCounter = 0;
}

void Keyboard2e::keyRelease( Uint8 key )
{

	keyDown--;
	if( keyDown<0 )
		keyDown = 0;  // Error checking
	if( keyDown==0 ) {
		longDelay = true;
		delayCounter = 0;
	}
	
}

void Keyboard2e::queueTypedKey( Uint8 key )
{
	queuedTypedKeys.push_back(key);
}
	
void Keyboard2e::setCtrlKeyState( CtrlKey2e keys )
{
	ctrlKeyState = keys;
}

void Keyboard2e::cycle()
{

	// Check reset status
	if( ctrlKeyState&RESET_KEY ) {
		if( !resetActive ) {
			if( ctrlKeyState&CTRL_KEY ) {
				resetActive = 1;
				cpu->interrupt(Cpu65c02::_HLT);
			}
			else
				resetActive = 2;
		}
	}
	else
		if( resetActive ) {
			if( resetActive==1 )
				cpu->interrupt(Cpu65c02::_RES);
			resetActive = 0;
		}

	// Handle delays
	Uint8 overflowByte = monitor->getOverflowCount();
	if( delayCounter && keyDown ) {
		if( longDelay ) {
			if( (overflowByte&0x10) != (overflowByteLast&0x10) ) {
				delayCounter--;
				if( delayCounter == 0 ) {
					keyPressed = true;
					longDelay = false;
				}
			}
		}
		else
			if( (overflowByte&0x04) != (overflowByteLast&0x04) ) {
				delayCounter = 0;
				keyPressed = true;
			}
	}
	
	// Record monitor timing for comparison next cycle
	overflowByteLast = overflowByte;

#ifdef _KEY_TEST_OUTPUT
		
	Uint8 k = getKeyboard();
	Uint8 s = getStrobe();
	bool o = getOpenApple();
	bool c = getClosedApple();

	if( kx!=k || sx!=s || ox!=o || cx!=c ) {
		if( (k&0x7f)>=0x20 )
			cout << hex << "KEY: 0x" << int(k)  << " (" << char(k&0x7f) << ") STROBE: 0x" << (int) s << " OPNAPPLE: " << (int) o << " CLSAPPLE: " << (int) c << dec << endl;
		else
			cout << hex << "KEY: 0x" << int(k)  << " STROBE: 0x" << (int) s << " OPNAPPLE: " << (int) o << " CLSAPPLE: " << (int) c << dec << endl;
	}
	
	kx = k;
	sx = s;
	ox = o;
	cx = c;
	
#endif

}

Uint8 Keyboard2e::getKeyboard()
{
	return (keyPressed<<7) | keyCode;
}

void Keyboard2e::consumeKeyboard()
{
	consumeCounter++;
	if( !keyPressed && !queuedTypedKeys.empty() ) {
		keyCode = queuedTypedKeys.front();
		queuedTypedKeys.pop_front();
		keyPressed = true;
		keyConsumed = true;
		return;
	}
	if( keyPressed )
		keyConsumed = true;
}

bool Keyboard2e::hasPendingKey() const
{
	return keyPressed;
}

Uint32 Keyboard2e::getConsumeCounter() const
{
	return consumeCounter;
}

void Keyboard2e::putStrobe()
{

	// Match IIe queue semantics: clear strobe only after KEYBOARD ($C000) consumed it.
	if( keyPressed && keyConsumed ) {
		keyPressed = false;
		keyConsumed = false;
		if( longDelay )
			delayCounter = 3;
		else
			delayCounter = 1;	
	}	

}

Uint8 Keyboard2e::getStrobe()
{
	return ((true&&keyDown)<<7) | keyCode;
}

bool Keyboard2e::getOpenApple()
{
	return ctrlKeyState&OPEN_APPLE;
}

bool Keyboard2e::getClosedApple()
{
	return ctrlKeyState&CLOSED_APPLE;
}

bool Keyboard2e::getShiftKey()
{
	return ctrlKeyState&SHIFT_KEY;
}

void Keyboard2e::store( SaveState& state )
{
	state.write8(keyCode);
	state.writeBool(longDelay);
	state.write32((Uint32)delayCounter);
	state.writeBool(keyPressed);
	state.writeBool(keyConsumed);
	state.write32(consumeCounter);
	state.write32((Uint32)keyDown);
	state.write8(overflowByteLast);
	state.write32((Uint32)resetActive);
	state.write16((Uint16)ctrlKeyState);
	state.write32((Uint32)queuedTypedKeys.size());
	for( size_t i = 0; i<queuedTypedKeys.size(); i++ )
		state.write8(queuedTypedKeys[i]);
}

int Keyboard2e::restore( SaveState& state )
{
	keyCode = state.read8();
	longDelay = state.readBool();
	delayCounter = (int) state.read32();
	keyPressed = state.readBool();
	keyConsumed = state.readBool();
	consumeCounter = state.read32();
	keyDown = (int) state.read32();
	overflowByteLast = state.read8();
	resetActive = (int) state.read32();
	ctrlKeyState = (CtrlKey2e) state.read16();
	queuedTypedKeys.clear();
	Uint32 queuedSize = state.read32();
	for( Uint32 i = 0; i<queuedSize; i++ )
		queuedTypedKeys.push_back(state.read8());
	return 0;
}
