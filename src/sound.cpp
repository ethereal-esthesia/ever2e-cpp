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


#include "sound.h"


using namespace std;


SoundBuffer::SoundBuffer()
{
	static bool instance = false;
	if( instance == true ) {
		cerr << "Multiple sound manager instances requested. Mixing is not currently supported.\n";
		stream = NULL;
		return;
	}
	instance = true;

	SDL_AudioSpec audioSpec;
	audioSpec.format = SDL_AUDIO_S16;
	audioSpec.channels = CHANNELS;
	audioSpec.freq = (int) SAMPLE_RATE;

	stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &audioSpec, NULL, NULL);
	if( stream == NULL ) {
		cerr << "Unable to open audio channel: " << SDL_GetError() << endl;
		exit(1);
	}
	if( !SDL_ResumeAudioStreamDevice(stream) ) {
		cerr << "Unable to start audio channel: " << SDL_GetError() << endl;
		exit(1);
	}
}

SoundBuffer::~SoundBuffer()
{
	if( stream != NULL )
		SDL_DestroyAudioStream(stream);
}

void SoundBuffer::playSample( Sint16 sample )
{
	if( stream == NULL )
		return;
	SDL_PutAudioStreamData(stream, &sample, SAMPLE_BYTES);
}
