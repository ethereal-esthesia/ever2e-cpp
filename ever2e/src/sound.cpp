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


void SoundBuffer::_audioCallback( void *dataPtr, Uint8 *stream, int size )
{

	// Error checking
	assert(SAMPLE_BUFFER_SIZE*SAMPLE_BYTES == size);
	
	BufferData *bufferData = (BufferData*) dataPtr;
	int writePage = (bufferData->writePtr / SAMPLE_BUFFER_SIZE);

	if( bufferData->readPage == writePage ) {
		// Fill audio with a pause at current speaker level to allow time to resolve sound buffer read/write conflict
		memset(stream, (bufferData->readPage+1)*size-1, size);
		bufferData->skipCount++;
	}
	else {
		// Copy read page to stream and set next read page to current write page
		memcpy(stream, &(bufferData->sampleBuffer[SAMPLE_BUFFER_SIZE*bufferData->readPage]), size);
		bufferData->readPage++;
		bufferData->readPage %= SAMPLE_PAGES;
	}

	bufferData->sampleReadCount += SAMPLE_BUFFER_SIZE;
	bufferData->syncFlag = true;
		
}

void SoundBuffer::_quitSound()
{
	SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

SoundBuffer::SoundBuffer()
{

	static bool instance = false;
	if( instance == true ) {
		cerr << "Multiple sound manager instances requested. Mixing is not currently supported.\n";
		return;
	}
	instance = true;

	if( !SDL_WasInit(SDL_INIT_AUDIO) )
		if( SDL_InitSubSystem(SDL_INIT_AUDIO) != 0 ) {
			cerr << "Error initializing sound: " << SDL_GetError() << endl;
			exit(1);
			atexit(_quitSound);
		}

	bufferData.sampleBuffer = new Sint16[SAMPLE_BUFFER_SIZE*SAMPLE_PAGES];
	for( int k = 0; k<SAMPLE_BUFFER_SIZE*SAMPLE_PAGES; k++ )
		bufferData.sampleBuffer[k] = 0;
	bufferData.writePtr = 0;
	bufferData.readPage = 1;
	bufferData.locked = false;
	bufferData.skipCount = 0;
	bufferData.sampleReadCount = 0;
	bufferData.syncFlag = false;
	SDL_AudioSpec audioSpec;

	audioSpec.freq = SAMPLE_RATE;
	audioSpec.format = AUDIO_S16SYS;
	audioSpec.channels = 1;
	audioSpec.samples = SAMPLE_BUFFER_SIZE;
	audioSpec.callback = _audioCallback;
	audioSpec.userdata = (void*) &bufferData;
	
	// Start sound channel
	if( SDL_OpenAudio(&audioSpec, &obtainedSpec) ) {
		cerr << "Unable to open audio channel: " << SDL_GetError() << endl;
		exit(1);
	}
	SDL_PauseAudio(0);

#ifdef _SOUND_TEST_OUTPUT_
	hPos = 0;
	sum = 0;
	sampleCount = 0;	
#endif

}

SoundBuffer::~SoundBuffer()
{
	SDL_CloseAudio();
}

void SoundBuffer::playSample( Sint16 sample )
{

	SDL_LockAudio();

	// Commit word to sound buffer looping back to first page if necessary
	bufferData.sampleBuffer[bufferData.writePtr] = sample;
	bufferData.writePtr++;
	if( bufferData.writePtr == SAMPLE_BUFFER_SIZE*SAMPLE_PAGES )
		bufferData.writePtr = 0;

	SDL_UnlockAudio();

#ifdef _SOUND_TEST_OUTPUT_

	// Visualize sound wave via console output
	sampleCount++;
	hPos++;
	
	if( (hPos&0x07) > 0 ) {
		sum += sample;
		return;
	}
	
	sum >>= 3;

	if( sum < 0 )
		cout << 'v';
	else if( sum > 0 )
		cout << '^';
	else
		cout << '-';

	if( hPos >= 735 ) {
		hPos = 0;
		cout << " (" << bufferData.skipCount << " skips) (lag: " << int(bufferData.sampleReadCount/441.-sampleCount/441.)/100. << ")    \r";
	}
	
	sum = 0;

#endif  // _SOUND_TEST_OUTPUT_

}

