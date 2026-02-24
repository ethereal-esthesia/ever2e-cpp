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
 

#include "manager.h"


using namespace std;


EventManager::EventManager( int xSize, int ySize, int bpp, bool fullScreen, const char* caption, const char* iconFile )
{	
	(void) iconFile;
	static bool instance = false;
	if( instance == true ) {
		std::cerr << "Multiple event manager instances requested. For maximum portability, only one interface is supported per application.\n";
		exit(1);
	}

	if( !SDL_Init( SDL_INIT_VIDEO ) ) {
		std::cerr << "Unable to initialize SDL: " << SDL_GetError() << std::endl;
		exit(1);
	}
	atexit(SDL_Quit);

	// Set mode
	window = SDL_CreateWindow(caption, xSize, ySize, SDL_WINDOW_RESIZABLE);
	if( window == NULL ) {
		std::cerr << "Unable to create window: " << SDL_GetError() << std::endl;
		exit(1);
	}
	displaySurface = NULL;
	displaySurfaceClass = NULL;
	resizeDisplay(xSize, ySize, bpp, fullScreen);
	atexit(showCursor);
	instance = true;

	displaySurfaceClass = new PixelSurface(displaySurface, true);
}

EventManager::~EventManager()
{
	if( displaySurface == NULL )
		return;
	Uint32 pixel = SDL_MapSurfaceRGB(displaySurface, 0, 0, 0);
	SDL_LockSurface(displaySurface);
	SDL_FillSurfaceRect(displaySurface, NULL, pixel);
	SDL_UnlockSurface(displaySurface);
	SDL_UpdateWindowSurface(window);
	SDL_Delay(67);  // Wait 1/15th of a second for screen refresh
	if( window != NULL )
		SDL_DestroyWindow(window);
}

void EventManager::resizeDisplay( int xSize, int ySize, int bpp, bool fullScreen )
{
	if( bpp == 0 )
		bpp = 32;
	if( !SDL_SetWindowFullscreen(window, fullScreen) ) {
		std::cerr << "Unable to set fullscreen state: " << SDL_GetError() << std::endl;
		exit(1);
	}
	if( !fullScreen && !SDL_SetWindowSize(window, xSize, ySize) ) {
		std::cerr << "Unable to set window size: " << SDL_GetError() << std::endl;
		exit(1);
	}
	displaySurface = SDL_GetWindowSurface(window);
	if( displaySurface == NULL ) {
		std::cerr << "Unable to initialize video display: " << SDL_GetError() << std::endl;
		exit(1);
	}
	if( displaySurface->w != xSize || displaySurface->h != ySize ) {
		std::cerr << "Unable to apply requested video surface size: " << xSize << "x" << ySize << std::endl;
		exit(1);
	}
	if( SDL_BITSPERPIXEL(displaySurface->format) == 8 ) {
		if( !SDL_SetWindowSize(window, xSize, ySize) ) {
			std::cerr << "Unable to set desired resolution.\n";
			exit(1);
		}
		displaySurface = SDL_GetWindowSurface(window);
		if( displaySurface == NULL || SDL_BITSPERPIXEL(displaySurface->format) == 8 || displaySurface->w != xSize || displaySurface->h != ySize ) {
			std::cerr << "Unable to set desired resolution.\n";
			exit(1);
		}
	}
	if( displaySurfaceClass != NULL )
		displaySurfaceClass->setSurface(displaySurface);

	// Clear the full host window surface so regions outside the emulated raster
	// start from a known black background.
	Uint32 blackPixel = SDL_MapSurfaceRGB(displaySurface, 0, 0, 0);
	SDL_FillSurfaceRect(displaySurface, NULL, blackPixel);
	SDL_UpdateWindowSurface(window);

	if( fullScreen ) 
		hideCursor();
	else
		showCursor();
	// If this is not the first time setting up the display, pause to allow the OS time to syncronize relevant settings
	if( displaySurfaceClass==NULL )
		SDL_Delay(250);
}

PixelSurface* EventManager::getVideoSurface()
{
	return displaySurfaceClass;
}

void EventManager::getVideoParam( int &xSize, int &ySize, int &bpp )
{
	SDL_DisplayID displayId = SDL_GetPrimaryDisplay();
	const SDL_DisplayMode *mode = SDL_GetCurrentDisplayMode(displayId);
	if( mode == NULL ) {
		xSize = displaySurface ? displaySurface->w : 0;
		ySize = displaySurface ? displaySurface->h : 0;
		bpp = displaySurface ? SDL_BITSPERPIXEL(displaySurface->format) : 0;
		return;
	}
	xSize = mode->w;
	ySize = mode->h;
	bpp = SDL_BITSPERPIXEL(mode->format);
}

void EventManager::videoRefresh()
{
	SDL_Surface* latestSurface = SDL_GetWindowSurface(window);
	if( latestSurface != NULL && latestSurface != displaySurface ) {
		displaySurface = latestSurface;
		if( displaySurfaceClass != NULL )
			displaySurfaceClass->setSurface(displaySurface);
	}
	SDL_UpdateWindowSurface(window);
}

void EventManager::hideCursor()
{
	SDL_HideCursor();
}

void EventManager::showCursor()
{
	SDL_ShowCursor();
}

void EventManager::consoleDisplay()
{
	freopen("CON","w",stdout);
	freopen("CON","w",stderr);
}

Uint32 EventManager::getClock()
{
	return (Uint32) SDL_GetTicks();
}

const SDL_Event* EventManager::popEvent()
{
	if( !SDL_PollEvent(&event) ) {
		modState = SDL_GetModState();
		return NULL;
	}
	return (const SDL_Event*) &event;
}

const SDL_Event* EventManager::peekEvent()
{
	if( popEvent()==NULL )
		return NULL;
	SDL_PushEvent(&event);
	return (const SDL_Event*) &event;
}

void EventManager::refreshEventStatus()
{
	SDL_PumpEvents();
	modState = SDL_GetModState();
}

bool EventManager::isPressed( SDL_Keycode key )
{
	switch( key )
	{
	
		case SDLK_LSHIFT:
			return modState & KMOD_LSHIFT;

		case SDLK_RSHIFT:
			return modState & KMOD_RSHIFT;
			break;
			
		case SDLK_LCTRL:
			return modState & KMOD_LCTRL;
			break;

		case SDLK_RCTRL:
			return modState & KMOD_RCTRL;
			break;
			
		case SDLK_LALT:
			return modState & KMOD_LALT;
			break;
			
		case SDLK_RALT:
			return modState & KMOD_RALT;
			break;

		case SDLK_LMETA:
			return modState & KMOD_LGUI;
			break;
			
		case SDLK_RMETA:
			return modState & KMOD_RGUI;
			break;
			
		case SDLK_NUMLOCKCLEAR:
			return modState & KMOD_NUM;
			break;
			
		case SDLK_CAPSLOCK:
			return modState & KMOD_CAPS;
			break;
			
		case SDLK_MODE:
			return modState & KMOD_MODE;
			break;
	
		default:
			break;

	}

	SDL_Keymod keymod = SDL_KMOD_NONE;
	SDL_Scancode scancode = SDL_GetScancodeFromKey(key, &keymod);
	if( scancode == SDL_SCANCODE_UNKNOWN )
		return false;
	int keyCount = 0;
	const bool* keyState = SDL_GetKeyboardState(&keyCount);
	if( keyState == NULL || scancode >= keyCount )
		return false;
	return keyState[scancode];

}

