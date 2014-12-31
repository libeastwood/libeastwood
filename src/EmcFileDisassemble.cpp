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

#include "eastwood/StdDef.h"

#include "eastwood/EmcFileBase.h"
#include "eastwood/EmcFileDisassemble.h"

#include "eastwood/Exception.h"

namespace eastwood {

EmcFileDisassemble::EmcFileDisassemble(std::istream &input, std::ostream &output) :
    EmcFileBase(input, output), _scriptLastPush(0), _opcodeCurrent(0), _stackCount(0),
    _scriptData(0), _scriptDataNext(0), _scriptPtrEnd(nullptr)
{
}

EmcFileDisassemble::~EmcFileDisassemble() {
}

bool EmcFileDisassemble::scriptLoad() {
    size_t scriptSize;

    // Read File Size
    scriptSize = _inputStream.sizeg();
    if(scriptSize < sizeof(emcHeader))
	throw(FileException(LOG_ERROR, __FUNCTION__, "File is too small!"));

    // Verify header
    char fileHeader[sizeof(emcHeader)];
    _inputStream.get(fileHeader, sizeof(fileHeader));
    if(memcmp(emcHeader, fileHeader, 4) || memcmp(emcHeader+8, fileHeader+8, sizeof(emcHeader)-9))
	throw(FileException(LOG_ERROR, __FUNCTION__, "Invalid header!"));

    _inputStream.seekg(3, std::ios::cur);
    scriptSize -= _inputStream.tellg();
    // Load file into _scriptBuffer
    _scriptBuffer = new uint8_t[scriptSize];
    if(!_inputStream.read(reinterpret_cast<char*>(_scriptBuffer), scriptSize))
	return false;

    return true;
}

bool EmcFileDisassemble::headerRead() {
    uint16_t *buffer = reinterpret_cast<uint16_t*>(_scriptBuffer);

    // Number of script functions
    _pointerCount = htobe16(*buffer++) / 2;

    _headerPointers = new uint16_t[_pointerCount];

    for(size_t ptrCount = 0; ptrCount < _pointerCount; ptrCount++)
	_headerPointers[ptrCount] = htobe16(*buffer++);

    // Skip 'DATA' tag
    buffer += 2;	// 2 Words

    // Not sure if this uint16_t is actually part of a DWORD, or 2 useless uint8_ts?
    buffer++;

    // Size of the upcoming script data
    _scriptSize = htobe16(*buffer++);

    // Start of script data
    _scriptStart = reinterpret_cast<uint8_t*>(buffer);

    opcodesSetup();

    switch(_scriptType) {
	case script_TEAM:
    	    _outputStream << "[Team]" << std::endl;
	    break;
	case script_BUILD:
    	    _outputStream << "[Build]" << std::endl;
	    break;
	case script_UNIT:
    	    _outputStream << "[Unit]" << std::endl;
	    break;
	default:
	    return false;
    }

    return true;
}

bool EmcFileDisassemble::scriptNextStart() {
    bool found = false;

    // Loop through each pointer and write the section names out
    for(int count = 0; count < _pointerCount; count++) {

	// In TEAM.EMC for example, two objects use the same script
	if(_scriptPos == static_cast<uint16_t>(_headerPointers[count])) {
	    if(!found)
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
    if(!scriptDisassemble())
	return false;

    // Disassemble the script 
    _modePreProcess = false;

    _outputStream << "[General]" << std::endl;

    return scriptDisassemble();
}

bool EmcFileDisassemble::scriptDisassemble() {
    _stackCount		= 0xF,
    _scriptPos		= 0,
    _scriptPtr		= reinterpret_cast<uint16_t*>(_scriptStart),
    _scriptPtrEnd	= reinterpret_cast<uint16_t*>(_scriptStart + _scriptSize);

    while(_scriptPtr <  _scriptPtrEnd) {

	if(!_modePreProcess) {
	    scriptNextStart();

	    // Check if label location, print label if so
	    if(scriptLabel(_scriptPos) != static_cast<size_t>(-1))
		_outputStream << "l" << _scriptPos << ":" << std::endl;
	}

	_scriptDataNext = 0,
	_scriptData = htobe16(*_scriptPtr),
	_scriptPtr++,
	_scriptPos++;

	// Keep the opcode
	_opcodeCurrent = _scriptData >> 8;
	_opcodeCurrent &= 0x1F;

	if(_scriptData & 0x8000)
	    // Opcode uses 13 bits
	    _opcodeCurrent = 0,
	    _scriptData &= 0x7FFF;
	else
	    // Opcode only requires 1 uint8_t
	    if(_scriptData & 0x4000)
		_scriptData &= 0xFF;
	    else 	
		// Opcode uses the next WORD, grab it
		if(_scriptData & 0x2000)
		    _scriptDataNext = htobe16(*_scriptPtr),
		    _scriptPtr++,
		    _scriptPos++;


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

void EmcFileDisassemble::o_goto() {
    size_t labelPos = scriptLabel(_scriptData);

    if(!_modePreProcess) {

	if(labelPos == static_cast<size_t>(-1))
	    dataPrint(_scriptData);
	else
	    _outputStream << "l" << _scriptLabels[labelPos]._scriptPos;

    } else {
	if(labelPos == static_cast<size_t>(-1))
	    scriptLabelAdd("", _scriptData);
    }
}

void EmcFileDisassemble::o_setreturn() {
    if(_scriptDataNext)
	dataPrint(_scriptDataNext);
    else
	dataPrint(_scriptData);
}

void EmcFileDisassemble::o_pushOp() {
    uint16_t data = _scriptData;
    if(_scriptData == 0) {
	data = _scriptDataNext;
	_stackCount--;
    }
    if(_scriptData == 1) {
	_stackCount--;
	_stackCount--;
    }

    dataPrint(data);
}

void EmcFileDisassemble::o_push() {
    _stackCount--;

    if(_scriptDataNext) {
	_scriptLastPush = _scriptDataNext;
	dataPrint(_scriptDataNext);
    } else {
	_scriptLastPush = _scriptData;
	dataPrint(_scriptData);
    }
}

void EmcFileDisassemble::o_pushWord() {
    o_push();
}

void EmcFileDisassemble::o_pushreg() {
    _stackCount--;

    if(_scriptDataNext)
	dataPrint(_scriptDataNext);
    else
	dataPrint(_scriptData);
}

void EmcFileDisassemble::o_pushframeMinArg() {
    _stackCount--,
    _stackCount--;
    if(_scriptDataNext)
	dataPrint(_scriptDataNext);
    else
	dataPrint(_scriptData);
}

void EmcFileDisassemble::o_pushframePluArg() {
    _stackCount--,
    _stackCount--;
    if(_scriptDataNext)
	dataPrint(_scriptDataNext);
    else
	dataPrint(_scriptData);
}

void EmcFileDisassemble::o_popret() {
    if(_scriptData == 1) {
	if(!_modePreProcess)
	    _outputStream << " (Return)";
	return;

    } else 
	_stackCount++;

    _stackCount++;
}

void EmcFileDisassemble::o_popreg() {
    _stackCount++;

    if(_scriptDataNext)
	dataPrint(_scriptDataNext);
    else
	dataPrint(_scriptData);
}

void EmcFileDisassemble::o_popframeMinArg() {
    _stackCount++,
    _stackCount++;
    if(_scriptDataNext)
	dataPrint(_scriptDataNext);
    else
	dataPrint(_scriptData);
}

void EmcFileDisassemble::o_popframePluArg() {
    _stackCount++,
    _stackCount++;
    if(_scriptDataNext)
	dataPrint(_scriptDataNext);
    else
	dataPrint(_scriptData);
}

void EmcFileDisassemble::o_spadd() {
    dataPrint(_scriptData);
    _stackCount += (_scriptData & 0xF);
}

void EmcFileDisassemble::o_spsub() {
    dataPrint(_scriptData);
    _stackCount -= (_scriptData & 0xF);
}

void EmcFileDisassemble::o_execute() {

    if(!_modePreProcess)
	_outputStream << std::left << _opcodesExecute[ _scriptData ].description << " ";

    (this->*_opcodesExecute[ _scriptData ].function)();
}

void EmcFileDisassemble::o_ifnotgoto() {
    size_t labelPos;

    if(_scriptDataNext) {
	labelPos = scriptLabel(_scriptDataNext & 0x7FFF);

	if(_modePreProcess)  {
	    if(labelPos == static_cast<size_t>(-1))
		scriptLabelAdd("", _scriptDataNext & 0x7FFF);

	} else {
	    if(labelPos == static_cast<size_t>(-1))
		dataPrint(_scriptDataNext & 0x7FFF);
	    else
		_outputStream << "l" << _scriptLabels[labelPos]._scriptPos;
	}

    } else {
	labelPos = scriptLabel(_scriptData);

	if(_modePreProcess) {
	    if(labelPos == static_cast<size_t>(-1))
		scriptLabelAdd("", _scriptData);
	} else {
	    if(labelPos == static_cast<size_t>(-1))
		dataPrint(_scriptData);
	    else
		_outputStream << "l" << _scriptLabels[labelPos]._scriptPos;
	}
    }
}

void EmcFileDisassemble::o_negate() {
    dataPrint(_scriptData);
}

void EmcFileDisassemble::o_evaluate() {
    if(!_modePreProcess)
	_outputStream << _opcodesEvaluate[ _scriptData ].description;

    (this->*_opcodesEvaluate[ _scriptData ].function)();
}

void EmcFileDisassemble::o_return() {

}


void EmcFileDisassemble::o_execute_Unit_GetDetail() {
    if(!_modePreProcess)
    {
	// Unit GetDetail Function DetailNames
	static const char *nameUnitDetails[] = {
	    "HitPointRepairCalc?",
	    "HoldingTypeIndex",
	    "WeaponRange",
	    "IndexGet",
	    "Anim1NewAngle",
	    "TargetScriptObject",
	    "Return",
	    "TypeIndex",
	    "IndexGetAsObject",
	    "Field6BGet",
	    "Anim1AngleToMove",
	    "IsMoving",
	    "WeaponCoolDownTimer",
	    "GetExplosionBits",
	    "HouseIDGet",
	    "CheckState200",
	    "anim2NewAngle",
	    "anim2AngleToMove",
	    "CanTurretRotates",
	    "CheckIfHuman"
	};
	_outputStream << "(" << nameUnitDetails[_scriptLastPush] << ")";
    }
}

}
