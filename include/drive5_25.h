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


#ifndef _DRIVE5_25_H_
#define _DRIVE5_25_H_


#include <array>
#include <string>
#include <vector>
#include "card16bit.h"


class Floppy525Controller : public PeripheralCard16bit
{
	static const int DRIVE_COUNT = 2;
	static const int TRACK_TOTAL = 35;
	static const int SECTOR_TOTAL = 16;
	static const int SECTOR_BYTES = 416;
	static const int TRACK_BYTES = SECTOR_BYTES*SECTOR_TOTAL;
	static const int PHASE0_MASK = 0x01;
	static const int PHASE1_MASK = 0x02;
	static const int PHASE2_MASK = 0x04;
	static const int PHASE3_MASK = 0x08;

	int slot;
	std::array<std::string, DRIVE_COUNT> fileName;
	std::array<bool, DRIVE_COUNT> readOnly;
	std::array<int, DRIVE_COUNT> phase;
	std::array<int, DRIVE_COUNT> headHalfTrack;
	std::array<int, DRIVE_COUNT> headSectorByte;
	std::array<std::vector<Uint8>, DRIVE_COUNT> diskImage;

	int dataRegister;
	int writeRequestRegister;
	int writeRegister;
	int cycleDelay;
	int cyclePeriod;
	bool driveWrite;
	bool driveOn;
	bool debugLog;
	int driveSelect;
	int driveOffRequest;
	bool writeOn;

	void loadRom();
	void loadImage( int drive );
	void saveImage( int drive );
	void ensureImageLoaded( int drive );
	void moveHead();
	void setDrive( int drive );
	int getDrive() const;
	void startDrive();
	void killDrive();
	Uint8 accessSwitch( Uint8 offset, bool isWrite, Uint8 value );

public:
	Floppy525Controller( int slot, const std::string& drive1Path, const std::string& drive2Path );
	~Floppy525Controller() override;

	void putMem16b( Uint8 offset, Uint8 byte ) override;
	Uint8 getMem16b( Uint8 offset ) override;
	void cycle() override;
};


#endif  // _DRIVE5_25_H_
