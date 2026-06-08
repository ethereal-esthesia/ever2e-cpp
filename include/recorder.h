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


#ifndef _RECORDER_H_
#define _RECORDER_H_


#include <cstddef>
#include <string>
#include "SDL.h"


class MediaRecorder
{

public:

	struct Config
	{
		std::string path;
		int width;
		int height;
		int fps;
		int audioSampleRate;
		int videoQueueFrames;
		int audioChunkSamples;
	};

	MediaRecorder( const Config& config );

	~MediaRecorder();

	bool start();

	void stop();

	bool isRunning() const;

	void captureVideoFrame( SDL_Surface* source, int sourceX, int sourceY );

	void captureAudioSample( Sint16 sample );

	size_t getDroppedVideoFrames() const;

	size_t getDroppedAudioSamples() const;

private:

	struct Impl;
	Impl* impl;

};


#endif  // _RECORDER_H_
