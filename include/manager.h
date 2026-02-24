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


#ifndef _MANAGER_H_
#define _MANAGER_H_


#include <iostream>
#include <cassert>
#include "SDL.h"
#include "SDL_image.h"
#include "pixel.h"


class EventManager
{

	SDL_Surface* displaySurface;
	PixelSurface* displaySurfaceClass;

	SDL_Event event;
	SDLMod modState;
	Uint8* keyState;    // Pointer to SDL key states
	int keyStateTotal;  // Number of SDL reserved keys
	
public:

	EventManager( int xSize = 600, int ySize = 440, int bpp = 0, bool fullScreen = false, const char* caption = "Application", const char* iconFile = "icon.png" );
		// Enter bpp of 0 to use optimal color resolution

	~EventManager();

	void resizeDisplay( int xSize, int ySize, int bpp = 0, bool fullScreen = false );

	PixelSurface* getVideoSurface();
		// Returns a pointer to visible buffer
		// Modifications to image properties generate an error
		// Any changes made can be displayed with a call to "videoRefresh"
	
	void getVideoParam( int &xSize, int &ySize, int &bpp );

	void videoRefresh();
	
	static void hideCursor();
	
	static void showCursor();

	static void consoleDisplay();
		// Redirects file I/O to monitor when default is set to file I/O
		// This is useful in operating systems where the existance of the console is not able to be detected
	
	Uint32 getClock();
	
	const SDL_Event* popEvent();
		// Returns NULL pointer if no events are on queue
		// Otherwise returns the next SDL-defined event on queue

	const SDL_Event* peekEvent();
		// Returns next event on queue without removing it
		
	void refreshEventStatus();

	bool isPressed( SDLKey key );
		// Returns whether a given key was pressed the last time the event queue was empty or
		//    during the last call to refreshEventStatus
		// Modifier keys return valid values

};


#endif  // _MANAGER_H_
