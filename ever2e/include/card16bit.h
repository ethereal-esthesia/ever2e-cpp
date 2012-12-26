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


#ifndef _CARD2E_H_
#define _CARD2E_H_

#include <string>
#include <cassert>
#include "SDL_stdinc.h"


class PeripheralCard16bit
{

	const Uint8 *slotRom256b;  // Pointer to the 256 byte memory that will be read at C_00h, where "_" is the slot number
	const Uint8 *expRom2k;  // Pointer to the 2k memory that will be read at C800h
	std::string idTag;
	
public:

	PeripheralCard16bit();
	
	void putMem16b( Uint8 offset, Uint8 byte );
	Uint8 getMem16b( Uint8 offset );
	
	Uint8 getMem256b( Uint8 offset );
	
	Uint8 getMem2k( Uint16 offset );

};


#endif  // _CARD2E_H_
