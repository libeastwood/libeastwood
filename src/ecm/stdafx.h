/* DST  -  Dune 2 Script Tools
 *  
 * Copyright (C) 2009 segra		<segra@strobs.com>
 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *  
 * 
 * $Id: stdafx.h 21 2009-04-11 01:11:32Z segra $
 * 
 */

#pragma once

#include <cstring>
#include <cstdlib>
#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>

using namespace std;

typedef	unsigned char	byte;
typedef unsigned short	word;

// Endian functions
// Read a word from the buffer
inline word	readWord( const void *buffer ) {
	const byte *wordByte = (const byte *) buffer;
	return (wordByte[0] << 8) + wordByte[1];
}

// Endian swap a word
inline word	swapWord(  word buffer ) {
	const byte *wordByte = (const byte *) &buffer;
	return (wordByte[0] << 8) + wordByte[1];
}

enum _scriptTypes {
	_scriptBUILD = 0,
	_scriptUNIT,
	_scriptHOUSE
};

extern const char *version;
extern const char *nameHouses[0x06];
extern const char *nameStructures[0x13];
extern const char *nameUnits[0x1B];
extern const char *nameUnitDetails[0x14];
