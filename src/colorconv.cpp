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
 

#include "colorconv.h"


using namespace std;


SDL_Color hlsToColor( Uint8 hue, Uint8 lum, Uint8 sat )
{

	int r, g, b;
	int iLum = lum-0x80;

	// Set hue
	r = 0x2ff-((int)(hue)*6);
	r = max( 0, min( 0xff, abs(r)-0x100 ) );
	g = 0x2ff-((((int)(hue)-0x55)&0xff)*6);
	g = max( 0, min( 0xff, abs(g)-0x100 ) );
	b = 0x2ff-((((int)(hue)-0xaa)&0xff)*6);
	b = max( 0, min( 0xff, abs(b)-0x100 ) );

	// Set lu_minescence
	r += (iLum*(iLum<0 ? r:(0xff^r)))>>7;
	g += (iLum*(iLum<0 ? g:(0xff^g)))>>7;
	b += (iLum*(iLum<0 ? b:(0xff^b)))>>7;

	// Set saturation
	r += ((int)(0xff^sat)*((int)(lum)-(int)(r)))>>8;
	g += ((int)(0xff^sat)*((int)(lum)-(int)(g)))>>8;
	b += ((int)(0xff^sat)*((int)(lum)-(int)(b)))>>8;

	SDL_Color color;
	color.r = r;
	color.g = g;
	color.b = b;
	color.unused = 0;

	return color;

}

SDL_Color hsbToColor( Uint8 hue, Uint8 sat, Uint8 bri )
{

	int r, g, b;

	// Set hue
	r = 0x2ff-((int)(hue)*6);
	r = max( 0, min( 0xff, abs(r)-0x100 ) );
	g = 0x2ff-((((int)(hue)-0x55)&0xff)*6);
	g = max( 0, min( 0xff, abs(g)-0x100 ) );
	b = 0x2ff-((((int)(hue)-0xaa)&0xff)*6);
	b = max( 0, min( 0xff, abs(b)-0x100 ) );

	// Set brightness
	r = (bri*r)>>8;
	g = (bri*g)>>8;
	b = (bri*b)>>8;

	// Set saturation
	r += ((int)(0xff^sat)*((int)(bri)-(int)(r)))>>8;
	g += ((int)(0xff^sat)*((int)(bri)-(int)(g)))>>8;
	b += ((int)(0xff^sat)*((int)(bri)-(int)(b)))>>8;

	SDL_Color color;
	color.r = r;
	color.g = g;
	color.b = b;
	color.unused = 0;

	return color;

}

