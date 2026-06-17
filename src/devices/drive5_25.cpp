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
#include "devices/drive5_25.h"

#include <algorithm>
#include <cerrno>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>


using namespace std;


namespace {

const int PHASE_SHIFT[16] = {
	-1, 0, 1, -1,
	2, -1, -1, -1,
	3, -1, -1, -1,
	-1, -1, -1, -1
};

string upperExtension( const string& path )
{
	size_t dot = path.find_last_of('.');
	if( dot==string::npos )
		return string();
	string ext = path.substr(dot+1);
	transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c){ return (char) toupper(c); });
	return ext;
}

} // namespace


Floppy525Controller::Floppy525Controller( int slotNumber, const string& drive1Path, const string& drive2Path,
		const string& romPath )
	: slot(slotNumber),
	  dataRegister(0),
	  writeRequestRegister(0),
	  writeRegister(-1),
	  writeDrive(-1),
	  writeTrack(-1),
	  writeByte(-1),
	  cycleDelay(0),
	  cyclePeriod(31),
	  driveOn(false),
	  debugLog(false),
	  driveSelect(0),
	  driveOffRequest(-1),
	  writeOn(false)
{
	fileName[0] = drive1Path;
	fileName[1] = drive2Path;
	readOnly.fill(false);
	dirty.fill(false);
	phase.fill(0);
	headHalfTrack.fill(69);
	headSectorByte.fill(0);
	loadRom(romPath);
	setIdTag("drive.floppy525.Floppy525Controller");
	const char* periodEnv = getenv("EVER2E_DISKII_CYCLE_PERIOD");
	if( periodEnv!=NULL && periodEnv[0]!='\0' ) {
		char* end = NULL;
		long parsed = strtol(periodEnv, &end, 0);
		if( end!=periodEnv && *end=='\0' && parsed>0 && parsed<100000 )
			cyclePeriod = (int) parsed;
		else
			cerr << "Ignoring invalid EVER2E_DISKII_CYCLE_PERIOD value: " << periodEnv << "\n";
	}
	const char* debugEnv = getenv("EVER2E_DISKII_DEBUG");
	debugLog = (debugEnv!=NULL && debugEnv[0]!='\0' && debugEnv[0]!='0');
}

Floppy525Controller::~Floppy525Controller()
{
	if( driveOn )
		killDrive();
}

void Floppy525Controller::loadRom( const string& romPath )
{
	if( romPath.empty() )
		throw runtime_error("Disk II slot ROM is required");

	ifstream in(romPath.c_str(), ios::in | ios::binary);
	if( !in.is_open() )
		throw runtime_error("Unable to open Disk II slot ROM: " + romPath);

	vector<Uint8> rom(256, 0);
	in.read(reinterpret_cast<char*>(rom.data()), (streamsize)rom.size());
	const streamsize readBytes = in.gcount();
	in.peek();
	if( readBytes!=256 || !in.eof() )
		throw runtime_error("Disk II slot ROM must be exactly 256 bytes: " + romPath);

	for( int i = 0; i<256; i++ )
		PeripheralCard16bit::putMem16b((Uint8)i, rom[(size_t)i]);
}

void Floppy525Controller::loadImage( int drive )
{
	if( drive<0 || drive>=DRIVE_COUNT || fileName[drive].empty() )
		return;

	const string ext = upperExtension(fileName[drive]);
	if( ext!="NIB" ) {
		if( ext=="PO" || ext=="DSK" || ext=="DO" )
			cerr << fileName[drive] << ": " << ext << " images are not yet supported; convert to NIB format.\n";
		else
			cerr << fileName[drive] << ": disk image format is not supported.\n";
		return;
	}

	ifstream in(fileName[drive].c_str(), ios::in | ios::binary);
	if( !in.is_open() ) {
		cerr << "Unable to open disk image for slot " << slot << ", drive " << (drive+1)
			 << ": " << fileName[drive] << "\n";
		return;
	}

	vector<Uint8> image(TRACK_TOTAL*TRACK_BYTES, 0);
	in.read(reinterpret_cast<char*>(image.data()), (streamsize) image.size());
	const streamsize got = in.gcount();
	if( got<(streamsize) image.size() ) {
		cerr << "Warning: short NIB image for slot " << slot << ", drive " << (drive+1)
			 << ": expected " << image.size() << " bytes, read " << got << "\n";
	}
	diskImage[drive].swap(image);

	error_code ec;
	filesystem::perms perms = filesystem::status(fileName[drive], ec).permissions();
	readOnly[drive] = ec || ((perms & filesystem::perms::owner_write)==filesystem::perms::none);
}

void Floppy525Controller::saveImage( int drive )
{
	if( drive<0 || drive>=DRIVE_COUNT || fileName[drive].empty() || diskImage[drive].empty() )
		return;
	if( readOnly[drive] )
		return;
	if( upperExtension(fileName[drive])!="NIB" )
		return;

	ofstream out(fileName[drive].c_str(), ios::out | ios::binary | ios::trunc);
	if( !out.is_open() ) {
		cerr << "Unable to save disk image for slot " << slot << ", drive " << (drive+1)
			 << ": " << fileName[drive] << "\n";
		return;
	}
	out.write(reinterpret_cast<const char*>(diskImage[drive].data()), (streamsize) diskImage[drive].size());
}

void Floppy525Controller::ensureImageLoaded( int drive )
{
	if( drive>=0 && drive<DRIVE_COUNT && diskImage[drive].empty() )
		loadImage(drive);
}

void Floppy525Controller::moveHead()
{
	const int phaseShift = PHASE_SHIFT[phase[driveSelect] & 0x0f];
	const int currentPhase = headHalfTrack[driveSelect] & 0x03;
	if( phaseShift<0 )
		return;

	const int oldHalfTrack = headHalfTrack[driveSelect];
	const int delta = phaseShift-currentPhase;
	if( abs(delta)==1 )
		headHalfTrack[driveSelect] += delta;
	else if( abs(delta)==3 )
		headHalfTrack[driveSelect] -= (delta>0) ? 1 : -1;

	if( headHalfTrack[driveSelect]<0 )
		headHalfTrack[driveSelect] = 0;
	else if( headHalfTrack[driveSelect]>=(TRACK_TOTAL<<1) )
		headHalfTrack[driveSelect] = (TRACK_TOTAL<<1)-1;
	if( debugLog && oldHalfTrack!=headHalfTrack[driveSelect] ) {
		cout << "Slot " << slot << ", drive " << getDrive() << ", track "
			 << (headHalfTrack[driveSelect]/2.0) << " selected\n";
	}
}

void Floppy525Controller::setDrive( int drive )
{
	const int nextDrive = drive-1;
	if( nextDrive<0 || nextDrive>=DRIVE_COUNT || nextDrive==driveSelect )
		return;
	if( driveOn ) {
		killDrive();
		driveOffRequest = -1;
		driveSelect = nextDrive;
		startDrive();
	}
	else {
		driveOffRequest = -1;
		driveSelect = nextDrive;
	}
}

int Floppy525Controller::getDrive() const
{
	return driveSelect+1;
}

void Floppy525Controller::startDrive()
{
	if( driveOn )
		return;
	driveOn = true;
	cycleDelay = 0;
	cout << "Slot " << slot << ", drive " << getDrive() << " started\n";
	ensureImageLoaded(driveSelect);
}

void Floppy525Controller::killDrive()
{
	if( !driveOn )
		return;
	driveOn = false;
	driveOffRequest = -1;
	cout << "Slot " << slot << ", drive " << getDrive() << " stopped\n";
	for( int drive = 0; drive<DRIVE_COUNT; drive++ ) {
		if( dirty[drive] ) {
			saveImage(drive);
			dirty[drive] = false;
		}
	}
}

Uint8 Floppy525Controller::accessSwitch( Uint8 offset, bool isWrite, Uint8 value )
{
	bool hasMsb = false;
	bool msb = false;

	if( isWrite && (offset==0x0d || offset==0x0f) && driveOn )
		writeRequestRegister = value;

	switch( offset & 0x0f ) {
		case 0x00:
			if( driveOn ) {
				phase[driveSelect] &= ~PHASE0_MASK;
				moveHead();
			}
			break;
		case 0x01:
			if( driveOn ) {
				phase[driveSelect] |= PHASE0_MASK;
				moveHead();
			}
			break;
		case 0x02:
			if( driveOn ) {
				phase[driveSelect] &= ~PHASE1_MASK;
				moveHead();
			}
			break;
		case 0x03:
			if( driveOn ) {
				phase[driveSelect] |= PHASE1_MASK;
				moveHead();
			}
			break;
		case 0x04:
			if( driveOn ) {
				phase[driveSelect] &= ~PHASE2_MASK;
				moveHead();
			}
			break;
		case 0x05:
			if( driveOn ) {
				phase[driveSelect] |= PHASE2_MASK;
				moveHead();
			}
			break;
		case 0x06:
			if( driveOn ) {
				phase[driveSelect] &= ~PHASE3_MASK;
				moveHead();
			}
			break;
		case 0x07:
			if( driveOn ) {
				phase[driveSelect] |= PHASE3_MASK;
				moveHead();
			}
			break;
		case 0x08:
			if( driveOn && driveOffRequest<0 )
				driveOffRequest = 0;
			break;
		case 0x09:
			startDrive();
			driveOffRequest = -1;
			break;
		case 0x0a:
			setDrive(1);
			break;
		case 0x0b:
			setDrive(2);
			break;
		case 0x0c:
			if( writeOn && driveOn && !diskImage[driveSelect].empty() ) {
				writeRegister = writeRequestRegister;
				writeDrive = driveSelect;
				writeTrack = headHalfTrack[driveSelect] >> 1;
				writeByte = headSectorByte[driveSelect] + 1;
				if( writeByte>=TRACK_BYTES )
					writeByte = 0;
			}
			break;
		case 0x0d:
			break;
		case 0x0e:
			writeOn = false;
			if( driveOn ) {
				hasMsb = true;
				msb = readOnly[driveSelect];
			}
			break;
		case 0x0f:
			writeOn = true;
			break;
	}

	Uint8 data = hasMsb ? (Uint8)((dataRegister & 0x7f) | (msb ? 0x80 : 0x00)) : (Uint8)dataRegister;
	if( dataRegister & 0x80 )
		dataRegister = 0x00;
	return data;
}

void Floppy525Controller::putMem16b( Uint8 offset, Uint8 byte )
{
	(void) accessSwitch(offset, true, byte);
}

Uint8 Floppy525Controller::getMem16b( Uint8 offset )
{
	return accessSwitch(offset, false, 0);
}

void Floppy525Controller::cycle()
{
	if( cycleDelay>0 ) {
		cycleDelay--;
		return;
	}

	if( driveOn && !diskImage[driveSelect].empty() ) {
		headSectorByte[driveSelect]++;
		if( headSectorByte[driveSelect]>=TRACK_BYTES )
			headSectorByte[driveSelect] = 0;
		if( !writeOn ) {
			const int track = headHalfTrack[driveSelect] >> 1;
			const size_t index = (size_t) track*TRACK_BYTES + (size_t) headSectorByte[driveSelect];
			dataRegister = diskImage[driveSelect][index];
		}
	}

	if( writeRegister>=0 ) {
		if( writeDrive>=0 && writeDrive<DRIVE_COUNT && !diskImage[writeDrive].empty() ) {
			const size_t index = (size_t) writeTrack*TRACK_BYTES + (size_t) writeByte;
			if( index<diskImage[writeDrive].size() ) {
				diskImage[writeDrive][index] = (Uint8) writeRegister;
				dirty[writeDrive] = true;
			}
		}
		writeRegister = -1;
		writeDrive = -1;
		writeTrack = -1;
		writeByte = -1;
	}

	if( driveOffRequest>=0 && driveOffRequest++==(0x40000>>3) ) {
		driveOffRequest = -1;
		killDrive();
	}

	cycleDelay = cyclePeriod-1;
}
