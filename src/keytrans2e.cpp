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
 

#include "keytrans2e.h"


using namespace std;


KeyTrans2e::KeyTrans2e()
{

	SDL_GetKeyState(&keyStateTotal);
	if( keyStateTotal==0 ) {
		cerr << "Unable to access key states.\n";
		exit(1);
	}
	keyConvTable = new Sint8[keyStateTotal];
	keyConvTableShift = new Sint8[keyStateTotal];
	keyConvTableCtrl = new Sint8[keyStateTotal];
	keyConvTableCaps = new Sint8[keyStateTotal];

	// Designate characters as not supported by default
	for( int k = 0; k<keyStateTotal; k++ )
		keyConvTable[k] = -1;

	// Generate list of non-modified characters
	keyConvTable[SDLK_LEFT] = 0x08;
	keyConvTable[SDLK_TAB] = 0x09;
	keyConvTable[SDLK_DOWN] = 0x0a;
	keyConvTable[SDLK_UP] = 0x0b;
	keyConvTable[SDLK_KP_ENTER] = 0x0d;
	keyConvTable[SDLK_RETURN] = 0x0d;
	keyConvTable[SDLK_RIGHT] = 0x15;
	keyConvTable[SDLK_ESCAPE] = 0x1b;
	keyConvTable[SDLK_SPACE] = 0x20;
	keyConvTable[SDLK_EXCLAIM] = 0x21;
	keyConvTable[SDLK_QUOTEDBL] = 0x22;
	keyConvTable[SDLK_HASH] = 0x23;
	keyConvTable[SDLK_DOLLAR] = 0x24;
	keyConvTable[SDLK_AMPERSAND] = 0x26;
	keyConvTable[SDLK_QUOTE] = 0x27;
	keyConvTable[SDLK_LEFTPAREN] = 0x28;
	keyConvTable[SDLK_RIGHTPAREN] = 0x29;
	keyConvTable[SDLK_ASTERISK] = 0x2a;
	keyConvTable[SDLK_KP_MULTIPLY] = 0x2a;
	keyConvTable[SDLK_KP_PLUS] = 0x2b;
	keyConvTable[SDLK_PLUS] = 0x2b;
	keyConvTable[SDLK_COMMA] = 0x2c;
	keyConvTable[SDLK_KP_MINUS] = 0x2d;
	keyConvTable[SDLK_MINUS] = 0x2d;
	keyConvTable[SDLK_KP_PERIOD] = 0x2e;
	keyConvTable[SDLK_PERIOD] = 0x2e;
	keyConvTable[SDLK_KP_DIVIDE] = 0x2f;
	keyConvTable[SDLK_SLASH] = 0x2f;
	keyConvTable[SDLK_0] = 0x30;
	keyConvTable[SDLK_KP0] = 0x30;
	keyConvTable[SDLK_1] = 0x31;
	keyConvTable[SDLK_KP1] = 0x31;
	keyConvTable[SDLK_2] = 0x32;
	keyConvTable[SDLK_KP2] = 0x32;
	keyConvTable[SDLK_3] = 0x33;
	keyConvTable[SDLK_KP3] = 0x33;
	keyConvTable[SDLK_4] = 0x34;
	keyConvTable[SDLK_KP4] = 0x34;
	keyConvTable[SDLK_5] = 0x35;
	keyConvTable[SDLK_KP5] = 0x35;
	keyConvTable[SDLK_6] = 0x36;
	keyConvTable[SDLK_KP6] = 0x36;
	keyConvTable[SDLK_7] = 0x37;
	keyConvTable[SDLK_KP7] = 0x37;
	keyConvTable[SDLK_8] = 0x38;
	keyConvTable[SDLK_KP8] = 0x38;
	keyConvTable[SDLK_9] = 0x39;
	keyConvTable[SDLK_KP9] = 0x39;
	keyConvTable[SDLK_COLON] = 0x3a;
	keyConvTable[SDLK_SEMICOLON] = 0x3b;
	keyConvTable[SDLK_LESS] = 0x3c;
	keyConvTable[SDLK_EQUALS] = 0x3d;
	keyConvTable[SDLK_KP_EQUALS] = 0x3d;
	keyConvTable[SDLK_GREATER] = 0x3e;
	keyConvTable[SDLK_QUESTION] = 0x3f;
	keyConvTable[SDLK_AT] = 0x40;
	keyConvTable[SDLK_LEFTBRACKET] = 0x5b;
	keyConvTable[SDLK_BACKSLASH] = 0x5c;
	keyConvTable[SDLK_RIGHTBRACKET] = 0x5d;
	keyConvTable[SDLK_CARET] = 0x5e;
	keyConvTable[SDLK_UNDERSCORE] = 0x5f;
	keyConvTable[SDLK_BACKQUOTE] = 0x60;
	keyConvTable[SDLK_a] = 0x61;
	keyConvTable[SDLK_b] = 0x62;
	keyConvTable[SDLK_c] = 0x63;
	keyConvTable[SDLK_d] = 0x64;
	keyConvTable[SDLK_e] = 0x65;
	keyConvTable[SDLK_f] = 0x66;
	keyConvTable[SDLK_g] = 0x67;
	keyConvTable[SDLK_h] = 0x68;
	keyConvTable[SDLK_i] = 0x69;
	keyConvTable[SDLK_j] = 0x6a;
	keyConvTable[SDLK_k] = 0x6b;
	keyConvTable[SDLK_l] = 0x6c;
	keyConvTable[SDLK_m] = 0x6d;
	keyConvTable[SDLK_n] = 0x6e;
	keyConvTable[SDLK_o] = 0x6f;
	keyConvTable[SDLK_p] = 0x70;
	keyConvTable[SDLK_q] = 0x71;
	keyConvTable[SDLK_r] = 0x72;
	keyConvTable[SDLK_s] = 0x73;
	keyConvTable[SDLK_t] = 0x74;
	keyConvTable[SDLK_u] = 0x75;
	keyConvTable[SDLK_v] = 0x76;
	keyConvTable[SDLK_w] = 0x77;
	keyConvTable[SDLK_x] = 0x78;
	keyConvTable[SDLK_y] = 0x79;
	keyConvTable[SDLK_z] = 0x7a;
	keyConvTable[SDLK_BACKSPACE] = 0x7f;
	keyConvTable[SDLK_DELETE] = 0x7f;

	// Copy non-modified keys to other key sets by default to simplify assignment
	for( int k = 0; k<keyStateTotal; k++ ) {
		keyConvTableCaps[k] = keyConvTable[k];
		keyConvTableCtrl[k] = keyConvTable[k];
	}

	// Add in key changes for caps-lock characters
	keyConvTableCaps[SDLK_a] = 0x41;
	keyConvTableCaps[SDLK_b] = 0x42;
	keyConvTableCaps[SDLK_c] = 0x43;
	keyConvTableCaps[SDLK_d] = 0x44;
	keyConvTableCaps[SDLK_e] = 0x45;
	keyConvTableCaps[SDLK_f] = 0x46;
	keyConvTableCaps[SDLK_g] = 0x47;
	keyConvTableCaps[SDLK_h] = 0x48;
	keyConvTableCaps[SDLK_i] = 0x49;
	keyConvTableCaps[SDLK_j] = 0x4a;
	keyConvTableCaps[SDLK_k] = 0x4b;
	keyConvTableCaps[SDLK_l] = 0x4c;
	keyConvTableCaps[SDLK_m] = 0x4d;
	keyConvTableCaps[SDLK_n] = 0x4e;
	keyConvTableCaps[SDLK_o] = 0x4f;
	keyConvTableCaps[SDLK_p] = 0x50;
	keyConvTableCaps[SDLK_q] = 0x51;
	keyConvTableCaps[SDLK_r] = 0x52;
	keyConvTableCaps[SDLK_s] = 0x53;
	keyConvTableCaps[SDLK_t] = 0x54;
	keyConvTableCaps[SDLK_u] = 0x55;
	keyConvTableCaps[SDLK_v] = 0x56;
	keyConvTableCaps[SDLK_w] = 0x57;
	keyConvTableCaps[SDLK_x] = 0x58;
	keyConvTableCaps[SDLK_y] = 0x59;
	keyConvTableCaps[SDLK_z] = 0x5a;
	
	// Copy caps-lock keys to shift keys
	for( int k = 0; k<keyStateTotal; k++ )
		keyConvTableShift[k] = keyConvTableCaps[k];
	
	// Add in key changes for shift characters
	keyConvTableShift[SDLK_1] = 0x21;
	keyConvTableShift[SDLK_QUOTE] = 0x22;
	keyConvTableShift[SDLK_3] = 0x23;
	keyConvTableShift[SDLK_4] = 0x24;
	keyConvTableShift[SDLK_5] = 0x25;
	keyConvTableShift[SDLK_7] = 0x26;
	keyConvTableShift[SDLK_9] = 0x28;
	keyConvTableShift[SDLK_0] = 0x29;
	keyConvTableShift[SDLK_8] = 0x2a;
	keyConvTableShift[SDLK_EQUALS] = 0x2b;
	keyConvTableShift[SDLK_SEMICOLON] = 0x3a;
	keyConvTableShift[SDLK_COMMA] = 0x3c;
	keyConvTableShift[SDLK_PERIOD] = 0x3e;
	keyConvTableShift[SDLK_SLASH] = 0x3f;
	keyConvTableShift[SDLK_2] = 0x40;
	keyConvTableShift[SDLK_6] = 0x5e;
	keyConvTableShift[SDLK_MINUS] = 0x5f;
	keyConvTableShift[SDLK_LEFTBRACKET] = 0x7b;
	keyConvTableShift[SDLK_BACKSLASH] = 0x7c;
	keyConvTableShift[SDLK_RIGHTBRACKET] = 0x7d;
	keyConvTableShift[SDLK_BACKQUOTE] = 0x7e;
			
	// Add in key changes for control characters
	keyConvTableCtrl[SDLK_AT] = 0x00;
	keyConvTableCtrl[SDLK_a] = 0x01;
	keyConvTableCtrl[SDLK_b] = 0x02;
	keyConvTableCtrl[SDLK_c] = 0x03;
	keyConvTableCtrl[SDLK_d] = 0x04;
	keyConvTableCtrl[SDLK_e] = 0x05;
	keyConvTableCtrl[SDLK_f] = 0x06;
	keyConvTableCtrl[SDLK_g] = 0x07;
	keyConvTableCtrl[SDLK_h] = 0x08;
	keyConvTableCtrl[SDLK_i] = 0x09;
	keyConvTableCtrl[SDLK_j] = 0x0a;
	keyConvTableCtrl[SDLK_KP_ENTER] = 0x0a;
	keyConvTableCtrl[SDLK_RETURN] = 0x0a;
	keyConvTableCtrl[SDLK_k] = 0x0b;
	keyConvTableCtrl[SDLK_l] = 0x0c;
	keyConvTableCtrl[SDLK_m] = 0x0d;
	keyConvTableCtrl[SDLK_n] = 0x0e;
	keyConvTableCtrl[SDLK_o] = 0x0f;
	keyConvTableCtrl[SDLK_p] = 0x10;
	keyConvTableCtrl[SDLK_q] = 0x11;
	keyConvTableCtrl[SDLK_r] = 0x12;
	keyConvTableCtrl[SDLK_s] = 0x13;
	keyConvTableCtrl[SDLK_t] = 0x14;
	keyConvTableCtrl[SDLK_u] = 0x15;
	keyConvTableCtrl[SDLK_v] = 0x16;
	keyConvTableCtrl[SDLK_w] = 0x17;
	keyConvTableCtrl[SDLK_x] = 0x18;
	keyConvTableCtrl[SDLK_y] = 0x19;
	keyConvTableCtrl[SDLK_z] = 0x1a;
	keyConvTableCtrl[SDLK_CARET] = 0x1e;
	keyConvTableCtrl[SDLK_UNDERSCORE] = 0x1f;

}

KeyTrans2e::~KeyTrans2e()
{
	delete [] keyConvTable;
	delete [] keyConvTableShift;
	delete [] keyConvTableCtrl;
	delete [] keyConvTableCaps;
}

bool KeyTrans2e::translate( const SDL_keysym & hostKey, Uint8 & key2e )
{

	Sint8* table;
	SDLKey key = hostKey.sym;
	assert(key<keyStateTotal);
	
	// If key has an ASCII code reference return it
	if( hostKey.mod & (KMOD_LCTRL|KMOD_RCTRL) ) {
		if( hostKey.mod & (KMOD_LSHIFT|KMOD_RSHIFT ) ) {
			if( key==SDLK_2 ) key = SDLK_AT;
			else if( key==SDLK_6 ) key = SDLK_CARET;
			else if( key==SDLK_MINUS ) key = SDLK_UNDERSCORE;
		}
		table = keyConvTableCtrl;
	}
	else if( hostKey.mod & (KMOD_LSHIFT|KMOD_RSHIFT ) )
		table = keyConvTableShift;
	else if( hostKey.mod & KMOD_CAPS )
		table = keyConvTableCaps;
	else
		table = keyConvTable;
	if( table[key]>=0 ) {
		key2e = (Uint8) table[key];
		return true;
	}
	else
		return false;

}

SDLKey KeyTrans2e::getOpenAppleKey()
{
	return SDLK_LALT;
}

SDLKey KeyTrans2e::getClosedAppleKey()
{
	return SDLK_RALT;
}

SDLKey KeyTrans2e::getResetKey()
{
	return SDLK_F12;
}
