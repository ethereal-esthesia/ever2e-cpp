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

 
#ifndef _EVENTLOOP_H_
#define _EVENTLOOP_H_


#include <iostream>
#include <cstdlib>
#include "SDL.h"
#include "manager.h"
#include "sound.h"
#include "pixel.h"
#include "cpu65c02.h"
#include "mon560x192.h"
#include "memory128k.h"
#include "speaker1bit.h"
#include "keyboard2e.h"
#include "keytrans2e.h"
#include "hostkey.h"
#include "savestate.h"

// Uncomment these to view speed stat changes (this test maxes out the CPU cycles)
//#define _BENCHMARK_

// Show console output (needed for most diagnostic routines)
//   Note: -mwindows flag can not be used during compile and application may need to be run from the console under some circumstances
//#define _CONSOLE_OUT


using namespace std;


class EventLoop
{

	static const int BLANK_BORDER = 16;
	static const int X_SIZE = Monitor560x192::X_RES*Monitor560x192::X_MULT+Monitor560x192::X_OFFSET;
	static const int Y_SIZE = Monitor560x192::Y_RES*Monitor560x192::Y_MULT+Monitor560x192::Y_OFFSET;

	static const int WINDOW_X_SIZE = X_SIZE+(BLANK_BORDER<<1);
	static const int WINDOW_Y_SIZE = Y_SIZE+(BLANK_BORDER<<1);
	static const int FULL_X_SIZE = 640;
	static const int FULL_Y_SIZE = 480;
	static const int BPP = 32;
	
	static const int HUE_FIX_POSITION = 1;
	static const int MAX_CPU_MULT = 16;

	static const string HOST_HELP[];
	int startPos;
	int lastStartPos;
    int yOffset;
	int xOffset;

	bool (*callbackFunc)( EventLoop* );    // Function called every frame refresh
	bool callBackPerCycle;
	
	class EventManager* manager;       // Host video / event interface
	class PixelSurface* surface;       // Host video buffer
	class KeyTrans2e* keyConvert;      // Translates host key values to Apple IIe equivelant
	class SoundBuffer* sound;          // Host sound interface
	class HostKeyboard* hostKeyboard;  // Host keyboard interface
	SaveState stateBuffer;
	
	Uint32 lastClock;
	Sint32 elapsedTimeNanoseconds;

	int cpuMult;
	bool fullScreen;
	bool exitStatus;
	bool idleState;
	bool idleCycle;		

	Sint16 hue;
	Sint16 defaultHue;
	Uint32 checkEventTimer;
		
#ifdef _BENCHMARK_
	double times;
	double timer;
	double bench;
#endif

	enum HostMenuType{ MENU_OFF, MENU_HELP }
		hostMenu;
	
	void _toggleHostInterface( HostMenuType type );

	void _menuCycle();

	void _helpMenuCycle();

public:

	class Memory128k* memory;       // Includes memory bus functions, RAM and ROM storage
	class Monitor560x192* monitor;  // The surface passed to the monitor points directly to the video display
	class Cpu65c02* cpu;            // The CPU accesses all hardware through soft-switches in located in memory
	class Keyboard2e* keyboard;     // The keyboard needs access to the monitor for syncing purposes and the CPU for RESET interrupt
	class Speaker1bit* speaker;     // Requires no access to other hardware, returns sound information via polling

	EventLoop();

	~EventLoop();
	
	void cycle();

	void incorporate( bool (*func)( EventLoop* ), bool perCycle = false );
	
	bool getExitStatus();
	
	void storeState( SaveState& state );
	
	void restoreState( SaveState& state );
		
};


#endif /* _EVENTLOOP_H_ */
