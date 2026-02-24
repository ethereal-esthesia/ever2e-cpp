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
#include <cstdlib>
#include <vector>
#include <sstream>
#include <iomanip>
#include "eventloop.h"


using namespace std;

const char* InputFileName = NULL;
string GuestCoreDumpFile;
bool PrintTextAtExit = false;
long long CpuStepLimit = -1;
long long CpuStepCount = 0;
bool HeadlessMode = false;
vector<Uint16> HaltExecutionPcs;
bool HaltedAtExecutionPc = false;
Uint16 HaltedExecutionPc = 0;

bool parseHexWordArg( const string& raw, Uint16& out )
{
	if( raw.empty() )
		return false;
	char* end = NULL;
	unsigned long v = strtoul(raw.c_str(), &end, 0);
	if( end==raw.c_str() || *end!='\0' || v>0xffff )
		return false;
	out = (Uint16) v;
	return true;
}

bool appendWordListArg( const string& rawList, vector<Uint16>& out )
{
	string token;
	stringstream ss(rawList);
	while( getline(ss, token, ',') ) {
		if( token.empty() )
			return false;
		Uint16 value;
		if( !parseHexWordArg(token, value) )
			return false;
		out.push_back(value);
	}
	return !out.empty();
}

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

	if( !HaltExecutionPcs.empty() ) {
		Uint16 pc = emulator->cpu->getProgramCounter();
		for( size_t i = 0; i<HaltExecutionPcs.size(); i++ ) {
			if( pc==HaltExecutionPcs[i] ) {
				HaltedAtExecutionPc = true;
				HaltedExecutionPc = pc;
				emulator->requestExit();
				return true;
			}
		}
	}

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
	else if( ptr<size ) {
		// Match JVM behavior: queue the entire paste payload immediately, with LF normalized to CR.
		while( ptr<size ) {
			unsigned char c = (unsigned char)buffer[ptr++];
			if( c==0x0a )
				c = 0x0d;
			emulator->queuePasteKey(c);
		}
	}

	if( CpuStepLimit>=0 ) {
		CpuStepCount++;
		if( CpuStepCount>=CpuStepLimit )
			emulator->requestExit();
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
			cout << "Usage: ever2e [--paste-file <path>] [--guest-core-dump <path>] [--print-text-at-exit] [--steps <count>] [--halt-execution <addr[,addr...]>] [--headless]\n";
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
		if( arg == "--headless" ) {
			HeadlessMode = true;
			continue;
		}
		if( arg == "--steps" ) {
			if( i+1>=args ) {
				cerr << "Missing value for --steps\n";
				return 1;
			}
			CpuStepLimit = strtoll(argv[++i], NULL, 10);
			if( CpuStepLimit<0 ) {
				cerr << "Invalid value for --steps\n";
				return 1;
			}
			continue;
		}
		if( arg.find("--steps=")==0 ) {
			CpuStepLimit = strtoll(arg.c_str() + (sizeof("--steps=")-1), NULL, 10);
			if( CpuStepLimit<0 ) {
				cerr << "Invalid value for --steps\n";
				return 1;
			}
			continue;
		}
		if( arg == "--halt-execution" ) {
			if( i+1>=args ) {
				cerr << "Missing value for --halt-execution\n";
				return 1;
			}
			if( !appendWordListArg(argv[++i], HaltExecutionPcs) ) {
				cerr << "Invalid value for --halt-execution\n";
				return 1;
			}
			continue;
		}
		if( arg.find("--halt-execution=")==0 ) {
			if( !appendWordListArg(arg.substr(sizeof("--halt-execution=")-1), HaltExecutionPcs) ) {
				cerr << "Invalid value for --halt-execution\n";
				return 1;
			}
			continue;
		}
		cerr << "Unrecognized argument: " << arg << "\n";
		return 1;
	}

	if( HeadlessMode ) {
		setenv("SDL_VIDEODRIVER", "dummy", 1);
		setenv("SDL_AUDIODRIVER", "dummy", 1);
	}

	EventLoop emulator;
	if( HeadlessMode )
		emulator.setUnthrottled(true);

	if( InputFileName!=NULL || CpuStepLimit>=0 || !HaltExecutionPcs.empty() ) {
		// Automated runs should start from the emulation screen, not the host help menu.
		emulator.dismissHostMenu();
		emulator.incorporate(hostCycle, true);
	}

	do
		emulator.cycle();
	while( !emulator.getExitStatus() );

	if( PrintTextAtExit )
		printTextScreen(&emulator);
	if( !HaltExecutionPcs.empty() && HaltedAtExecutionPc ) {
		cout << "Stopped at PC=" << hex << uppercase << setw(4) << setfill('0') << (int)HaltedExecutionPc << dec << "\n";
	}

	if( !GuestCoreDumpFile.empty() && !writeGuestCoreDump(&emulator, GuestCoreDumpFile) )
		return 1;
	
	return 0;

}
