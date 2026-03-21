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
#include <cctype>
#include <map>
#include <algorithm>
#include <filesystem>
#include "CLI.hpp"
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
bool DebugMode = false;
vector<Uint16> TraceWriteAddresses;
Cpu65c02::CpuProfile SelectedCpuProfile = Cpu65c02::PROFILE_CMD;
bool CpuProfileSetByCli = false;
vector<Uint16> HaltExecutionPcs;
vector<Uint16> RequiredHaltPcs;
bool HaltedAtExecutionPc = false;
Uint16 HaltedExecutionPc = 0;
string EmuConfigPath;
string RomFileOverridePath;

struct EmuConfig
{
	map<string, string> properties;
	string sourcePath;
};

string trimString( const string& in )
{
	size_t start = 0;
	while( start<in.size() && (in[start]==' ' || in[start]=='\t' || in[start]=='\r' || in[start]=='\n') )
		start++;
	size_t end = in.size();
	while( end>start && (in[end-1]==' ' || in[end-1]=='\t' || in[end-1]=='\r' || in[end-1]=='\n') )
		end--;
	return in.substr(start, end-start);
}

bool hasCaseInsensitiveSuffix( const string& value, const string& suffix )
{
	if( value.size()<suffix.size() )
		return false;
	for( size_t i = 0; i<suffix.size(); i++ ) {
		char a = (char) tolower((unsigned char) value[value.size()-suffix.size()+i]);
		char b = (char) tolower((unsigned char) suffix[i]);
		if( a!=b )
			return false;
	}
	return true;
}

bool loadEmuConfig( const string& rawPath, EmuConfig& out )
{
	string path = rawPath;
	if( !hasCaseInsensitiveSuffix(path, ".emu") )
		path += ".emu";
	ifstream in(path.c_str(), ios::in);
	if( !in.is_open() )
		return false;
	string line;
	while( getline(in, line) ) {
		string t = trimString(line);
		if( t.empty() || t[0]=='#' || t[0]==';' )
			continue;
		size_t eq = t.find('=');
		size_t colon = t.find(':');
		size_t split = string::npos;
		if( eq!=string::npos && colon!=string::npos )
			split = min(eq, colon);
		else if( eq!=string::npos )
			split = eq;
		else
			split = colon;
		if( split==string::npos )
			continue;
		string key = trimString(t.substr(0, split));
		string value = trimString(t.substr(split+1));
		if( !key.empty() )
			out.properties[key] = value;
	}
	out.sourcePath = path;
	return true;
}

bool tryParseSlotPattern( const string& raw, Uint8& out )
{
	if( raw.empty() )
		return false;
	char* end = NULL;
	unsigned long v = strtoul(raw.c_str(), &end, 0);
	if( end==raw.c_str() || *end!='\0' || v>0xff )
		return false;
	out = (Uint8) v;
	return true;
}

string dirnameOfPath( const string& path )
{
	size_t slash = path.find_last_of("/\\");
	if( slash==string::npos )
		return string(".");
	return path.substr(0, slash);
}

bool isAbsolutePath( const string& path )
{
	if( path.empty() )
		return false;
	if( path[0]=='/' || path[0]=='\\' )
		return true;
	if( path.size()>2 && ((path[0]>='A' && path[0]<='Z') || (path[0]>='a' && path[0]<='z')) && path[1]==':' )
		return true;
	return false;
}

string resolveEmuRelativePath( const EmuConfig& cfg, const string& fileName )
{
	if( fileName.empty() )
		return fileName;
	if( isAbsolutePath(fileName) )
		return fileName;
	std::filesystem::path base = dirnameOfPath(cfg.sourcePath);
	if( !base.is_absolute() )
		base = std::filesystem::absolute(base);
	return (base / fileName).lexically_normal().string();
}

void installSlotsFromEmu( EventLoop* emulator, const EmuConfig& cfg, vector<PeripheralCard16bit*>& ownedCards )
{
	for( int slot = 1; slot<=7; slot++ ) {
		string key = string("machine.layout.slot.") + char('0' + slot);
		map<string, string>::const_iterator it = cfg.properties.find(key);
		if( it==cfg.properties.end() || trimString(it->second).empty() ) {
			emulator->memory->putSlot(slot, NULL);
			continue;
		}
		string className = trimString(it->second);
		if( className=="test.PatternSlotCard" ) {
			Uint8 pattern = (Uint8) (slot & 0xff);
			string patternKey = key + ".pattern";
			map<string, string>::const_iterator pit = cfg.properties.find(patternKey);
			if( pit!=cfg.properties.end() ) {
				Uint8 parsedPattern;
				if( tryParseSlotPattern(trimString(pit->second), parsedPattern) )
					pattern = parsedPattern;
				else
					cerr << "Warning: invalid " << patternKey << " value in " << cfg.sourcePath
						 << "; using slot default pattern\n";
			}
			PeripheralCard16bit* card = new PeripheralCard16bit();
			card->setSlotPattern(pattern);
			card->setIdTag(className);
			ownedCards.push_back(card);
			emulator->memory->putSlot(slot, card);
			cout << "Slot " << slot << ": " << className << " pattern=0x"
				 << hex << uppercase << setw(2) << setfill('0') << (int)pattern << dec << "\n";
		}
		else {
			// JVM config compatibility: accept keys but keep slot empty when unsupported.
			cerr << "Warning: unsupported slot class \"" << className
				 << "\" for slot " << slot << " in " << cfg.sourcePath << "; leaving empty\n";
			emulator->memory->putSlot(slot, NULL);
		}
	}
}

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
		Uint8 c = emulator->memory->peekMemNoSideEffects(addr) & 0x7f;
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
			line.push_back(transliterateText(emulator->memory->peekMemNoSideEffects(addr)));
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
				Uint8 opcode = emulator->memory->peekMemNoSideEffects(pc);
				if( TraceVerbose ) {
					Uint8 opc1 = opcode;
					Uint8 opc2 = emulator->memory->peekMemNoSideEffects((Uint16)(pc + 1));
					Uint8 opc3 = emulator->memory->peekMemNoSideEffects((Uint16)(pc + 2));
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
	string pasteFileArg;
	string cpuProfileArg;
	string traceStartPcArg;
	vector<string> traceWriteArgTokens;
	vector<string> haltExecutionArgTokens;
	vector<string> requireHaltArgTokens;

	CLI::App app("Ever2e");
	app.set_help_flag("--help", "Show help");
	app.add_option("emu-path", EmuConfigPath, "Path to required .emu config")->required();
	app.add_option("--rom-file", RomFileOverridePath, "Override ROM path (replaces binary.file from .emu)");
	app.add_option("--paste-file", pasteFileArg, "Paste script file path");
	app.add_option("--guest-core-dump", GuestCoreDumpFile, "Write guest core dump to file");
	app.add_flag("--print-text-at-exit", PrintTextAtExit, "Print text screen at exit");
	app.add_flag("--print-cpu-state-at-exit", PrintCpuStateAtExit, "Print CPU state at exit");
	app.add_option("--steps", CpuStepLimit, "Step limit")->check(CLI::NonNegativeNumber);
	app.add_option("--trace-steps-from", TraceStepsFrom, "Console trace start step")->check(CLI::NonNegativeNumber);
	app.add_option("--trace-steps-count", TraceStepsCount, "Console trace step count")->check(CLI::NonNegativeNumber);
	app.add_option("--trace-file", TraceFilePath, "CSV trace output path");
	app.add_flag("--trace-verbose", TraceVerbose, "Verbose CSV trace format");
	app.add_option("--trace-start-pc", traceStartPcArg, "Hex start PC (e.g. 0x2000)");
	app.add_option("--trace-write", traceWriteArgTokens, "Trace writes to hex address list")->delimiter(',');
	app.add_option("--halt-execution", haltExecutionArgTokens, "Stop when PC reaches address list")->delimiter(',');
	app.add_option("--require-halt-pc", requireHaltArgTokens, "Require final PC to be in address list")->delimiter(',');
	app.add_flag("--headless", HeadlessMode, "Run without video/audio output");
	app.add_flag("--deterministic-open-bus", DeterministicOpenBus, "Force deterministic open-bus reads");
	app.add_flag("--debug", DebugMode, "Enable debug instrumentation");
	app.add_option("--cpu-profile", cpuProfileArg, "CPU profile: cmd|wdc");

	try {
		app.parse(args, argv);
	}
	catch( const CLI::ParseError& e ) {
		return app.exit(e);
	}

	if( !pasteFileArg.empty() )
		InputFileName = pasteFileArg.c_str();

	if( !cpuProfileArg.empty() ) {
		if( !parseCpuProfileArg(cpuProfileArg, SelectedCpuProfile) ) {
			cerr << "Invalid value for --cpu-profile (expected cmd|wdc)\n";
			return 1;
		}
		CpuProfileSetByCli = true;
	}

	if( !traceStartPcArg.empty() ) {
		if( !parseHexWordArg(traceStartPcArg, TraceStartPc) ) {
			cerr << "Invalid value for --trace-start-pc\n";
			return 1;
		}
		TraceStartPcSet = true;
	}

	for( size_t i = 0; i<traceWriteArgTokens.size(); i++ ) {
		Uint16 value;
		if( !parseHexWordArg(traceWriteArgTokens[i], value) ) {
			cerr << "Invalid value for --trace-write\n";
			return 1;
		}
		TraceWriteAddresses.push_back(value);
	}
	for( size_t i = 0; i<haltExecutionArgTokens.size(); i++ ) {
		Uint16 value;
		if( !parseHexWordArg(haltExecutionArgTokens[i], value) ) {
			cerr << "Invalid value for --halt-execution\n";
			return 1;
		}
		HaltExecutionPcs.push_back(value);
	}
	for( size_t i = 0; i<requireHaltArgTokens.size(); i++ ) {
		Uint16 value;
		if( !parseHexWordArg(requireHaltArgTokens[i], value) ) {
			cerr << "Invalid value for --require-halt-pc\n";
			return 1;
		}
		RequiredHaltPcs.push_back(value);
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
	if( DebugMode )
		setenv("EVER2E_DEBUG", "1", 1);

	vector<PeripheralCard16bit*> ownedSlotCards;
	EmuConfig emuConfig;
	if( !loadEmuConfig(EmuConfigPath, emuConfig) ) {
		cerr << "Unable to load .emu config: \"" << EmuConfigPath << "\"\n";
		return 1;
	}
	string romPath;
	map<string, string>::const_iterator binIt = emuConfig.properties.find("binary.file");
	if( binIt!=emuConfig.properties.end() && !trimString(binIt->second).empty() )
		romPath = resolveEmuRelativePath(emuConfig, trimString(binIt->second));
	if( !RomFileOverridePath.empty() )
		romPath = RomFileOverridePath;
	EventLoop emulator(SelectedCpuProfile, romPath);
	installSlotsFromEmu(&emulator, emuConfig, ownedSlotCards);
	emulator.memory->setDeterministicOpenBusHigh(DeterministicOpenBus);
	emulator.setDebugMenuToggle(DebugMode);
	emulator.memory->clearTraceWriteAddresses();
	for( size_t i = 0; i<TraceWriteAddresses.size(); i++ )
		emulator.memory->addTraceWriteAddress(TraceWriteAddresses[i]);
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
	for( size_t i = 0; i<ownedSlotCards.size(); i++ )
		delete ownedSlotCards[i];
	
	return 0;

}
