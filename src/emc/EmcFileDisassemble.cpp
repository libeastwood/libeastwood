/* Based on:
 * DST  -  Dune 2 Script Tools
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
 */

#include <iostream>
#include <iomanip>
#include <string.h>

#include "StdDef.h"

#include "emc/EmcFileBase.h"
#include "emc/EmcFileDisassemble.h"
#include "emc/EmcFileInternal.h"

#include "Exception.h"

namespace eastwood {

EmcFileDisassemble::EmcFileDisassemble(std::istream &input, std::ostream &output) :
    EmcFileBase(input, output), _scriptLastPush(0)
{
}

EmcFileDisassemble::~EmcFileDisassemble() {
}

bool EmcFileDisassemble::scriptLoad() {
    size_t scriptSize;


    // Read File Size
    scriptSize = getStreamSize(_inputStream);
    if(scriptSize < sizeof(emcHeader))
	throw(FileException(LOG_ERROR, "EmcFile", "_inputStream", "File is too small!"));

    // Verify header
    char fileHeader[sizeof(emcHeader)];
    _inputStream.get(fileHeader, sizeof(fileHeader));
    if(memcmp(emcHeader, fileHeader, 4) || memcmp(emcHeader+8, fileHeader+8, sizeof(emcHeader)-9))
	throw(FileException(LOG_ERROR, "EmcFile", "_inputStream", "Invalid header!"));

    _inputStream.seekg(3, std::ios::cur);
    scriptSize -= _inputStream.tellg();
    // Load file into _scriptBuffer
    _scriptBuffer = new uint8_t[scriptSize];
    if(_inputStream.read((char*) _scriptBuffer, scriptSize) == false)
	return false;

    return true;
}

bool EmcFileDisassemble::headerRead() {
    uint16_t *buffer = (uint16_t*) _scriptBuffer;

    // Number of script functions
    _pointerCount = htobe16(*buffer) / 2;
    buffer++;

    _headerPointers = new uint16_t[_pointerCount];

    for(size_t ptrCount = 0; ptrCount < _pointerCount; ptrCount++) {
	_headerPointers[ptrCount] = htobe16(*buffer);
	buffer++;
    }

    // Skip 'DATA' tag
    buffer += 2;	// 2 Words

    // Not sure if this uint16_t is actually part of a DWORD, or 2 useless uint8_ts?
    buffer++;

    // Size of the upcoming script data
    _scriptSize = htobe16(*buffer);
    buffer++;

    // Start of script data
    _scriptStart = (uint8_t*) buffer;

    opcodesSetup();

    switch(_scriptType) {
	case script_HOUSE:
    	    _outputStream << "[House]" << std::endl;
	    break;
	case script_BUILD:
    	    _outputStream << "[Build]" << std::endl;
	    break;
	case script_UNIT:
    	    _outputStream << "[Unit]" << std::endl;
	    break;
    }

    return true;
}

bool EmcFileDisassemble::scriptNextStart() {
    bool found = false;

    // Loop through each pointer and write the section names out
    for(int count = 0; count < _pointerCount; count++) {

	// In TEAM.EMC for example, two objects use the same script
	if(_scriptPos == (uint16_t) _headerPointers[count]) {
	    if(found==false)
		_outputStream << std::endl;

	    // Write the section name in square brackets
	    _outputStream << "[" << _objectNames[count] << "]" << std::endl;
	    found = true;
	}
    }

    return found;
}

bool EmcFileDisassemble::execute() {

    // Load the script into a _scriptBuffer, then read the header information
    scriptLoad();
    headerRead();

    // Map out all labels
    _modePreProcess = true;
    if(scriptDisassemble() == false)
	return false;

    // Disassemble the script 
    _modePreProcess = false;

    _outputStream << "[General]" << std::endl;

    return scriptDisassemble();
}

bool EmcFileDisassemble::scriptDisassemble() {

    _lineCount		= 0;
    _opcodeCurrent	= 0;
    _scriptData		= _scriptDataNext = 0;
    _scriptPtr		= NULL;
    _scriptPtrEnd	= NULL;
    _scriptPos		= 0;
    _stackCount		= 0xF;

    _scriptPos		= 0;
    _scriptPtr		= (uint16_t*) _scriptStart;
    _scriptPtrEnd	= (uint16_t*) (_scriptStart + _scriptSize);

    while(_scriptPtr <  _scriptPtrEnd) {

	if(!_modePreProcess) {
	    scriptNextStart();

	    // Check if label location, print label if so
	    if(scriptLabel(_scriptPos) != (size_t)-1)
		_outputStream << "l" << _scriptPos << ":" << std::endl;
	}

	_scriptDataNext = 0;
	_scriptData = htobe16(*_scriptPtr);
	_scriptPtr++;
	_scriptPos++;

	// Keep the opcode
	_opcodeCurrent = _scriptData >> 8;
	_opcodeCurrent &= 0x1F;

	if(_scriptData & 0x8000) {
	    // Opcode uses 13 bits
	    _opcodeCurrent = 0;
	    _scriptData &= 0x7FFF;
	} else
	    // Opcode only requires 1 uint8_t
	    if(_scriptData & 0x4000) {
		_scriptData &= 0xFF;
	    } else 	
		// Opcode uses the next WORD, grab it
		if(_scriptData & 0x2000) {
		    _scriptDataNext = htobe16(*_scriptPtr);
		    _scriptPtr++;
		    _scriptPos++;
		}


	    // Print opcode
	    if(!_modePreProcess)
		_outputStream << std::setw(20) << std::left << _opcodes[_opcodeCurrent].description;

	    // Excute opcode
	    (this->*_opcodes[_opcodeCurrent].function)();

	    //_outputStream  << setw(20) << " ";
	    //_outputStream  << hex << uppercase << "S: 0x" << _stackCount << std::endl;
	    if(!_modePreProcess)
		_outputStream << std::endl;

	    _lineCount++;
    }

    return true;
}

void EmcFileDisassemble::o_Goto() {
    size_t labelPos = scriptLabel(_scriptData);

    if(!_modePreProcess) {

	if(labelPos == (size_t)-1)
	    dataPrint(_scriptData);
	else
	    _outputStream << "l" << _scriptLabels[labelPos]._scriptPos;

    } else {
	if(labelPos == (size_t)-1)
	    scriptLabelAdd("", _scriptData);
    }
}

void EmcFileDisassemble::o_SetReturn() {
    if(_scriptDataNext)
	dataPrint(_scriptDataNext);
    else
	dataPrint(_scriptData);
}

void EmcFileDisassemble::o_PushOp() {
    uint16_t data = _scriptData;
    if(_scriptData == 0)
	data = _scriptDataNext;

    _stackCount--;

    dataPrint(data);
}

void EmcFileDisassemble::o_Push() {
    _stackCount--;

    if(_scriptDataNext) {
	_scriptLastPush = _scriptDataNext;
	dataPrint(_scriptDataNext);
    } else {
	_scriptLastPush = _scriptData;
	dataPrint(_scriptData);
    }
}

void EmcFileDisassemble::o_PushWord() {
    o_Push();
}

void EmcFileDisassemble::o_PushReg() {
    _stackCount--;

    if(_scriptDataNext)
	dataPrint(_scriptDataNext);
    else
	dataPrint(_scriptData);
}

void EmcFileDisassemble::o_PushFrameMinArg() {
    _stackCount--;
    _stackCount--;
    if(_scriptDataNext)
	dataPrint(_scriptDataNext);
    else
	dataPrint(_scriptData);
}

void EmcFileDisassemble::o_PushFramePluArg() {
    _stackCount--;
    _stackCount--;
    if(_scriptDataNext)
	dataPrint(_scriptDataNext);
    else
	dataPrint(_scriptData);
}

void EmcFileDisassemble::o_Pop() {
    if(_scriptData == 1) {
	if(!_modePreProcess)
	    _outputStream << " (Return)";
	return;

    } else 
	_stackCount++;

    _stackCount++;
}

void EmcFileDisassemble::o_PopReg() {
    _stackCount++;

    if(_scriptDataNext)
	dataPrint(_scriptDataNext);
    else
	dataPrint(_scriptData);
}

void EmcFileDisassemble::o_PopFrameMinArg() {
    _stackCount++;
    _stackCount++;
    if(_scriptDataNext)
	dataPrint(_scriptDataNext);
    else
	dataPrint(_scriptData);
}

void EmcFileDisassemble::o_PopFramePluArg() {
    _stackCount++;
    _stackCount++;
    if(_scriptDataNext)
	dataPrint(_scriptDataNext);
    else
	dataPrint(_scriptData);
}

void EmcFileDisassemble::o_AddSP() {
    dataPrint(_scriptData);
    _stackCount += (_scriptData & 0xF);
}

void EmcFileDisassemble::o_SubSP() {
    dataPrint(_scriptData);
    _stackCount -= (_scriptData & 0xF);
}

void EmcFileDisassemble::o_Execute() {

    if(!_modePreProcess)
	_outputStream << std::left << _opcodesExecute[ _scriptData ].description << " ";

    (this->*_opcodesExecute[ _scriptData ].function)();
}

void EmcFileDisassemble::o_IfNotGoto() {
    size_t labelPos;

    if(_scriptDataNext) {
	labelPos = scriptLabel(_scriptDataNext & 0x7FFF);

	if(_modePreProcess)  {
	    if(labelPos == (size_t)-1)
		scriptLabelAdd("", _scriptDataNext & 0x7FFF);

	} else {
	    if(labelPos == (size_t)-1)
		dataPrint(_scriptDataNext & 0x7FFF);
	    else
		_outputStream << "l" << _scriptLabels[labelPos]._scriptPos;
	}

    } else {
	labelPos = scriptLabel(_scriptData);

	if(_modePreProcess) {
	    if(labelPos == (size_t)-1)
		scriptLabelAdd("", _scriptData);
	} else {
	    if(labelPos == (size_t)-1)
		dataPrint(_scriptData);
	    else
		_outputStream << "l" << _scriptLabels[labelPos]._scriptPos;
	}
    }
}

void EmcFileDisassemble::o_Negate() {
    dataPrint(_scriptData);
}

void EmcFileDisassemble::o_Evaluate() {
    if(!_modePreProcess)
	_outputStream << _opcodesEvaluate[ _scriptData ].description;

    (this->*_opcodesEvaluate[ _scriptData ].function)();
}

void EmcFileDisassemble::o_Return() {

}


void EmcFileDisassemble::o_execute_Unit_GetDetail() {
    if(!_modePreProcess)
    {
	// Unit GetDetail Function DetailNames
	static const char *nameUnitDetails[] = {
	    "HitPointRepairCalc?",   
	    "BuildingTypeIndex",     
	    "WeaponRange",           
	    "IndexGet",              
	    "Field64Get",            
	    "AttackObjectIndexTypeGet",                   
	    "Return",
	    "TypeIndex",             
	    "IndexGetAsObject",      
	    "Field6BGet",                 
	    "loc_27562",          
	    "IsMoving",
	    "WeaponCoolDownTimer",
	    "OptsFitW",
	    "HouseIDGet",            
	    "loc_275C1",                 
	    "TurretGet?",     
	    "loc_2752F",                 
	    "CanTurretRotates",          
	    "CheckIfHuman"              
	};
	_outputStream << "(" << nameUnitDetails[_scriptLastPush] << ")";
    }
}

}
