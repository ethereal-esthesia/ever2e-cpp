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


#ifndef _MON560x192_H_
#define _MON560x192_H_

#include "pixel.h"
#include "colorconv.h"
#include "memory128k.h"
#include "savestate.h"


enum MonitorType { MONO_MONITOR, GREEN_MONITOR, COLOR_MONITOR };


class Monitor560x192
{

private:

	static const Uint16 GR_TO_DHGR[16*2];
	static const Uint16 HGR_TO_DHGR[128];

	static const int PAL_BRIGHTNESS = 104;

	// For potential future emulation of 7-bit shift for 40/80 display
	static const int DHIRES_OFFSET = 7;

	class PixelSurface* surface;
	class Memory128k* memory;

	// Timings - Sather 3-5

public:

	static const int X_RES = 560;
	static const int Y_RES = 192;
	static const int X_MULT = 1;
	static const int Y_MULT = 2;
	static const int X_OFFSET = 0;
	static const int Y_OFFSET = 0;

	static const Sint32 HORIZONTAL_CPU_SYNC_TIMING = 139;  // Extra time on last cycle of horizontal scan taken to sync CPU to the horizontal refresh rate

	static const Uint16 HORIZONTAL_BLANK = 40;
	static const Uint16 HORIZONTAL_SCANS = 65;
	static const Uint16 VERTICAL_BLANK = 192;
	static const Uint16 VERTICAL_SCANS = 262;
	static const Uint16 TOP_PANEL_SIZE = 160;

	static const Uint8 CHAR_MAP[3*256*8];

private:

	Uint8 hRefreshCount;
	Uint16 vRefreshCount;
	Uint16 xc;
	Uint16 xOff, yOff;
	Uint8 overflowCount;

	bool flashOn;
	bool hiresCarryBit;
	bool whitePalCarry;
	Uint32 word32bit;
	int word32bitSize;
	bool idleState;
	bool updateOff;

	enum TextType {NORMAL_TEXT, FLASH_TEXT, MOUSE_TEXT}
		textType;
	enum DisplayType {TEXT40, TEXT80, LORES40, LORES40M, LORES80, HIRES40, HIRES40M, HIRES80}
		topPanel, bottomPanel;
	DisplayType currentDisplayType;

	Uint16 readPage;  // Can be 1 or 2
	Uint16 loresReadAddress;
	Uint16 hiresReadAddress;

	enum PalType {MONO_PAL, GREEN_PAL, COLOR_PAL, COLOR80_PAL}
		palType;
	Uint32 applePalette[6*16];
	Uint8 paletteHue;
	bool solidColor;
	bool scanlineSkip;		
	bool colorPalReserved;

	void _setApplePalette();

	void _setPalType( PalType type );

public:

	Monitor560x192( class PixelSurface* surface, class Memory128k* memory );

	~Monitor560x192();

	void commitSwitches();
	
	void resetAll();
		// Restores all settings to initial state
	
	void setOffset( Uint16 xOff, Uint16 yOff );

	Uint8 getHRefreshCount();
	
	Uint16 getVRefreshCount();

	bool getVbl();
	
	Uint8 getOverflowCount();

	bool getScanlineSkip();
	
	void setScanlineSkip( bool status );

	MonitorType getMonitorType();
		// Valid types are "MONO_MONITOR", "GREEN_MONITOR", and "COLOR_MONITOR"
	
	void setMonitorType( MonitorType type );
		// Valid types are "MONO_MONITOR", "GREEN_MONITOR", and "COLOR_MONITOR"
	
	Uint8 getHue();
	
	void setHue( Uint8 hue );

	bool getSolidColor();
	
	void setSolidColor( bool status );

	void setIdleState( bool state );
		// If idle is true, shows current display frame at 25% brightness and calls to cycle are ignored
		// If idle is false, restores full display brightness

	bool getIdleState();
		// Returns true if monitor is set to idle, otherwise false
		
	void refreshFrame();
		// Displays current frame of display without updating emulation state
		
	void cycle();

	static void getPositionLores( Uint16 address, Uint16& page, Uint16& scanline, Uint16& offset );

	static void getPositionHires( Uint16 address, Uint16& page, Uint16& scanline, Uint16& offset );

	static Uint16 getAddressLo40( Uint16 page, Uint16 scanline, Uint16 offset );

	static Uint16 getAddressHi40( Uint16 page, Uint16 scanline, Uint16 offset );

	enum States{ SETTING_STATE = 0x01, HARDWARE_STATE = 0x02, ALL_STATES = 0x03 };
	
	void store( SaveState &state, States states = HARDWARE_STATE );
		// Stores specified variables to "state"
		// A backup state can be created beforehand if there is a chance that read data is corrupted
		//   either due to a version change, altered data, or a damaged file
		// A call to "restore" will indicate if there is any unreadable data
		// Note: idle state and pixel offset are not stored by this function

	int restore( SaveState &state, States states = HARDWARE_STATE );
		// Restores specified variables from "state"
		// Returns 0 if successful
		// Returns 1 if state data is corrupted
		// Upon finding a read error, all state values are restored to their defaults
		// Note: idle state and pixel offset are not changed by this function

};


#endif  // _MON560x192_H_
