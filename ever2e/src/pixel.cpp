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
 

#include "pixel.h"


using namespace std;



void PixelSurface::_newSurface( int xSize, int ySize, int bpp )
{

	Uint32 rmask, gmask, bmask, amask;

	switch( bpp ) {

	case 8:
		rmask = 0x00000000;
		gmask = 0x00000000;
		bmask = 0x00000000;
		amask = 0x00000000;
		break;

	case 15:
		rmask = 0x0000001f;
		gmask = 0x000003e0;
		bmask = 0x00007c00;
		amask = 0x00000000;
		break;

	case 16:
		rmask = 0x0000001f;
		gmask = 0x000007e0;
		bmask = 0x0000f800;
		amask = 0x00000000;
		break;

	case 24:
		rmask = 0x000000ff;
		gmask = 0x0000ff00;
		bmask = 0x00ff0000;
		amask = 0x00000000;
		break;

	case 32:
		rmask = 0x000000ff;
		gmask = 0x0000ff00;
		bmask = 0x00ff0000;
		amask = 0xff000000;
		break;

	default:
		std::cerr << "PixelSurface::_newSurface - " << bpp << " bits-per-pixel not supported\n";
		exit(1);

	}

	surface = SDL_CreateRGBSurface(SDL_SWSURFACE, xSize, ySize, bpp, rmask, gmask, bmask, amask);
	if( surface==NULL ) {
		std::cerr << "PixelSurface::PixelSurface - Unable to initialize surface - " << SDL_GetError() << std::endl;
		exit(1);
	}
	
	if( bpp == 8 )
		setGreyPalette();

}

PixelSurface::PixelSurface( SDL_Surface* surface, bool locked )
{
	if( surface == NULL ) {
		std::cerr << "PixelSurface::PixelSurface - Invalid surface given.\n";
		exit(1);
	}
	this->surface = surface;
	this->locked = locked;
}

PixelSurface::PixelSurface( PixelSurface& copySurface )
{
	/// STUB ///
	surface = NULL;
	locked = false;
}

PixelSurface::PixelSurface( int xSize, int ySize, int bpp )
{
	_newSurface(xSize, ySize, bpp);
	locked = false;
}

PixelSurface::PixelSurface( const char* fileName )
{
	loadFile(fileName);
	locked = false;
}

PixelSurface::~PixelSurface()
{
	SDL_FreeSurface(surface);
}

void PixelSurface::setPalette( SDL_Color *palette, int entries )
{	
	SDL_SetColors(surface, palette, 0, entries);	
}

void PixelSurface::setGreyPalette()
{
	SDL_Color greyPal[0x100];

	// Set up gray scale palette
	for( int k = 0; k<0x100; k++ ) {
		greyPal[k].r = k;
		greyPal[k].g = k;
		greyPal[k].b = k;
		greyPal[k].unused = 0;
	}
	setPalette(greyPal, 0x100);
}

int PixelSurface::getXSize()
{
	return surface->w;
}

int PixelSurface::getYSize()
{
	return surface->h;
}

int PixelSurface::getBpp()
{
	return surface->format->BitsPerPixel;
}

void PixelSurface::fillSurface( SDL_Color color )
{
	Uint32 pixel = SDL_MapRGB(surface->format, color.r, color.g, color.b);
	SDL_FillRect(surface, NULL, pixel);
}

void PixelSurface::resize( int xSize, int ySize )
{	
	assert( !locked );
	/// STUB ///
	SDL_FreeSurface(surface);
	_newSurface(xSize, ySize, getBpp());
}

void PixelSurface::convertToColorDepth( int bpp )
{
	assert( !locked );
	/// STUB ///
}

void PixelSurface::loadFile( const char* fileName )
{
	assert( !locked );
	surface = IMG_Load(fileName);
	if( surface==NULL ) {
		std::cerr << "PixelSurface::PixelSurface - Unable to load file: \"" << fileName << "\"\n";
		exit(1);
	}
}

Uint32 PixelSurface::colorToPixel( SDL_Color color )
{
	Uint32 pixel;
	/// STUB
	if( surface->format->BitsPerPixel==8 )
		pixel = ((Uint16)color.r+color.g+color.b)/3;
	else
	///
	pixel = SDL_MapRGB(surface->format, color.r, color.g, color.b);
	return pixel;
}

SDL_Color PixelSurface::pixelToColor( Uint32 pixel )
{
	SDL_Color color;
	/// STUB ///
	/// if( surface->format->BitsPerPixel==8 ) {
	///	color = pal...;
	/// }
	/// else
	SDL_GetRGB(pixel, surface->format, &color.r, &color.g, &color.b);
	return color;
}

void PixelSurface::putPixel32( int x, int y, Uint32 pixel )
{
	// Perform clipping
	if( (unsigned int) x >= (unsigned int) surface->w || (unsigned int) y >= (unsigned int) surface->h )
		return;
	size_t bytes = (surface->format->BitsPerPixel+7)>>3;
	memcpy((Uint8*) surface->pixels + y*(surface->pitch)+x*bytes, &pixel, bytes);
}

void PixelSurface::putPixelLine32( int x, int y, const Uint32* pixel, int entries )
{
	// Perform clipping
	/// Should truncate clipped line ///
	/// Should parse out 1, 2, 3, and 4 byte lines, not just 4 byte ///
	if( (unsigned int) x >= (unsigned int) surface->w || (unsigned int) x+entries > (unsigned int) surface->w || (unsigned int) y >= (unsigned int) surface->h )
		return;
	size_t bytes = (surface->format->BitsPerPixel+7)>>3;
	memcpy((Uint8*) surface->pixels + y*(surface->pitch)+x*bytes, pixel, bytes*entries);
}

Uint32 PixelSurface::getPixel32( int x, int y )
{
	// Perform clipping
	if( (unsigned int) x >= (unsigned int) surface->w || (unsigned int) y >= (unsigned int) surface->h )
		return 0x0000;
	Uint32 pixel;
	size_t bytes = (surface->format->BitsPerPixel+7)>>3;
	memcpy(&pixel, (Uint8*) surface->pixels + y*(surface->pitch)+x*bytes, bytes);
	return pixel;
}

void PixelSurface::putPixel( int x, int y, SDL_Color color )
{
	putPixel32(x, y, colorToPixel(color));
}

SDL_Color PixelSurface::getPixel( int x, int y )
{
	return pixelToColor(getPixel32(x, y));
}

void PixelSurface::copy( PixelSurface& copySurface, int x, int y )
{
	SDL_Rect dispRect;
	dispRect.x = x;
	dispRect.y = y;
	dispRect.w = copySurface.surface->w;
	dispRect.h = copySurface.surface->h;
	
	if( SDL_BlitSurface(copySurface.surface, NULL, surface, &dispRect) != 0 ) {
		std::cerr << "PixelSurface::copy - Unable to copy surface: " << SDL_GetError() << std::endl;
		exit(1);
	}
		
}

