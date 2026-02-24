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


#ifndef _SOUND_H_
#define _SOUND_H_


#include <iostream>
#include <cassert>
#include "SDL.h"
#include "SDL_image.h"


// Uncomment this line to show currently playing waveform data when in console mode
//#define _SOUND_TEST_OUTPUT_


class SoundBuffer
{

	static const int SAMPLE_BUFFER_SIZE = 512;                 // Lag of ~1/30th of a second at 22050Hz
	static const int SAMPLE_PAGES = 3;
	static const int SAMPLE_BYTES = 2;                         // Number of bytes per sample
	static const int CHANNELS = 1;                             // Speaker sound channels (1 for an Apple IIe)
	static constexpr float MAX_SOUND_WORD = 32767.;                // Min and max limits on sound resolution
	static constexpr float MIN_SOUND_WORD = -32768.;
	static constexpr float SAMPLE_RATE = 22050;                    // 22050 sound words per second
	static const Sint32 SAMPLE_DURATION = 1000000000/22050;    // Sample length in nanoseconds
	
	SDL_AudioStream* stream;

public:

	SoundBuffer();

	~SoundBuffer();

	void playSample( Sint16 sample );
		
};


#endif  // _SOUND_H_
