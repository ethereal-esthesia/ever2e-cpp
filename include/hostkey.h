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

 
#ifndef _HOSTKEY_H_
#define _HOSTKEY_H_


#include "SDL.h"


static const Uint8 KEY_UP = 0x0b;
static const Uint8 KEY_DOWN = 0x0a;
static const Uint8 KEY_LEFT = 0x08;
static const Uint8 KEY_RIGHT = 0x15;
static const Uint8 KEY_ENTER = 0x0d;
static const Uint8 KEY_ESCAPE = 0x1b;


class HostKeyboard
{

	static const Uint32 LONG_DELAY = 500;  // Initial repeat-delay: half a second (Apple IIe - average .6675 seconds)
	static const Uint32 SHORT_DELAY = 33;  // Auto-repeat delay: 30 Hz (Apple IIe - 14.98 Hz - once every .06675 sec)

	//static const Uint32 LONG_DELAY = 668;  // Initial repeat-delay: .6675 seconds
	//static const Uint32 SHORT_DELAY = 67;  // Auto-repeat delay: 15 Hz

	Uint8 lastKey;
	Uint32 waitDuration;
	Uint32 waitTimer;
	Uint32 lastTimer;
	bool active;
	bool keyDown;

	Uint32 videoTimer;
	
public:

	HostKeyboard();

	bool popKey( Uint8 &key );
	
	void keyPress( Uint8 key );
	
	void keyRelease( Uint8 key );
	
	bool videoRefreshTimer();
	
};


#endif /* _HOSTKEY_H_ */
