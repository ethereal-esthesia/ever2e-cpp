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
#include <array>
#include "SDL_stdinc.h"


class PeripheralCard16bit
{

	std::array<Uint8, 256> slotRom256b;  // 256-byte slot ROM window at Cn00
	std::array<Uint8, 2048> expRom2k;    // 2KB expansion ROM window at C800
	std::string idTag;
	
public:

	PeripheralCard16bit();
	
	void putMem16b( Uint8 offset, Uint8 byte );
	Uint8 getMem16b( Uint8 offset );
	
	Uint8 getMem256b( Uint8 offset );
	
	Uint8 getMem2k( Uint16 offset );

	void setSlotPattern( Uint8 value );

	void setIdTag( const std::string& id );

	const std::string& getIdTag() const;

};


#endif  // _CARD2E_H_
