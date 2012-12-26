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

 
#include "hostkey.h"


HostKeyboard::HostKeyboard()
{
	lastKey = 0x00;
	active = false;
	keyDown = true;
	waitDuration = 0;
	waitTimer = 0;
	lastTimer = 0;
	videoTimer = SDL_GetTicks();
}

bool HostKeyboard::popKey( Uint8 &key )
{

	if( !active && waitTimer==0 )
		return false;

	Uint32 timer = SDL_GetTicks();

	if( !active && keyDown ) {
		if( waitTimer<=timer || lastTimer>timer )
			active = true;
		lastTimer = timer;
	}

	if( active ) {
		waitTimer = timer + waitDuration;
		waitDuration = SHORT_DELAY;
		active = false;
		key = lastKey;
		return true;
	}

	return false;
	
}

void HostKeyboard::keyPress( Uint8 key )
{
	waitDuration = LONG_DELAY;
	lastKey = key;
	active = true;
	keyDown = true;
}

void HostKeyboard::keyRelease( Uint8 key )
{
	if( key!=lastKey )
		return;
	active = false;
	keyDown = false;
	waitTimer = 0;
}

bool HostKeyboard::videoRefreshTimer()
{
	Uint32 timer = SDL_GetTicks();
	if( videoTimer<timer ) {
		videoTimer += 1000/60;
		// If timer is more than half a frame behind the clock, ignore lag
		if( videoTimer < timer-500/60 )
			videoTimer = timer-500/60;
		return true;
	}
	else
		return false;
}
