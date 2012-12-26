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
 

#include "eventloop.h"


using namespace std;

const string EventLoop::HOST_HELP[] =
{
	"",
	"\001Function Keys",
	"\003LLLLLLLLLLLLL\001",
	"F1    Help Menu",
	"F2    Device Menu *",
	"",
	"F3/F4 Slow Down / Speed Up CPU",
	"F3+F4 Reset CPU Speed",
	"",
	"F5/F6 CC / Clockwise Color Adjust",
	"F5+F6 Reset Color",
	"F7    Adjust Video Quality Settings",
	"F8    Toggle Monochrome / Color",
	"",
	"SHIFT-F9      Quick Save State *",
	"CTRL-SHIFT-F9 Quick Load State *",
	"",
	"F10   Save / Load Emulator State *",
	"",
	"F11   Toggle Fullscreen Mode",
	"",
	"\001Other Keys                          \001",
	"\003LLLLLLLLLL\001",
	"F12       Reset Key",
	"LEFT ALT  Open-Apple Key (\003A\001)",
	"RIGHT ALT Closed-Apple Key (\0035\001)",
	"",
	"PAUSE     Halt Emulation",
	"ALT-F4    Close Application",
	"",
	"",
	"* Coming Soon",
	""
};


void EventLoop::_toggleHostInterface( HostMenuType type )
{

	if( type==hostMenu ) {
		if( type==MENU_OFF )
			return;
		else {
			type = MENU_OFF;
		}
	}
	else if( hostMenu==MENU_OFF ) {
		storeState(stateBuffer);
		// Remove pause state as needed
		if( idleState )
			monitor->setIdleState(false);
	}

	hostMenu = type;
	
	switch( type ) {

		case MENU_HELP:
			// Set up interface state
			startPos = 0;
			lastStartPos = -1;
			yOffset = 1;
			xOffset = 1;
			cpu->setModeText40();
			break;

		///case SAVE_STATE: . . .
			
		case MENU_OFF:
			restoreState(stateBuffer);
			// Restore pause status
			if( idleState ) {
				monitor->setIdleState(true);
				manager->videoRefresh();
			}
			break;

	}
	
}

void EventLoop::_menuCycle()
{
	switch( hostMenu ) {
		case MENU_HELP:		
			_helpMenuCycle();
			break;
		case MENU_OFF:
			assert( hostMenu != MENU_OFF );
			break;
	}
}

void EventLoop::_helpMenuCycle()
{

	Uint8 key = 0x00;
	if( hostKeyboard->popKey(key) ) {

		if( key == KEY_UP && startPos>0 )
			startPos--;
		else if( key == KEY_DOWN && startPos<int(sizeof(HOST_HELP)/sizeof(string*)-21) )
			startPos++;
		else if( key == KEY_ESCAPE ) {
			_toggleHostInterface(MENU_OFF);
			return;
		}
			
	}
	
	lastStartPos = startPos;
	
	// Show text window
	for( int y = 0; y<21; y++ ) {
		cpu->putText(1, y+yOffset+1, xOffset+1, string(37, ' '));
		cpu->putText(1, y+yOffset+1, xOffset+1, HOST_HELP[startPos+y]);
	}

	// Outer border
	for( int y=0; y<23; y++ ) {
		cpu->putText(1, y, 0, ' ', Cpu65c02::NORMAL);
		cpu->putText(1, y, 39, ' ', Cpu65c02::NORMAL);
	}
	for( int x=0; x<40; x++ ) {
		cpu->putText(1, 0, x, ' ', Cpu65c02::NORMAL);
		cpu->putText(1, 23, x, ' ', Cpu65c02::NORMAL);
	}
	
	// Menu border
	for( int y=1; y<23; y++ )
		cpu->putText(1, y, 1, '4', Cpu65c02::FLASH);
	for( int x=1; x<39; x++ )
		cpu->putText(1, 0, x, '_', Cpu65c02::NORMAL);
	for( int x=1; x<39; x++ )
		cpu->putText(1, 1, x, ' ', Cpu65c02::INVERSE);
	for( int x=1; x<39; x++ )
		cpu->putText(1, 23, x, 'L', Cpu65c02::FLASH);

	// Show title
	string str(" Help Menu ");
	cpu->putText(1, 1, (40-str.length())>>1, str, Cpu65c02::INVERSE);

	// Draw scroll bar
	for( int y=2; y<23; y++ )
		cpu->putText(1, y, 37, "ZZ", Cpu65c02::FLASH);  // Bar
	cpu->putText(1, 2, 38, 'R', Cpu65c02::FLASH);       // Up arrow
	cpu->putText(1, 22, 38, 'Q', Cpu65c02::FLASH);      // Down arrow

	cpu->putText(1, yOffset+2+(18*startPos)/(sizeof(HOST_HELP)/sizeof(string*)-21), 38, '3', Cpu65c02::FLASH);  // Position indicator
	
}

EventLoop::EventLoop()
{

#ifdef _CONSOLE_OUT
	EventManager::consoleDisplay();
#endif		

	fullScreen = false;
	manager = new EventManager(WINDOW_X_SIZE, WINDOW_Y_SIZE, BPP, false, "Ever2e", "icon.png");
	surface = manager->getVideoSurface();
	sound = new SoundBuffer();		
	hostKeyboard = new HostKeyboard();		
	memory = new Memory128k();
	monitor = new Monitor560x192(surface, memory);
	monitor->setOffset( (WINDOW_X_SIZE-X_SIZE)>>1, (WINDOW_Y_SIZE-Y_SIZE)>>1 );
	cpu = new Cpu65c02(memory);                
	cpuMult = 1;
	cpu->setMultiplier(cpuMult);           
	keyboard = new Keyboard2e(monitor, cpu);  
	keyConvert = new KeyTrans2e();
	speaker = new Speaker1bit();   
	memory->putPeripheral(cpu, monitor, speaker, keyboard);

	lastClock = manager->getClock();
	elapsedTimeNanoseconds = 0;

	idleCycle = false;
	
	exitStatus = false;
	idleState = false;
	hostMenu = MENU_OFF;
	
	callbackFunc = NULL;
	callBackPerCycle = false;

	hue = monitor->getHue()<<HUE_FIX_POSITION;
	defaultHue = hue;

	checkEventTimer = 0;

#ifdef _BENCHMARK_
	// Time CPU
	times = 0.;
	timer = 0.;
	for( int k = 0; k<100; k++ ) {
		Uint32 clock = manager->getClock();
		while( clock==lastClock ) {
			timer++;
			clock = manager->getClock();
		}
		lastClock = clock;
		times++;
	}
	bench = timer/times;
	times = 0.;
	timer = 0.;
#endif

	// Start with help screen
	_toggleHostInterface( MENU_HELP );
	
}

EventLoop::~EventLoop()
{

	delete manager;
	delete sound;
	delete hostKeyboard;		
	delete memory;
	delete monitor;
	delete cpu;
	delete keyboard;
	delete keyConvert;
	delete speaker;

}

void EventLoop::cycle() 
{

#ifdef _BENCHMARK_
	// Report timing information
	Uint32 clock = manager->getClock();
	while( clock==lastClock ) {
		timer++;
		clock = manager->getClock();
	}
	Uint32 elapsedTime = clock - lastClock;
	lastClock = clock;
	times++;
	if( !(int(times)&0xff) )
		cout << int(timer/times/bench*100.+.5) << "%        \r";
#else
	// Sync host/guest clock
	Uint32 clock = manager->getClock();
	while( clock==lastClock ) {
		SDL_Delay(1);
		clock = manager->getClock();
	}
	Uint32 elapsedTime = clock - lastClock;
	lastClock = clock;
#endif

	// Cap on catch-up rate to prevent unnecessary speed-up in case of a long OS pause, possibly caused by:
	//   frame dragging / sleep mode / temporary system lock-up / temporarily heavy cpu workload / clock overflow (every 48 days)
	if( elapsedTime > 1000/32) {
#ifdef _BENCHMARK_
		cerr << "Warning: emulation dropped " << (elapsedTime -1000/32)/1000. << " host sec\n";
#endif
		elapsedTime = 1000/32;
	}

	// Convert guest clock to nanoseconds
	elapsedTimeNanoseconds += (Sint32) elapsedTime*1000000;

	// Update I/O switches and catch system events
	const SDL_Event* event;
	
	checkEventTimer += elapsedTime;
	if( checkEventTimer>10 ) {

		checkEventTimer = 0;

		bool doubleDown = false;  // Keep track of whether key events are in a lag state to avoid trailing events
		
		while( ( event = manager->popEvent() ) != NULL )
		{

///			const SDL_Event* nextEvent;
					
			switch( event->type ) {

				case SDL_KEYDOWN:

/***
					// Ignore key if it is not being pressed at this moment to avoid key glitches in malfunctioning keyboards
					nextEvent = manager->peekEvent();
					if( ( nextEvent != NULL ) && ( nextEvent->type == SDL_KEYUP ) && ( nextEvent->key.keysym.sym == event->key.keysym.sym ) )
						break;
***/				
					switch( event->key.keysym.sym ) {

						case SDLK_F1:
							if( doubleDown )
								break;
							// Help menu
							_toggleHostInterface(MENU_HELP);
							break;
		
						case SDLK_F2:
							if( doubleDown )
								break;
							// Device menu
							/// STUB
							break;
		
						case SDLK_F3:
							if( doubleDown||idleState )
								break;
							if( cpuMult>1 ) {
								cpuMult >>= 1;
								cpu->setMultiplier(cpuMult);
							}
							break;
		
						case SDLK_F4:
							if( doubleDown )
								break;
							if( event->key.keysym.mod&KMOD_ALT )
								exitStatus = true;
							else if( cpuMult<MAX_CPU_MULT ) {
								cpuMult <<= 1;
								cpu->setMultiplier(cpuMult);
							}
							break;

						// F5 and F6 keys are handled outside of this loop to allow appropriate key-holding logic
							
						case SDLK_F7: {

								if( doubleDown||idleState )
									break;
									
								// Change visual quality state

								int scanState = monitor->getSolidColor() | (monitor->getScanlineSkip()<<1);
								
								if( monitor->getMonitorType() != COLOR_MONITOR )
									monitor->setScanlineSkip(!monitor->getScanlineSkip());
								else
									switch( scanState ) {
									
										// Solid = false, skip = false  ->  state 1
										case 0:
											monitor->setSolidColor(true);
											break;
										
										// Solid = true, skip = false  ->  state 3
										case 1:
											monitor->setScanlineSkip(true);
											break;
										
										// Solid = false, skip = true  ->  state 0
										case 2:
											monitor->setScanlineSkip(false);
											break;
										
										// Solid = true, skip = true  ->  state 2
										case 3:
											monitor->setSolidColor(false);
											break;
									
									};
								
							}
							break;
	
						case SDLK_F8:
							if( doubleDown||idleState )
								break;
							// Change color mode
							if( monitor->getMonitorType() == MONO_MONITOR )
								monitor->setMonitorType(GREEN_MONITOR);
							else if( monitor->getMonitorType() == GREEN_MONITOR )
								monitor->setMonitorType(COLOR_MONITOR);
							else
								monitor->setMonitorType(MONO_MONITOR);
							break;
						
						case SDLK_F9:
							if( doubleDown )
								break;
							if( event->key.keysym.mod&KMOD_SHIFT ) {
								if( event->key.keysym.mod&KMOD_CTRL ) {
									// Quick-load state
									/// STUB
								}
								else {
									// Quick-save state
									/// STUB
								}
							}
							break;
		
						case SDLK_F10:
							if( doubleDown )
								break;
							// Save load emulator state
							/// STUB
							break;
		
						case SDLK_F11:
							if( doubleDown )
								break;
							// Fullscreen mode
							fullScreen = !fullScreen;
							if( fullScreen ) {
								manager->resizeDisplay(FULL_X_SIZE, FULL_Y_SIZE, BPP, true);
								monitor->setOffset( (FULL_X_SIZE-X_SIZE)>>1, (FULL_Y_SIZE-Y_SIZE)>>1 );
							}
							else {
								manager->resizeDisplay(WINDOW_X_SIZE, WINDOW_Y_SIZE, BPP, false);
								monitor->setOffset( (WINDOW_X_SIZE-X_SIZE)>>1, (WINDOW_Y_SIZE-Y_SIZE)>>1 );
							}
							break;
						
						case SDLK_PAUSE:
							if( doubleDown||hostMenu!=MENU_OFF )
								break;
							// Toggle idle video emulation state
							idleState = !idleState;
							monitor->setIdleState(idleState);
							manager->videoRefresh();
							break;
							
						default: {

								// Pass key to keyboard handler
								Uint8 key;
								if( keyConvert->translate(event->key.keysym, key) ) {
									keyboard->keyPress(key);
									hostKeyboard->keyPress(key);
#ifdef _KEY_TEST_OUTPUT
									cout << "PRESS '" << SDL_GetKeyName(event->key.keysym.sym) << hex << "' " << int(key) << dec << endl;
#endif
								}
#ifdef _KEY_TEST_OUTPUT
								else
									cout << "UNPROCESSED KEY PRESSED: " << SDL_GetKeyName( event->key.keysym.sym ) << endl;
#endif
								break;
							}
						
					}
					doubleDown = true;
					break;
					
				case SDL_KEYUP: {
				
						// Pass key to keyboard handler
						Uint8 key;
						if( keyConvert->translate(event->key.keysym, key) ) {
							keyboard->keyRelease(key);
#ifdef _KEY_TEST_OUTPUT
							cout << "RELEASE '" << SDL_GetKeyName(event->key.keysym.sym) << hex << "' " << int(key) << dec << endl;
#endif
							hostKeyboard->keyRelease(key);
						}
							
					}
					break;

				case SDL_VIDEOEXPOSE:
					if( idleState ) {
						monitor->refreshFrame();
						manager->videoRefresh();
					}
					break;
					
				case SDL_QUIT:
					exitStatus = true;
					break;

				default:
					break;

			}

		}

		// Check for passive key presses before continuing cycle (apple keys, reset key)
		if( !idleState ) {
		
			// Check for palette shifting (color knob emulation)
			if( monitor->getMonitorType() == COLOR_MONITOR ) {
				if( manager->isPressed(SDLK_F5) && manager->isPressed(SDLK_F6) ) {
					hue = defaultHue;
					monitor->setHue(hue>>HUE_FIX_POSITION);
				}
				else if( manager->isPressed(SDLK_F5) && hue>defaultHue-(0x0100<<HUE_FIX_POSITION) )
					monitor->setHue((--hue)>>HUE_FIX_POSITION);
				else if( manager->isPressed(SDLK_F6) && hue<defaultHue+(0x0100<<HUE_FIX_POSITION) )
					monitor->setHue((++hue)>>HUE_FIX_POSITION);
			}

			// Check for simultaneous F3+F4 (CPU speed - and +)
			if( manager->isPressed(SDLK_F3) && manager->isPressed(SDLK_F4) ) {
				cpuMult = 1;
				cpu->setMultiplier(cpuMult);
			}	
			
			// Check for passive key presses: open apple, closed apple, reset, control, shift
			CtrlKey2e ctrlKey = manager->isPressed(keyConvert->getOpenAppleKey()) ? OPEN_APPLE : NO_CTRL_KEY;
			ctrlKey = CtrlKey2e( ctrlKey | ( manager->isPressed(keyConvert->getClosedAppleKey()) ? CLOSED_APPLE : NO_CTRL_KEY ) );
			ctrlKey = CtrlKey2e( ctrlKey | ( manager->isPressed(keyConvert->getResetKey()) ? RESET_KEY : NO_CTRL_KEY ) );
			ctrlKey = CtrlKey2e( ctrlKey | ( (manager->isPressed(SDLK_LCTRL) || manager->isPressed(SDLK_RCTRL)) ? CTRL_KEY : NO_CTRL_KEY ) );
			ctrlKey = CtrlKey2e( ctrlKey | ( (manager->isPressed(SDLK_LSHIFT) || manager->isPressed(SDLK_RSHIFT)) ? SHIFT_KEY : NO_CTRL_KEY ) );
			keyboard->setCtrlKeyState(ctrlKey);
			
		}

	}

	// Check to see if emulation is in idle mode and cancel updates as needed
	if( idleState && hostMenu==MENU_OFF )
		elapsedTimeNanoseconds = 0;

	// Continue emulation sequence on the microsecond timescale
	while( elapsedTimeNanoseconds > 0 ) {

		// Time lapse

		// Sync timing (64 1.022727 MHz CPU cycles followed by a 65th horizontal refresh sync-cycle at 0.8956777 MHz)
		Sint32 cycleTimeNanoseconds = Cpu65c02::CPU_TIMING_NANOSECONDS;
		if( monitor->getHRefreshCount() == Monitor560x192::HORIZONTAL_SCANS-1 )
			cycleTimeNanoseconds += Monitor560x192::HORIZONTAL_CPU_SYNC_TIMING;

		// Call host function once per frame refresh
		if( monitor->getVRefreshCount()==0 && monitor->getHRefreshCount()==0 ) {
			if( hostMenu!=MENU_OFF ) {
				_menuCycle();
				idleCycle = true;
			}
			if( hostMenu==MENU_OFF ) {
				if( callbackFunc!=NULL )
					idleCycle = callbackFunc(this);
				else
					idleCycle = false;
			}
			manager->videoRefresh();
		}
		else if( callBackPerCycle && hostMenu==MENU_OFF ) {
			if(  callbackFunc!=NULL )
				idleCycle = callbackFunc(this);
			else
				idleCycle = false;
		}


		// Cycle hardware

		keyboard->cycle();
		if( !idleCycle ) {
			cpu->cycle();
		}
		monitor->cycle();
		memory->cycle();
		speaker->cycle(cycleTimeNanoseconds);

		// Communicate with host speaker if a sound sample is complete (approx. every 46,000 emulated cycles at 22050 samples per sec.)
		Uint16 soundWord;
		if( speaker->pollSound(soundWord) )
			sound->playSample(soundWord);

		elapsedTimeNanoseconds -= cycleTimeNanoseconds;

	}

}
	
void EventLoop::incorporate( bool (*func)( EventLoop* ), bool perCycle )
{
	callbackFunc = func;
	callBackPerCycle = perCycle;
}
	
bool EventLoop::getExitStatus()
{
	return exitStatus;
}

void EventLoop::storeState( SaveState& state )
{
	memory->store(state);
	monitor->store(state);
}

void EventLoop::restoreState( SaveState& state )
{	
	assert( memory->restore(state)==0 );
	assert( monitor->restore(state)==0 );
	state.clear();
}
