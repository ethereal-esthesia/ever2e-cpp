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
	SDL_PixelFormat format;
	switch( bpp ) {

	case 8:
		format = SDL_PIXELFORMAT_INDEX8;
		break;

	case 15:
		format = SDL_PIXELFORMAT_RGB555;
		break;

	case 16:
		format = SDL_PIXELFORMAT_RGB565;
		break;

	case 24:
		format = SDL_PIXELFORMAT_RGB24;
		break;

	case 32:
		format = SDL_PIXELFORMAT_ARGB8888;
		break;

	default:
		std::cerr << "PixelSurface::_newSurface - " << bpp << " bits-per-pixel not supported\n";
		exit(1);

	}
	surface = SDL_CreateSurface(xSize, ySize, format);
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
	if( !locked )
		SDL_DestroySurface(surface);
}

void PixelSurface::setPalette( SDL_Color *palette, int entries )
{	
	SDL_Palette *surfacePalette = SDL_GetSurfacePalette(surface);
	if( surfacePalette == NULL )
		surfacePalette = SDL_CreateSurfacePalette(surface);
	if( surfacePalette == NULL || !SDL_SetPaletteColors(surfacePalette, palette, 0, entries) ) {
		std::cerr << "PixelSurface::setPalette - Unable to set palette - " << SDL_GetError() << std::endl;
		exit(1);
	}
}

void PixelSurface::setGreyPalette()
{
	SDL_Color greyPal[0x100];

	// Set up gray scale palette
	for( int k = 0; k<0x100; k++ ) {
		greyPal[k].r = k;
		greyPal[k].g = k;
		greyPal[k].b = k;
		greyPal[k].a = SDL_ALPHA_OPAQUE;
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
	return SDL_BITSPERPIXEL(surface->format);
}

void PixelSurface::fillSurface( SDL_Color color )
{
	Uint32 pixel = SDL_MapSurfaceRGB(surface, color.r, color.g, color.b);
	SDL_FillSurfaceRect(surface, NULL, pixel);
}

void PixelSurface::resize( int xSize, int ySize )
{	
	assert( !locked );
	/// STUB ///
	SDL_DestroySurface(surface);
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
//	surface = IMG_Load(fileName);
	if( surface==NULL ) {
		std::cerr << "PixelSurface::PixelSurface - Unable to load file: \"" << fileName << "\"\n";
		exit(1);
	}
}

Uint32 PixelSurface::colorToPixel( SDL_Color color )
{
	Uint32 pixel;
	/// STUB
	if( SDL_BITSPERPIXEL(surface->format)==8 )
		pixel = ((Uint16)color.r+color.g+color.b)/3;
	else
	///
	pixel = SDL_MapSurfaceRGB(surface, color.r, color.g, color.b);
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
	const SDL_PixelFormatDetails *formatDetails = SDL_GetPixelFormatDetails(surface->format);
	SDL_GetRGB(pixel, formatDetails, SDL_GetSurfacePalette(surface), &color.r, &color.g, &color.b);
	color.a = SDL_ALPHA_OPAQUE;
	return color;
}

void PixelSurface::putPixel32( int x, int y, Uint32 pixel )
{
	// Perform clipping
	if( (unsigned int) x >= (unsigned int) surface->w || (unsigned int) y >= (unsigned int) surface->h )
		return;
	size_t bytes = (SDL_BITSPERPIXEL(surface->format)+7)>>3;
	memcpy((Uint8*) surface->pixels + y*(surface->pitch)+x*bytes, &pixel, bytes);
}

void PixelSurface::putPixelLine32( int x, int y, const Uint32* pixel, int entries )
{
	// Perform clipping
	/// Should truncate clipped line ///
	/// Should parse out 1, 2, 3, and 4 byte lines, not just 4 byte ///
	if( (unsigned int) x >= (unsigned int) surface->w || (unsigned int) x+entries > (unsigned int) surface->w || (unsigned int) y >= (unsigned int) surface->h )
		return;
	size_t bytes = (SDL_BITSPERPIXEL(surface->format)+7)>>3;
	memcpy((Uint8*) surface->pixels + y*(surface->pitch)+x*bytes, pixel, bytes*entries);
}

Uint32 PixelSurface::getPixel32( int x, int y )
{
	// Perform clipping
	if( (unsigned int) x >= (unsigned int) surface->w || (unsigned int) y >= (unsigned int) surface->h )
		return 0x0000;
	Uint32 pixel;
	size_t bytes = (SDL_BITSPERPIXEL(surface->format)+7)>>3;
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
	
	if( !SDL_BlitSurface(copySurface.surface, NULL, surface, &dispRect) ) {
		std::cerr << "PixelSurface::copy - Unable to copy surface: " << SDL_GetError() << std::endl;
		exit(1);
	}
		
}

void PixelSurface::setSurface( SDL_Surface* newSurface )
{
	if( !locked ) {
		std::cerr << "PixelSurface::setSurface - Dynamic surface swap requires locked surface mode.\n";
		exit(1);
	}
	if( newSurface == NULL ) {
		std::cerr << "PixelSurface::setSurface - Invalid surface given.\n";
		exit(1);
	}
	surface = newSurface;
}

