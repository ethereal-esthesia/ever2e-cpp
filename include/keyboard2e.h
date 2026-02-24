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

 
#ifndef _KEYBOARD2E_H_
#define _KEYBOARD2E_H_


//#define _KEY_TEST_OUTPUT


#include "SDL.h"
#include "mon560x192.h"


enum CtrlKey2e{ NO_CTRL_KEY = 0x00, OPEN_APPLE = 0x01, CLOSED_APPLE = 0x02, RESET_KEY = 0x04, CTRL_KEY = 0x08, SHIFT_KEY = 0x10 };

class Keyboard2e
{

#ifdef _KEY_TEST_OUTPUT
	Uint8 kx;
	Uint8 sx;
	bool ox;
	bool cx;
#endif

	class Monitor560x192* monitor;  // The Apple II accesses the monitor timing registers to produce key delays leading to unpredictable behavior at times
	class Cpu65c02* cpu;            // Accesses CPU for reset interrupt
	SDL_Event event;                // Used to read key presses and other I/O in SDL

	// Internal variables
	Uint8 keyCode;             // Most recent key with ctrl and shift modifiers included
	bool longDelay;            // Indicates first keypress delay (.53 to .80 sec long), subsequent key repeats are .07 sec)
	int delayCounter;          // Keeps track of number of accesses to timing register (2 for long delay, only 1 for short delay)
	bool keyPressed;           // Key in waiting for processing
	int keyDown;               // Key currently down
	Uint8 overflowByteLast;    // Last timing register access - used to identify state changes

	int resetActive;           // Present state of reset key when not accompanied by other needed keys to perform an interrupt
	CtrlKey2e ctrlKeyState;    // Present state of passive keys

public:

	Keyboard2e( Monitor560x192* monitor, Cpu65c02* cpu );

	~Keyboard2e();

	void keyPress( Uint8 key );
	
	void keyRelease( Uint8 key );
	
	void setCtrlKeyState( CtrlKey2e keys );
		// "keys" should be any or'd combination of OPEN_APPLE | CLOSED_APPLE | RESET_KEY

	void cycle();

	Uint8 getKeyboard();
		// KEYBOARD read from address $C000
		// Bit 7 indicates key was pressed
		// Bit 0-6 indicate ASCII code

	void putStrobe();
		// A write to $C010 (STROBE) will clear the high bit in KEYBOARD
	
	Uint8 getStrobe();
		// Read from address $C010 (STROBE) on IIe and later
		// Read ordinarily clears high bit in KEYBOARD, but this must be done separately with an additional call to "putStrobe"
		// Bit 7 indicates key is currently down
		// Bit 0-6 indicate ASCII code

	bool getOpenApple();
		// Read from address $C061 (OPNAPPLE)
		// Bit 7 is used to indicate if open apple, or PB0 game button is pressed (implemented in joystick class)
		// Bits 0-6 are undefined
	
	bool getClosedApple();
		// Read from address $C062 (CLSAPPLE)
		// Bit 7 is used to indicate if closed apple, or PB1 game button is pressed (implemented in joystick class)
		// Bits 0-6 are undefined
		
};


#endif  // _KEYBOARD2E_H_
