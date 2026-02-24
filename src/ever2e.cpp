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
#include <string>
#include "eventloop.h"


using namespace std;

const char* InputFileName = NULL;
string GuestCoreDumpFile;
bool PrintTextAtExit = false;

char transliterateText( Uint8 ascii )
{
	ascii &= 0x7f;
	if( ascii<0x20 || ascii==0x7f )
		ascii = 0x7e;
	return (char) ascii;
}

void printTextScreen( EventLoop* emulator )
{
	int page = emulator->memory->getSwitch(Memory128k::_PAGE2) ? 2:1;
	cout << "text_screen_begin\n";
	for( int y = 0; y<24; y++ ) {
		string line;
		line.reserve(40);
		for( int x = 0; x<40; x++ ) {
			Uint16 addr = Monitor560x192::getAddressLo40(page, y, x);
			line.push_back(transliterateText(emulator->memory->getMem(addr)));
		}
		cout << line << "\n";
	}
	cout << "text_screen_end\n";
}

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

bool writeGuestCoreDump( EventLoop* emulator, const string& outPath )
{
	ofstream out(outPath.c_str(), ios::out | ios::trunc);
	if( !out.is_open() ) {
		cerr << "Unable to write guest core dump file: \"" << outPath << "\"\n";
		return false;
	}
	streambuf* oldBuf = cout.rdbuf(out.rdbuf());
	emulator->memory->dumpMem();
	cout.rdbuf(oldBuf);
	return true;
}

int main( int args, char** argv )
{

	for( int i = 1; i<args; i++ ) {
		string arg = argv[i];
		if( arg == "--help" ) {
			cout << "Usage: ever2e [--paste-file <path>] [--guest-core-dump <path>] [--print-text-at-exit]\n";
			return 0;
		}
		if( arg == "--paste-file" ) {
			if( i+1>=args ) {
				cerr << "Missing value for --paste-file\n";
				return 1;
			}
			InputFileName = argv[++i];
			continue;
		}
		if( arg.find("--paste-file=")==0 ) {
			InputFileName = argv[i] + (sizeof("--paste-file=")-1);
			continue;
		}
		if( arg == "--guest-core-dump" ) {
			if( i+1>=args ) {
				cerr << "Missing value for --guest-core-dump\n";
				return 1;
			}
			GuestCoreDumpFile = argv[++i];
			continue;
		}
		if( arg.find("--guest-core-dump=")==0 ) {
			GuestCoreDumpFile = arg.substr(sizeof("--guest-core-dump=")-1);
			continue;
		}
		if( arg == "--print-text-at-exit" ) {
			PrintTextAtExit = true;
			continue;
		}
		cerr << "Unrecognized argument: " << arg << "\n";
		return 1;
	}

	EventLoop emulator;

	if( InputFileName!=NULL ) {
		emulator.incorporate(hostCycle, true);
	}

	do
		emulator.cycle();
	while( !emulator.getExitStatus() );

	if( PrintTextAtExit )
		printTextScreen(&emulator);

	if( !GuestCoreDumpFile.empty() && !writeGuestCoreDump(&emulator, GuestCoreDumpFile) )
		return 1;
	
	return 0;

}
