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
#include <cstdint>
#include "eventloop.h"


using namespace std;

const char* InputFileName = NULL;
string GuestCoreDumpFile;
bool PrintTextAtExit = false;
bool PrintCpuStateAtExit = false;
long long CpuStepLimit = -1;
long long CpuStepCount = 0;
long long TraceStepsFrom = -1;
long long TraceStepsCount = 0;
string TraceFilePath;
bool TraceStartPcSet = false;
Uint16 TraceStartPc = 0;
bool TraceStarted = false;
ofstream TraceFileOut;
bool TraceVerbose = false;
bool HeadlessMode = false;
bool DeterministicOpenBus = false;
Cpu65c02::CpuProfile SelectedCpuProfile = Cpu65c02::PROFILE_CMD;
bool CpuProfileSetByCli = false;
vector<Uint16> HaltExecutionPcs;
vector<Uint16> RequiredHaltPcs;
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

bool parseCpuProfileArg( const string& raw, Cpu65c02::CpuProfile& out )
{
	if( raw=="cmd" || raw=="CMD" ) {
		out = Cpu65c02::PROFILE_CMD;
		return true;
	}
	if( raw=="wdc" || raw=="WDC" ) {
		out = Cpu65c02::PROFILE_WDC;
		return true;
	}
	return false;
}

char transliterateText( Uint8 ascii )
{
	ascii &= 0x7f;
	if( ascii<0x20 || ascii==0x7f )
		ascii = 0x7e;
	return (char) ascii;
}

bool isGuestPromptReadyForPaste( EventLoop* emulator )
{
	int page = emulator->memory->getSwitch(Memory128k::_PAGE2) ? 2:1;
	for( int y = 0; y<24; y++ ) {
		Uint16 addr = Monitor560x192::getAddressLo40(page, y, 0);
		Uint8 c = emulator->memory->getMem(addr) & 0x7f;
		if( c==']' || c=='*' )
			return true;
	}
	return false;
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

void printCpuState( EventLoop* emulator )
{
	cout << "cpu_state_begin\n";
	cout << "steps=" << CpuStepCount << "\n";
	cout << "halt_requested=" << (HaltedAtExecutionPc ? 1 : 0) << "\n";
	if( HaltedAtExecutionPc )
		cout << "halt_pc=" << hex << uppercase << setw(4) << setfill('0') << (int)HaltedExecutionPc << dec << "\n";
	cout << "registers=" << emulator->cpu->getRegisterString() << "\n";
	cout << "cpu_state_end\n";
}

bool hostCycle( EventLoop *emulator )
{

	static ifstream::pos_type size = -1;
	static int ptr = 0;
	static char* buffer = NULL;
	bool hasInputFile = (InputFileName!=NULL && InputFileName[0]!='\0');

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
	if( hasInputFile && size == -1 ) {
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
	else if( hasInputFile && ptr<size ) {
		if( !isGuestPromptReadyForPaste(emulator) )
			return false;
		int totalSize = (int) size;
		emulator->queuePasteText((const Uint8*)buffer + ptr, (size_t)(totalSize - ptr));
		ptr = (int) size;
	}

	if( CpuStepLimit>=0 ) {
		CpuStepCount++;
		Uint16 pc = emulator->cpu->getProgramCounter();
		if( TraceFileOut.is_open() ) {
			if( !TraceStartPcSet || TraceStarted || pc==TraceStartPc )
				TraceStarted = true;
			if( TraceStarted ) {
				Uint8 opcode = emulator->memory->getMem(pc);
				if( TraceVerbose ) {
					Uint8 opc1 = opcode;
					Uint8 opc2 = emulator->memory->getMem((Uint16)(pc + 1));
					Uint8 opc3 = emulator->memory->getMem((Uint16)(pc + 2));
					Cpu65c02::OpcodeDescriptor desc =
							Cpu65c02::getOpcodeDescriptorForProfile(emulator->cpu->getCpuProfile(), opcode);
					TraceFileOut << CpuStepCount << ','
							<< hex << uppercase << setw(4) << setfill('0') << (int)pc << ','
							<< setw(2) << (int)opcode << ','
							<< setw(2) << (int)opc1 << ','
							<< setw(2) << (int)opc2 << ','
							<< setw(2) << (int)opc3 << ','
							<< setw(2) << (int)emulator->cpu->getRegisterA() << ','
							<< setw(2) << (int)emulator->cpu->getRegisterX() << ','
							<< setw(2) << (int)emulator->cpu->getRegisterY() << ','
							<< setw(2) << (int)emulator->cpu->getRegisterP() << ','
							<< setw(2) << (int)emulator->cpu->getRegisterS() << ','
							<< '"' << Cpu65c02::getOpcodeMnemonicName(desc.mnemonic) << '"' << ','
							<< '"' << Cpu65c02::getAddressModeName(desc.addressMode) << '"' << ','
							<< '"' << emulator->cpu->getRegisterString() << '"' << dec << "\n";
				}
				else {
					TraceFileOut << CpuStepCount << ','
							<< hex << uppercase << setw(4) << setfill('0') << (int)pc << ','
							<< setw(2) << (int)opcode << dec << ','
							<< '"' << emulator->cpu->getRegisterString() << '"' << "\n";
				}
			}
		}
		if( TraceStepsFrom>=0 && TraceStepsCount>0 &&
				CpuStepCount>=TraceStepsFrom &&
				CpuStepCount<(TraceStepsFrom+TraceStepsCount) ) {
			cout << "trace_step=" << CpuStepCount
				 << " " << emulator->cpu->getRegisterString() << "\n";
		}
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
			cout << "Usage: ever2e [--paste-file <path>] [--guest-core-dump <path>] [--print-text-at-exit] [--print-cpu-state-at-exit] [--steps <count>] [--trace-steps-from <count>] [--trace-steps-count <count>] [--trace-file <path>] [--trace-verbose] [--trace-start-pc <addr>] [--halt-execution <addr[,addr...]>] [--require-halt-pc <addr[,addr...]>] [--headless] [--deterministic-open-bus] [--cpu-profile <cmd|wdc>]\n";
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
		if( arg == "--print-cpu-state-at-exit" ) {
			PrintCpuStateAtExit = true;
			continue;
		}
		if( arg == "--headless" ) {
			HeadlessMode = true;
			continue;
		}
		if( arg == "--deterministic-open-bus" ) {
			DeterministicOpenBus = true;
			continue;
		}
		if( arg == "--cpu-profile" ) {
			if( i+1>=args ) {
				cerr << "Missing value for --cpu-profile\n";
				return 1;
			}
			if( !parseCpuProfileArg(argv[++i], SelectedCpuProfile) ) {
				cerr << "Invalid value for --cpu-profile (expected cmd|wdc)\n";
				return 1;
			}
			CpuProfileSetByCli = true;
			continue;
		}
		if( arg.find("--cpu-profile=")==0 ) {
			if( !parseCpuProfileArg(arg.substr(sizeof("--cpu-profile=")-1), SelectedCpuProfile) ) {
				cerr << "Invalid value for --cpu-profile (expected cmd|wdc)\n";
				return 1;
			}
			CpuProfileSetByCli = true;
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
		if( arg == "--trace-file" ) {
			if( i+1>=args ) {
				cerr << "Missing value for --trace-file\n";
				return 1;
			}
			TraceFilePath = argv[++i];
			continue;
		}
		if( arg.find("--trace-file=")==0 ) {
			TraceFilePath = arg.substr(sizeof("--trace-file=")-1);
			continue;
		}
		if( arg == "--trace-verbose" ) {
			TraceVerbose = true;
			continue;
		}
		if( arg == "--trace-start-pc" ) {
			if( i+1>=args ) {
				cerr << "Missing value for --trace-start-pc\n";
				return 1;
			}
			if( !parseHexWordArg(argv[++i], TraceStartPc) ) {
				cerr << "Invalid value for --trace-start-pc\n";
				return 1;
			}
			TraceStartPcSet = true;
			continue;
		}
		if( arg.find("--trace-start-pc=")==0 ) {
			if( !parseHexWordArg(arg.substr(sizeof("--trace-start-pc=")-1), TraceStartPc) ) {
				cerr << "Invalid value for --trace-start-pc\n";
				return 1;
			}
			TraceStartPcSet = true;
			continue;
		}
		if( arg == "--trace-steps-from" ) {
			if( i+1>=args ) {
				cerr << "Missing value for --trace-steps-from\n";
				return 1;
			}
			TraceStepsFrom = strtoll(argv[++i], NULL, 10);
			if( TraceStepsFrom<0 ) {
				cerr << "Invalid value for --trace-steps-from\n";
				return 1;
			}
			continue;
		}
		if( arg.find("--trace-steps-from=")==0 ) {
			TraceStepsFrom = strtoll(arg.c_str() + (sizeof("--trace-steps-from=")-1), NULL, 10);
			if( TraceStepsFrom<0 ) {
				cerr << "Invalid value for --trace-steps-from\n";
				return 1;
			}
			continue;
		}
		if( arg == "--trace-steps-count" ) {
			if( i+1>=args ) {
				cerr << "Missing value for --trace-steps-count\n";
				return 1;
			}
			TraceStepsCount = strtoll(argv[++i], NULL, 10);
			if( TraceStepsCount<0 ) {
				cerr << "Invalid value for --trace-steps-count\n";
				return 1;
			}
			continue;
		}
		if( arg.find("--trace-steps-count=")==0 ) {
			TraceStepsCount = strtoll(arg.c_str() + (sizeof("--trace-steps-count=")-1), NULL, 10);
			if( TraceStepsCount<0 ) {
				cerr << "Invalid value for --trace-steps-count\n";
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
		if( arg == "--require-halt-pc" ) {
			if( i+1>=args ) {
				cerr << "Missing value for --require-halt-pc\n";
				return 1;
			}
			if( !appendWordListArg(argv[++i], RequiredHaltPcs) ) {
				cerr << "Invalid value for --require-halt-pc\n";
				return 1;
			}
			continue;
		}
		if( arg.find("--require-halt-pc=")==0 ) {
			if( !appendWordListArg(arg.substr(sizeof("--require-halt-pc=")-1), RequiredHaltPcs) ) {
				cerr << "Invalid value for --require-halt-pc\n";
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

	const char* envCpuProfile = getenv("EVER2E_CPU_PROFILE");
	if( !CpuProfileSetByCli && envCpuProfile!=NULL && envCpuProfile[0]!='\0' ) {
		Cpu65c02::CpuProfile envProfile;
		if( !parseCpuProfileArg(envCpuProfile, envProfile) ) {
			cerr << "Invalid EVER2E_CPU_PROFILE value (expected cmd|wdc)\n";
			return 1;
		}
		SelectedCpuProfile = envProfile;
	}

	EventLoop emulator(SelectedCpuProfile);
	emulator.memory->setDeterministicOpenBusHigh(DeterministicOpenBus);
	if( !TraceFilePath.empty() ) {
		TraceFileOut.open(TraceFilePath.c_str(), ios::out | ios::trunc);
		if( !TraceFileOut.is_open() ) {
			cerr << "Unable to open trace file: \"" << TraceFilePath << "\"\n";
			return 1;
		}
		if( TraceVerbose )
			TraceFileOut << "step,pc,opcode,opc1,opc2,opc3,a,x,y,p,s,mnemonic,mode,registers\n";
		else
			TraceFileOut << "step,pc,opcode,registers\n";
	}
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
	if( PrintCpuStateAtExit )
		printCpuState(&emulator);
	if( !HaltExecutionPcs.empty() && HaltedAtExecutionPc ) {
		cout << "Stopped at PC=" << hex << uppercase << setw(4) << setfill('0') << (int)HaltedExecutionPc << dec << "\n";
	}
	if( !RequiredHaltPcs.empty() ) {
		Uint16 finalPc = HaltedAtExecutionPc ? HaltedExecutionPc : emulator.cpu->getProgramCounter();
		bool matched = false;
		for( size_t i = 0; i<RequiredHaltPcs.size(); i++ ) {
			if( finalPc==RequiredHaltPcs[i] ) {
				matched = true;
				break;
			}
		}
		if( !matched ) {
			cerr << "Error: final PC did not match required value(s). PC="
				 << hex << uppercase << setw(4) << setfill('0') << (int)finalPc << dec << "\n";
			return 2;
		}
	}

	if( !GuestCoreDumpFile.empty() && !writeGuestCoreDump(&emulator, GuestCoreDumpFile) )
		return 1;
	if( TraceFileOut.is_open() )
		TraceFileOut.close();
	
	return 0;

}
