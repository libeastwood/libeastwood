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

#include <fstream>
#include <iostream>

#include "eastwood/StdDef.h"

#include "eastwood/EmcFileBase.h"
#include "eastwood/EmcFileAssemble.h"

namespace eastwood {

EmcFileAssemble::EmcFileAssemble(std::istream &input, std::ostream &output) :
    EmcFileBase(input, output), _currentLine(""), _opcodeCurrent(""), _data(""),
    _opcode(0)
{
}

EmcFileAssemble::~EmcFileAssemble() {
}


bool EmcFileAssemble::headerCreate() {
    uint16_t *buffer = reinterpret_cast<uint16_t*>(_scriptBuffer);

    // Copy the header in
    memcpy(reinterpret_cast<uint8_t*>(buffer), emcHeader, sizeof(emcHeader));

    buffer += 0x3;

    *buffer = htobe16(_scriptSize + (_pointerCount*2) + 0x1C);

    // 0x6 is WORDS, not bytes like 0x10 above
    buffer += 0x6;

    // Write pointer counter
    *buffer++ = htobe16(_pointerCount * 2);

    // Write ID script pointers
    for(size_t ptrCount = 0; ptrCount < _pointerCount; ptrCount++)
	*buffer++ = htobe16(_headerPointers[ptrCount]);

    buffer += snprintf(reinterpret_cast<char*>(buffer), 5, "DATA")-1;

    // Write total script size
    *buffer = htobe16(_scriptSize);

    return true;
}

bool EmcFileAssemble::scriptSave() {
    // Set the header/pointers in the buffer
    headerCreate();

    // Write the header, the pointers and the script
    _outputStream.write(reinterpret_cast<char*>(_scriptBuffer), _scriptSize + (_pointerCount*2) + 0x1C);

    return true;
}

int EmcFileAssemble::scriptSectionCheck() {
    int count = 0;
    size_t posStart = _currentLine.find("[") + 1,
       	   posEnd;

    // Not a section header
    if(posStart == std::string::npos)
	return -2;

    posEnd = _currentLine.find("]", posStart);

    // Read the section name
    std::string name = _currentLine.substr(posStart, posEnd - posStart);

    // Check for the section name
    for(const char **namePtr = _objectNames; *namePtr; *namePtr++, count++)
	if(name.compare(*namePtr) == 0)
	    return count;

    // General Section
    return -1;
}

bool EmcFileAssemble::execute() {

    _scriptBuffer = new uint8_t[0x100000];			// should be big enough :p
    memset(reinterpret_cast<void*>(_scriptBuffer), 0, 0x100000);

    // Run the script assembler in pre process mode (find all jump locations)
    if(!scriptAssemble())
	return false;

    // Cleanup header pointers
    if(_headerPointers)
	delete [] _headerPointers;

    // Disable PreProcess mode
    _modePreProcess = false;

    // Properly assemble the script
    if(!scriptAssemble())
	return false;

    // Save the _scriptBuffer to disk
    return scriptSave();
}

// -1 Disassembles the script at _scriptStart
bool EmcFileAssemble::scriptAssemble() {
    int	objectID = 0;
    std::streamoff lastOffset = _inputStream.sizeg()-1;

    _lineCount = 0;

    // Reset the script position and the size
    _scriptSize	= 0;
    _scriptPos	= 0;

    _inputStream.seekg(0, std::ios::beg);

    // Read file type line
    _inputStream >> _currentLine;
    _lineCount++;

    opcodesSetup(_currentLine);

    // Prepare memory for scriptBuffer and headerPointers
    _headerPointers = new uint16_t[_pointerCount];

    // Set the script pointer to the script starting position (in buffer)
    _scriptPtr = reinterpret_cast<uint16_t*>(_scriptBuffer + (_pointerCount*2) + 0x1C);

    // Clear memory
    memset(reinterpret_cast<void*>(_headerPointers), 0, _pointerCount * 2);

    // Loop until end of source file
    while(static_cast<std::streamoff>(_inputStream.tellg()) < lastOffset) {

	// Read next line from sourcecode file
	_inputStream >> _currentLine;

	// Find the opcode in the opcode table
	_opcode = 0xFFFF;
	while(_opcode == 0xFFFF) {
	    // Check for a header pointer
	    objectID = scriptSectionCheck();

	    // object script pointer location found
	    if(objectID != -1)
		_headerPointers[objectID] = _scriptPos;		

	    // Find the opcode
	    _opcode = scriptOpcodeFind(_currentLine, _opcodes);

	    // Check for label
	    if(_modePreProcess &&  (_currentLine.find(":") != std::string::npos)) {

		// Add the label to the labels vector
		scriptLabelAdd(_currentLine, _scriptPos);
	    }

	    // Did we reach end of file?
	    if(_inputStream.eof())
		break;

	    // Check if the opcode is valid
	    if(_opcode == 0xFFFF) {
		_inputStream >> _currentLine;
		continue;
	    }

	    // Skip the spaces and check the next character
	    while((_inputStream.get()) == 0x20);

	    _inputStream.seekg(-1, std::ios::cur);
	    // Is it end of line? or is it a parameter?
	    if(_inputStream.peek() != '\n')
		_inputStream >> _currentLine;
	}

	if(_opcode == 0xFFFF)
	    break;

	_lineCount++;

	// Write bytecode
	*_scriptPtr = _opcode;

	// Execute opcode call
	(this->*_opcodes[_opcode].function)();

	// Debugging Use
	//if(((uint8_t*) _scriptPtr) - _scriptBuffer > 0x89B)
	//	std::cout << "a";

	// Next Line Number
	_scriptPos++;

	// Next Word in _scriptBuffer pointer
	_scriptPtr++;
    }

    // Set the size of the script to the Line numbers * 2 (sizeof uint16_t)
    _scriptSize = _scriptPos * 2;

    return true;
}

void EmcFileAssemble::o_Goto() {
    size_t labelPos = scriptLabelGet(_currentLine);
    int bb = 0;

    // huh? this can't be right???
    if(!_modePreProcess && labelPos == static_cast<size_t>(-1))
	*(reinterpret_cast<uint8_t*>(bb)) = 01;

    *(_scriptPtr) |= 0x80;
    *(_scriptPtr) |= htobe16(labelPos);

}

void EmcFileAssemble::o_SetReturn() {
    //*(_scriptPtr) |= 0x40;

}

void EmcFileAssemble::o_PushOp() {
    *(_scriptPtr) |= 0x40;
    *(_scriptPtr) |= htobe16(atoi(_currentLine.c_str()));
}

void  EmcFileAssemble::o_PushWord() {
    *(_scriptPtr) |= 0x20;
    *(_scriptPtr+1) |= htobe16(atoi(_currentLine.c_str()));
    _scriptPtr++;
    _scriptPos++;
}

void EmcFileAssemble::o_Push() {
    uint16_t value = atoi(_currentLine.c_str());

    // Just incase, we dont want to corrupt the opcode
    if(value > 0xFF)
	return o_PushWord();

    *(_scriptPtr)  = 0x04;
    *(_scriptPtr) |= 0x40;
    *(_scriptPtr) |= htobe16(value);
}

void EmcFileAssemble::o_PushReg() {
    *(_scriptPtr) |= 0x40;
    *(_scriptPtr) |= htobe16(atoi(_currentLine.c_str()));
}

void EmcFileAssemble::o_PushFrameMinArg() {
    *(_scriptPtr) |= 0x40;
    *(_scriptPtr) |= htobe16(atoi(_currentLine.c_str()));
}

void EmcFileAssemble::o_PushFramePluArg() {
    *(_scriptPtr) |= 0x40;
    *(_scriptPtr) |= htobe16(atoi(_currentLine.c_str()));
}

void EmcFileAssemble::o_Pop() {
    *(_scriptPtr) |= 0x40;

    if(_currentLine == "(Return)")
	_currentLine = "1";
    else
	_currentLine = "0";

    *(_scriptPtr) |= htobe16(atoi(_currentLine.c_str()));
}

void EmcFileAssemble::o_PopReg() {
    *(_scriptPtr) |= 0x40;
    *(_scriptPtr) |= htobe16(atoi(_currentLine.c_str()));
}

void EmcFileAssemble::o_PopFrameMinArg() {
    *(_scriptPtr) |= 0x40;
    *(_scriptPtr) |= htobe16(atoi(_currentLine.c_str()));
}

void EmcFileAssemble::o_PopFramePluArg() {
    *(_scriptPtr) |= 0x40;
    *(_scriptPtr) |= htobe16(atoi(_currentLine.c_str()));
}

void EmcFileAssemble::o_AddSP() {
    *(_scriptPtr) |= 0x40;
    *(_scriptPtr) |= htobe16(atoi(_currentLine.c_str()));
}

void EmcFileAssemble::o_SubSP() {
    *(_scriptPtr) |= 0x40;
    *(_scriptPtr) |= htobe16(atoi(_currentLine.c_str()));
}

void EmcFileAssemble::o_Execute() {
    uint16_t opcode = 0;

    *(_scriptPtr) |= 0x40;
    opcode = scriptOpcodeFind(_currentLine, _opcodesExecute);
    *(_scriptPtr) |= htobe16(opcode);

    (this->*_opcodesExecute[opcode].function)();
}

void EmcFileAssemble::o_IfNotGoto() {
    size_t labelPos = scriptLabelGet(_currentLine);
    int line = 0;

    *(_scriptPtr) |= 0x20;
    line = labelPos;
    line |= 0x8000;
    *(_scriptPtr+1) = htobe16(line),

    _scriptPtr++;
    _scriptPos++;
}

void EmcFileAssemble::o_Negate() {
    *(_scriptPtr) |= 0x40;
    *(_scriptPtr) |= htobe16(atoi(_currentLine.c_str()));
}

void EmcFileAssemble::o_Evaluate() {
    uint16_t opcode = 0;

    *(_scriptPtr) |= 0x40;
    opcode = scriptOpcodeFind(_currentLine, _opcodesEvaluate);
    *(_scriptPtr) |= htobe16(opcode);
}

void EmcFileAssemble::o_Return() {

}

#if 0
void EmcFileAssemble::o_execute_Unit_GetDetail() {
    /* What's this for??
    static std::string	detailName;
    *_inputStream >> detailName;*/
}
#endif

}
