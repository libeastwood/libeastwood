#ifndef EASTWOOD_EMC_STDAFX_H
#define EASTWOOD_EMC_STDAFX_H 1

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

#include "../StdDef.h"
#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>

// Endian functions
// Read a uint16_t from the buffer
static inline uint16_t	readWord( const uint16_t *buffer ) {
    const uint8_t *uint16_tByte = (const uint8_t *) buffer;
    return (uint16_tByte[0] << 8) + uint16_tByte[1];
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

#endif // EASTWOOD_EMC_STDAFX_H
