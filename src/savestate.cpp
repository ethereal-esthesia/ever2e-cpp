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


#include "savestate.h"


using namespace std;


SaveState::SaveState()
{
	checksum = 0;
}

SaveState::~SaveState()
{	
}

void SaveState::write8( Uint8 value )
{
	data.put(value);
	checksum += value ^ TAG_8;
}

void SaveState::write16( Uint16 value )
{
	Uint16 sVal = SWAP16(value);
	data.write((const char*) &sVal, 2);
	checksum += value ^ TAG_16;
}

void SaveState::write32( Uint32 value )
{
	Uint32 sVal = SWAP32(value);
	data.write((const char*) &sVal, 4);
	checksum += value ^ TAG_32;
}

void SaveState::write64( Uint64 value )
{
	Uint64 sVal = SWAP64(value);
	data.write((const char*) &sVal, 8);
	checksum += value ^ TAG_64;
}

void SaveState::writeBool( bool value )
{
	Uint8 sVal = value && 1;
	data.put(sVal);
	checksum += sVal ^ TAG_BOOL;
}

void SaveState::writeFloat( float value )
{	
	Uint32 byteVal = *(Uint32*)(void*)(&value);
	byteVal = SWAP32(byteVal);
	data.write((const char*) &byteVal, 4);
	checksum += byteVal ^ TAG_FLOAT;
}

void SaveState::writeDouble( double value )
{
	Uint64 byteVal = *(Uint64*)(void*)(&value);
	byteVal = SWAP64(byteVal);
	data.write((const char*) &byteVal, 8);
	checksum += byteVal ^ TAG_DOUBLE;
}

void SaveState::writeString( string str )
{
	Uint32 length = str.length();
	write32(length);
	data.write(str.c_str(), length);
	for( Uint32 i = 0; i<length; i++ )
		checksum += str[i] ^ TAG_STRING;
}

void SaveState::writeCString( const char* str, Uint32 size )
{
	data.write(str, size);
	for( Uint32 i = 0; i<size; i++ )
		checksum += str[i] ^ TAG_C_STRING;
}

void SaveState::startChecksum()
{
	checksum = 0;
}

void SaveState::writeChecksum()
{
	Uint64 value = SWAP64(checksum);
	data.write((char*) &value, 8);
}

int SaveState::verifyChecksum()
{
	Uint64 value;
	data.read((char*) &value, 8);
	value = SWAP64(value);
	return checksum != value;
}

Uint8 SaveState::read8()
{
	Uint8 value = data.get();
	checksum += value ^ TAG_8;
	return value;
}

Uint16 SaveState::read16()
{
	Uint16 value;
	data.read((char*) &value, 2);
	value = SWAP16(value);
	checksum += value ^ TAG_16;
	return value;
}

Uint32 SaveState::read32()
{
	Uint32 value;
	data.read((char*) &value, 4);
	value = SWAP32(value);
	checksum += value ^ TAG_32;
	return value;
}

Uint64 SaveState::read64()
{
	Uint64 value;
	data.read((char*) &value, 8);
	value = SWAP64(value);
	checksum += value ^ TAG_64;
	return value;
}

bool SaveState::readBool()
{
	bool value = (bool) data.get();
	checksum += value ^ TAG_BOOL;
	return value;
}

float SaveState::readFloat()
{
	Uint32 byteVal;
	data.read((char*) &byteVal, 4);
	byteVal = SWAP32(byteVal);
	checksum += byteVal ^ TAG_FLOAT;
	return *(float*)(void*)(&byteVal);
}

double SaveState::readDouble()
{
	Uint64 byteVal;
	data.read((char*) &byteVal, 8);
	byteVal = SWAP64(byteVal);
	checksum += byteVal ^ TAG_DOUBLE;
	return *(double*)(void*)(&byteVal);
}

string SaveState::readString()
{
	Uint32 stringSize = read32();
	ostringstream out;
	while( stringSize-- ) {
		char chr = data.get();
		checksum += chr ^ TAG_STRING;
		out << chr;
	}
	return out.str();
}

void SaveState::readCString( char* str, Uint32 size )
{
	data.read(str, size);
	for( Uint32 i = 0; i<size; i++ )
		checksum += str[i] ^ TAG_C_STRING;
}

string SaveState::getInfo()
{
	/// STUB ///
	return "";
}

streamsize SaveState::getPosition()
{
	return data.tellg();
}

void SaveState::clear()
{
	data.str("");
}
