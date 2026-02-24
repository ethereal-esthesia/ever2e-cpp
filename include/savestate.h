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


#ifndef _SAVESTATE_H_
#define _SAVESTATE_H_


#include <iostream>
#include <string>
#include <sstream>
#include "SDL_stdinc.h"
#include "SDL_endian.h"


#if SDL_BYTEORDER == SDL_LIL_ENDIAN
#define SWAP16(X)    (X)
#define SWAP32(X)    (X)
#define SWAP64(X)    (X)
#else
#define SWAP16(X)    SDL_Swap16(X)
#define SWAP32(X)    SDL_Swap32(X)
#define SWAP64(X)    SDL_Swap64(X)
#endif


class SaveState
{

	std::stringstream data;
	Uint64 checksum;

	static const Uint8  TAG_8        = 0x18;
	static const Uint16 TAG_16       = 0xa596;
	static const Uint32 TAG_32       = 0x2566ff41;
	static const Uint64 TAG_64       = 0x6d164dc29c1eedcdull;
	static const Uint8  TAG_BOOL     = 0x28;
	static const Uint32 TAG_FLOAT    = 0x4441233a;
	static const Uint64 TAG_DOUBLE   = 0xd88a9f12e4631fe1ull;
	static const Uint8  TAG_STRING   = 0x44;
	static const Uint8  TAG_C_STRING = 0xed;

public:

	SaveState();

	~SaveState();

	// Values that do not have an explicitly guaranteed size (int, short, etc.) can be stored using smallest-case estimates
	// According to the C99 specification, it is safe to assume the following minimum sizes:
	//     type     bits
	//     char        8
	//     short      16
	//     int        16
	//     long       32
	//     long long  64
	
	void write8( Uint8 value );
		// Write any 8 bit value to state stream (Uint8, Sint8, unsigned char, char)

	void write16( Uint16 value );
		// Write any 16-bit integer value to state stream (Uint16, Sint16)
	
	void write32( Uint32 value );
		// Write any 32-bit integer value to state stream (Uint32, Sint32)
	
	void write64( Uint64 value );
		// Write any 64-bit integer value to state stream (Uint64, Sint64)

	void writeBool( bool value );

	void writeFloat( float value );
		// Write float value to state stream

	void writeDouble( double value );
		// Write double value to state stream

	void writeString( std::string str );
		// Write string to state stream

	void writeCString( const char* str, Uint32 size );
		// Copies c-string data to "str"

	void startChecksum();
		// Begins checksum cycle
		// Must be called before subsequent calls to "readChecksum" or "writeChecksum" in a given data block
		// "storeChecksum()" is used after all writes are complete to store checksum information in the data stream
		// "verifyChecksum()" is used after all reads are complete to verify data integrity

	void writeChecksum();
		// Stores checksum information to data stream
	
	int verifyChecksum();
		// Used after all reads are complete to check data integrity
		// Returns 0 if all reads were successful since last call to "startChecksum"
		// Returns 1 if reads were performed out of sequence, with different data types,
		//   or if any of the data has been altered since values were stored

	// The same routine used to store values to a state stream should be used to retrieve them keeping the original order in mind

	Uint8 read8();
		// Returns any 8 bit value from state stream (Uint8, Sint8, unsigned char, char)
	
	Uint16 read16();
		// Returns any 16-bit integer value from state stream (Uint16, Sint16)
	
	Uint32 read32();
		// Returns any 32-bit integer value from state stream (Uint32, Sint32)
	
	Uint64 read64();
		// Returns any 64-bit integer value from state stream (Uint64, Sint64)
	
	bool readBool();
	
	float readFloat();
		// Returns float value from state stream

	double readDouble();
		// Returns double value from state stream

	std::string readString();
		// Returns string value from state stream
	
	void readCString( char* str, Uint32 size );
		// Copies c-string data to "array"
	
	std::string getInfo();
		// Returns a string containing human-readable tag and version information
		//   as well as total structure size in bytes

	std::streamsize getPosition();

	void clear();
	
};
	

#endif  // _SAVESTATE_H_
