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


#ifndef _SPEAKER1BIT_H_
#define _SPEAKER1BIT_H_


#define _SPEAKER1BIT_TEST_OUTPUT_

#include <iostream>
#include <cstdlib>
#include <cassert>
#include "SDL.h"
#include "cpu65c02.h"


#define SPEAKER_STATIC
 

class Speaker1bit
{

	static const int SAMPLE_BUFFER_SIZE = 1024;                // Lag of 1/40th to 1/20th of a second at 22050Hz
	static const int CHANNELS = 1;                             // Speaker sound channels (1 for an Apple IIe)
	static const int BITS_PER_SAMPLE = 16;                     // Standard CD quality sound resolution
	static const float MAX_SOUND_WORD = 32767.;                // Min and max limits on sound resolution
	static const float MIN_SOUND_WORD = -32768.;
	static const float SAMPLE_RATE = 22050;                    // 22050 sound words per second (48000 and 96000 are also common)
	static const Sint32 SAMPLE_DURATION = 1000000000/22050;    // Sample length in nanoseconds

	static const int SKIP_CYCLES = 4;  // Granularity of physics simulation (speaker sensitivity)
	                                   // A value of 1 checks for changes every cycle, 4 every 4 cycles
	unsigned int cycleCount;           // Number of cycles played (used to tell which cycles to skip)
	
#ifdef SPEAKER_STATIC
	static const float FRICTION = .01;                                 // 1% acceleration loss per unit velocity
	static const Sint32 CHARGE_DURATION = 20/SKIP_CYCLES;              // 20 cycles
	static const float MAGNET_FORCE = 11.*SKIP_CYCLES*SKIP_CYCLES;     // Magnet acceleration in units distance per increment
	static const float SPRING_FORCE = .00049*SKIP_CYCLES*SKIP_CYCLES;  // Units reverse spring acceleration per unit distance per increment
#else
	// Remove typical Apple IIe speaker static
	static const float FRICTION = .02;        
	static const Sint32 CHARGE_DURATION = 10/SKIP_CYCLES; 
	static const float MAGNET_FORCE = 34*SKIP_CYCLES*SKIP_CYCLES;
	static const float SPRING_FORCE = .0002*SKIP_CYCLES*SKIP_CYCLES;
#endif

	float pos;                  // Between (-32768) - 32767 for sampling
	float vel;                  // Diafragm velocity in units per cycle
	bool toggleChargeNegative;  // Indicates charge of next magnet toggle
	float charge;               // Indicates current magnet force (+/- MAGNET_FORCE)
	Sint32 chargeDur;           // Cycles remaining for current charge

	float sampleSum;      // Keeps track of sample sum for averaging
	int sampleTotal;      // Number of consecutive samples in sampleSum
	Sint32 sampleLength;  // Number of nanoseconds per sound sample

	bool samplePending;   // Indicates if sound sample is waiting to be processed
	Uint16 soundSample;   // Sample to be processed
	
	void _commitSoundWord( Sint16 word );
	
public:

	Speaker1bit();

	~Speaker1bit();
	
	void toggle();
		// Add a speaker toggle to the simulation at current time step
		// Time step must be incremented appropriately before and after call using "cycle"

	void cycle( Sint32 elapsedNanoseconds );
		// Commits 1 CPU cycle to sound buffer
		// Should be called in order, allowing other emulation routines to finish their cycles in turn
	
	bool pollSound( Uint16& sample );
		// Returns true if a sound word is pending (stored to "sample"), false otherwise
	
};


#endif /* _SPEAKER1BIT_H_ */
