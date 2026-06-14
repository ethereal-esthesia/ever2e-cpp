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
#include "drive5_25.h"

#include <algorithm>
#include <cerrno>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>


using namespace std;


namespace {

const Uint8 DISKII_P6_ROM[256] = {
	0xA2,0x20,0xA0,0x00,0xA2,0x03,0x86,0x3C,0x8A,0x0A,0x24,0x3C,0xF0,0x10,0x05,0x3C,
	0x49,0xFF,0x29,0x7E,0xB0,0x08,0x4A,0xD0,0xFB,0x98,0x9D,0x56,0x03,0xC8,0xE8,0x10,
	0xE5,0x20,0x58,0xFF,0xBA,0xBD,0x00,0x01,0x0A,0x0A,0x0A,0x0A,0x85,0x2B,0xAA,0xBD,
	0x8E,0xC0,0xBD,0x8C,0xC0,0xBD,0x8A,0xC0,0xBD,0x89,0xC0,0xA0,0x50,0xBD,0x80,0xC0,
	0x98,0x29,0x03,0x0A,0x05,0x2B,0xAA,0xBD,0x81,0xC0,0xA9,0x56,0x20,0xA8,0xFC,0x88,
	0x10,0xEB,0x85,0x26,0x85,0x3D,0x85,0x41,0xA9,0x08,0x85,0x27,0x18,0x08,0xBD,0x8C,
	0xC0,0x10,0xFB,0x49,0xD5,0xD0,0xF7,0xBD,0x8C,0xC0,0x10,0xFB,0xC9,0xAA,0xD0,0xF3,
	0xEA,0xBD,0x8C,0xC0,0x10,0xFB,0xC9,0x96,0xF0,0x09,0x28,0x90,0xDF,0x49,0xAD,0xF0,
	0x25,0xD0,0xD9,0xA0,0x03,0x85,0x40,0xBD,0x8C,0xC0,0x10,0xFB,0x2A,0x85,0x3C,0xBD,
	0x8C,0xC0,0x10,0xFB,0x25,0x3C,0x88,0xD0,0xEC,0x28,0xC5,0x3D,0xD0,0xBE,0xA5,0x40,
	0xC5,0x41,0xD0,0xB8,0xB0,0xB7,0xA0,0x56,0x84,0x3C,0xBC,0x8C,0xC0,0x10,0xFB,0x59,
	0xD6,0x02,0xA4,0x3C,0x88,0x99,0x00,0x03,0xD0,0xEE,0x84,0x3C,0xBC,0x8C,0xC0,0x10,
	0xFB,0x59,0xD6,0x02,0xD0,0x87,0xA0,0x00,0xA2,0x56,0xCA,0x30,0xFB,0xB1,0x26,0x5E,
	0x00,0x03,0x2A,0x5E,0x00,0x03,0x2A,0x91,0x26,0xC8,0xD0,0xEE,0xE6,0x27,0xE6,0x3D,
	0xA5,0x3D,0xCD,0x00,0x08,0xA6,0x2B,0x90,0xDB,0x4C,0x01,0x08,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

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


Floppy525Controller::Floppy525Controller( int slotNumber, const string& drive1Path, const string& drive2Path )
	: slot(slotNumber),
	  dataRegister(0),
	  writeRequestRegister(0),
	  writeRegister(-1),
	  cycleDelay(0),
	  cyclePeriod(32),
	  driveWrite(false),
	  driveOn(false),
	  debugLog(false),
	  driveSelect(0),
	  driveOffRequest(-1),
	  writeOn(false)
{
	fileName[0] = drive1Path;
	fileName[1] = drive2Path;
	readOnly.fill(false);
	phase.fill(0);
	headHalfTrack.fill(69);
	headSectorByte.fill(0);
	loadRom();
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

void Floppy525Controller::loadRom()
{
	for( int i = 0; i<256; i++ )
		PeripheralCard16bit::putMem16b((Uint8)i, DISKII_P6_ROM[i]);
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
		driveSelect = nextDrive;
		startDrive();
	}
	else {
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
	cout << "Slot " << slot << ", drive " << getDrive() << " stopped\n";
	if( driveWrite )
		saveImage(driveSelect);
	driveWrite = false;
}

Uint8 Floppy525Controller::accessSwitch( Uint8 offset, bool isWrite, Uint8 value )
{
	bool hasMsb = false;
	bool msb = false;

	if( isWrite && (offset==0x0d || offset==0x0f) && driveOn )
		writeRequestRegister = value;
	if( isWrite )
		driveWrite = true;

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
			if( writeOn )
				writeRegister = writeRequestRegister;
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

	if( writeRegister>=0 && !diskImage[driveSelect].empty() ) {
		const int track = headHalfTrack[driveSelect] >> 1;
		const size_t index = (size_t) track*TRACK_BYTES + (size_t) headSectorByte[driveSelect];
		diskImage[driveSelect][index] = (Uint8) writeRegister;
		writeRegister = -1;
	}

	if( driveOffRequest>=0 && driveOffRequest++==(0x40000>>3) ) {
		driveOffRequest = -1;
		killDrive();
	}

	cycleDelay = cyclePeriod-1;
}
