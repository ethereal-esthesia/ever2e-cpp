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
 

#include "speaker1bit.h"


using namespace std;


void Speaker1bit::_commitSoundWord( Sint16 word )
{	
	samplePending = true;
	soundSample = word;
}

Speaker1bit::Speaker1bit()
{

	pos = 0.;
	vel = 0.;
	toggleChargeNegative = false;
	charge = 0.;
	chargeDur = 0;
	
	sampleSum = 0.;
	sampleTotal = 0;
	sampleLength = 0;

	samplePending = false;
	soundSample = 0;
	cycleCount = 0;
	
}

Speaker1bit::~Speaker1bit()
{	
	SDL_CloseAudio();
}	

void Speaker1bit::toggle()
{
	chargeDur = CHARGE_DURATION;
	charge = toggleChargeNegative ? -MAGNET_FORCE : MAGNET_FORCE;
	toggleChargeNegative = !toggleChargeNegative;
}

void Speaker1bit::cycle( Sint32 elapsedNanoseconds )
{
	sampleLength += elapsedNanoseconds;

	// Slow down cycle feed to avoid needlessly ramping up CPU
	// Speaker can only be toggled at most approx every 4 cycles (255,000 Hz) which is the value allowed here
	// This speed provides about 12 averaged physics calculations per sound sample
	if( (cycleCount++)%SKIP_CYCLES )
		return;

	// Check for underflow and set variables to 0 as needed to speed up math routines in idle state
	/// It may be slightly faster to use fixed point integers ///
	if( pos>-1./32768. && pos<1./32767. )
		pos = 0.;
	if( vel>-1./32768. && vel<1./32767. )
		vel = 0.;

	// Move diafragm
	float accel = -SPRING_FORCE*pos;

	accel += charge;
	accel -= vel*FRICTION;
	vel += accel;
	pos += vel;

	// Check magnet-charge duration expiration
	if( chargeDur>0 ) {
		chargeDur--;
		if( !chargeDur )
			charge = 0.;
	}

#ifdef _SPEAKER1BIT_TEST_OUTPUT_
	if( pos<-100000. || pos>100000. ) {
		cerr << "Warning: diafragm position far exceeds limits:\n";
		cerr << "pos " << pos << ", vel " << vel << ", charge " << charge << ", accel " << accel << " chargeDur: " << chargeDur << endl;
	}
#endif

	// Apply diafragm position to average for sampling and commit sample when necessary
	sampleTotal++;
	sampleSum += pos;

	if( sampleLength >= SAMPLE_DURATION ) {

		sampleSum /= (float)sampleTotal;
		sampleSum = sampleSum>MAX_SOUND_WORD ? MAX_SOUND_WORD:sampleSum;
		sampleSum = sampleSum<MIN_SOUND_WORD ? MIN_SOUND_WORD:sampleSum;

		_commitSoundWord(Sint16(sampleSum));
		sampleLength -= SAMPLE_DURATION;
		
		sampleSum = 0.;
		sampleTotal = 0;

	}

}

bool Speaker1bit::pollSound( Uint16& sample )
{
	if( samplePending ) {
		sample = soundSample;
		samplePending = false;
		return true;
	}
	else
		return false;
}

