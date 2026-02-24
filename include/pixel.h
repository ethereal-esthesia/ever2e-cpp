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


#ifndef _PIXEL_H_
#define _PIXEL_H_

#include <iostream>
#include <cassert>
#include "SDL.h"
#include "SDL_image.h"



const SDL_Color COLOR_WHITE = {255, 255, 255, 0};
const SDL_Color COLOR_BLACK = {0, 0, 0, 0};



class PixelSurface
{

	SDL_Surface* surface;
	bool locked;
	
	void _newSurface( int xSize, int ySize, int bpp );

public:

	PixelSurface( SDL_Surface* surface, bool locked = false );
		// Can be used for compatibility with alternative SDL libraries
		// If "locked" is set, mode modifications generate an error - useful for shared surfaces

	PixelSurface( PixelSurface& copySurface );
	
	PixelSurface( int xSize, int ySize, int bpp );

	PixelSurface( const char* fileName );
	
	~PixelSurface();
	
	void setPalette( SDL_Color *palette, int entries );

	void setGreyPalette();
	
	int getXSize();
	
	int getYSize();

	int getBpp();

	void fillSurface( SDL_Color color = (SDL_Color) {0, 0, 0, 0} );

	void resize( int xSize, int ySize );
	
	void convertToColorDepth( int bpp );
	
	void loadFile( const char* fileName );

	Uint32 colorToPixel( SDL_Color color );

	SDL_Color pixelToColor( Uint32 pixel );

	void putPixel32( int x, int y, Uint32 pixel );

	void putPixelLine32( int x, int y, const Uint32* pixel, int entries );

	Uint32 getPixel32( int x, int y );

	void putPixel( int x, int y, SDL_Color color );

	SDL_Color getPixel( int x, int y );

	void copy( PixelSurface& copySurface, int x = 0, int y = 0 );

};


#endif  // _PIXEL_H_
