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

 
#ifndef _KEYTRANS2E_H_
#define _KEYTRANS2E_H_


#include <iostream>
#include <cassert>
#include <unordered_map>
#include "SDL.h"


class KeyTrans2e
{

	std::unordered_map<SDL_Keycode, Sint8> keyConvTable;
	std::unordered_map<SDL_Keycode, Sint8> keyConvTableCaps;
	std::unordered_map<SDL_Keycode, Sint8> keyConvTableShift;
	std::unordered_map<SDL_Keycode, Sint8> keyConvTableCtrl;

public:

	KeyTrans2e();
	
	~KeyTrans2e();

	bool translate( SDL_Keycode key, SDL_Keymod mod, Uint8 & key2e );

	SDL_Keycode getOpenAppleKey();
	
	SDL_Keycode getClosedAppleKey();
	
	SDL_Keycode getResetKey();
	
};


#endif  // _KEYTRANS2E_H_
