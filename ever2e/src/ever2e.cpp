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

#include <fstream>
#include "eventloop.h"


using namespace std;

char* InputFileName;

bool hostCycle( EventLoop *emulator )
{

	static ifstream::pos_type size = -1;
	static int ptr = 0;
	static char* buffer = NULL;
	static char lc = 0;
	static bool firstChar = true;
	
	while( ptr<size && (buffer[ptr]<0x20 || buffer[ptr]>=0x80) && buffer[ptr]!=0x0d )
		ptr++;

	//	if( ptr == size ) {
	if( size == -1 ) {
//		if( emulator->keyboard->getStrobe() == 0x96 ) {
//			emulator->keyboard->setKeyboard(0x8D);
		ifstream file;
		file.open(InputFileName, ios::binary);
		if( !file.is_open() ) {
			cerr << "Input file not found: \"" << InputFileName << "\"\n";
			exit(1);
		}
		file.seekg(0, ios::end);   
		size = file.tellg();
		delete [] buffer;
		buffer = new char[(int)size+1];
//		buffer[0] = ' ';
		file.seekg(0, ios::beg);
		file.read(buffer, size);
		file.close();
		ptr = 0;
	}
	else if( ptr < size ) {
	
		if( (int) emulator->memory->getMem(0xc000) != (lc|0x80) ) {
cout << (int)emulator->memory->getMem(0xc000) << " != " << (lc|0x80) << endl;
			lc = buffer[ptr];
			emulator->keyboard->keyPress(lc);
			emulator->keyboard->keyRelease(lc);
			if( !firstChar )
				ptr++;
			else
				firstChar = false;
		}

	}
///	else
///		emulator->setKeySupress(false);
		
	return false;
	
}

int main( int args, char** argv )
{

	EventLoop emulator;

	if( args>1 ) {
		InputFileName = argv[1];
		emulator.incorporate(hostCycle, true);
	}

	do
		emulator.cycle();
	while( !emulator.getExitStatus() );
	
	return 0;

}
