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
	static bool instance = false;
	if( instance == true ) {
		std::cerr << "Multiple event manager instances requested. For maximum portability, only one interface is supported per application.\n";
		exit(1);
	}

	if( SDL_Init( SDL_INIT_TIMER | SDL_INIT_VIDEO ) ) {
		std::cerr << "Unable to initialize SDL: " << SDL_GetError() << std::endl;
		exit(1);
	}
	atexit(SDL_Quit);

	SDL_WM_SetCaption(caption, caption);
	SDL_Surface * icon = NULL;
//	if( iconFile != NULL )
//		icon = IMG_Load(iconFile);
//	if( icon != NULL )
//		SDL_WM_SetIcon(icon, NULL);

	// Set mode
	displaySurface = NULL;
	displaySurfaceClass = NULL;
	resizeDisplay(xSize, ySize, bpp, fullScreen);
	atexit(showCursor);
	instance = true;

	displaySurfaceClass = new PixelSurface(displaySurface, true);
	
	keyState = SDL_GetKeyState(&keyStateTotal);
	
}

EventManager::~EventManager()
{
	if( displaySurface == NULL )
		return;
	Uint32 pixel = SDL_MapRGB(displaySurface->format, 0, 0, 0);
	SDL_LockSurface(displaySurface);
	SDL_FillRect(displaySurface, NULL, pixel);
	SDL_UnlockSurface(displaySurface);
	SDL_Flip(displaySurface);
	SDL_Delay(67);  // Wait 1/15th of a second for screen refresh
	SDL_FreeSurface(displaySurface);
}

void EventManager::resizeDisplay( int xSize, int ySize, int bpp, bool fullScreen )
{
	if( displaySurface!=NULL )
		SDL_FreeSurface(displaySurface);
	Uint32 flags = SDL_SWSURFACE | (fullScreen ? SDL_FULLSCREEN : 0); 
	if( bpp == 0 ) {
		bpp = 32;
		flags |= SDL_ANYFORMAT;
	}
	displaySurface = SDL_SetVideoMode(xSize, ySize, bpp, flags);
	if( displaySurface == NULL ) {
		std::cerr << "Unable to initialize video display: " << SDL_GetError() << std::endl;
		exit(1);
	}
	if( displaySurface->w != xSize || displaySurface->h != ySize ) {
		std::cerr << "Unable to apply requested video surface size: " << xSize << "x" << ySize << std::endl;
		exit(1);
	}
	if( displaySurface->format->BitsPerPixel == 8 ) {
		displaySurface = SDL_SetVideoMode(xSize, ySize, 32, flags&(~SDL_ANYFORMAT));
		if( displaySurface == NULL || displaySurface->format->BitsPerPixel == 8 || displaySurface->w != xSize || displaySurface->h != ySize ) {
			std::cerr << "Unable to set desired resolution.\n";
			exit(1);
		}
	}
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
	const SDL_VideoInfo *videoInfo = SDL_GetVideoInfo();
	xSize = videoInfo->current_w;
	ySize = videoInfo->current_h;
	bpp = videoInfo->vfmt->BitsPerPixel;
}

void EventManager::videoRefresh()
{
	SDL_Flip(displaySurface);
}

void EventManager::hideCursor()
{
	SDL_ShowCursor(SDL_DISABLE);
}

void EventManager::showCursor()
{
	SDL_ShowCursor(SDL_ENABLE);
}

void EventManager::consoleDisplay()
{
	freopen("CON","w",stdout);
	freopen("CON","w",stderr);
}

Uint32 EventManager::getClock()
{
	return SDL_GetTicks();
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

bool EventManager::isPressed( SDLKey key )
{
	// Error checking
	assert( (Uint32) key < (Uint32) keyStateTotal );

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
			return modState & KMOD_LMETA;
			break;
			
		case SDLK_RMETA:
			return modState & KMOD_RMETA;
			break;
			
		case SDLK_NUMLOCK:
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

	return keyState[key];

}

