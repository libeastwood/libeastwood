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

#include "EmcFileBase.h"
#include "EmcFileAssemble.h"
#include "EmcFileInternal.h"

EmcFileAssemble::EmcFileAssemble(const char *fileName) : EmcFileBase(fileName) {
    _sourceFile = NULL;
}

EmcFileAssemble::~EmcFileAssemble() {
    if(_sourceFile) {
    	_sourceFile->close();
    	delete _sourceFile;
    }
}


bool EmcFileAssemble::headerCreate() {
    uint16_t *buffer = (uint16_t*) (_scriptBuffer);

    // Copy the header in
    memcpy((uint8_t*) buffer, emcHeader, sizeof(emcHeader));

    buffer += 0x3;

    // Not sure what these values' means yet, so i copied them from each different script
    if(_scriptType == _scriptBUILD)
	*buffer = 0x2E04;

    if(_scriptType == _scriptUNIT)
	*buffer = 0x6A15;

    if(_scriptType == _scriptHOUSE) 
	*buffer = 0x5A01;

    // 0x6 is WORDS, not uint8_ts like 0x10 above
    buffer += 0x6;

    // Write pointer counter
    *buffer = htobe16(_pointerCount * 2);

    buffer++;

    // Write ID script pointers
    for(size_t ptrCount = 0; ptrCount < _pointerCount; ptrCount++) {
	*buffer = htobe16(_headerPointers[ptrCount]);
	buffer++;
    }

    // Write DATA std::string
    *buffer = ('A'<<8) + 'D';
    buffer++;
    *buffer = ('A'<<8) + 'T';
    buffer++;

    // Not sure if this uint16_t is actually part of a DWORD, or 2 useless uint8_ts?
    buffer++;

    // Write total script size
    *buffer = htobe16(_scriptSize);

    return true;
}

bool EmcFileAssemble::scriptSave() {
    std::ofstream _targetFile;
    std::string file;

    file.append(_fileName);
    file.append(".EMC");

    _targetFile.open(file.c_str(), std::ios::binary | std::ios::out);

    if(_targetFile.is_open() == false)
	return false;

    // Set the header/pointers in the buffer
    headerCreate();

    // Write the header, the pointers and the script
    _targetFile.write((char*)_scriptBuffer, _scriptSize + (_pointerCount*2) + 0x1C);
    _targetFile.close();

    return true;
}

int EmcFileAssemble::scriptSectionCheck() {
    int count = 0;
    size_t posStart = _currentLine.find("[") + 1;
    size_t posEnd;

    // Not a section header
    if(posStart == std::string::npos)
	return -2;

    posEnd = _currentLine.find("]", posStart);;

    // Read the section name
    std::string name = _currentLine.substr(posStart, posEnd - posStart);

    // Check for the section name
    for(const char **namePtr = _objectNames; *namePtr; *namePtr++, count++) {
	if(name.compare(*namePtr) == 0)
	    return count;
    }

    // General Section
    return -1;
}

bool EmcFileAssemble::execute() {

    std::cout << "Preprocessing " << _fileName << std::endl;

    // Run the script assembler in pre process mode (find all jump locations)
    if(scriptAssemble() == false)
	return false;

    // Cleanup script buffer
    if(_scriptBuffer)
	delete [] _scriptBuffer;

    // Cleanup header pointers
    if(_headerPointers)
	delete [] _headerPointers;

    // Disable PreProcess mode
    _modePreProcess = false;

    std::cout << "Compiling....." << std::endl;
    // Properly assemble the script
    if(scriptAssemble() == false)
	return false;

    // Save the _scriptBuffer to disk
    return scriptSave();
}

// -1 Disassembles the script at _scriptStart
bool EmcFileAssemble::scriptAssemble() {
    char nextChar;
    int	objectID = 0;

    _lineCount = 0;

    // Reset the script position and the size
    _scriptSize	= 0;
    _scriptPos	= 0;

    // Cleanup previous file operation
    if(_sourceFile)
	delete _sourceFile;

    // Open source file 
    _sourceFile = new std::ifstream();
    _sourceFile->open(_fileName, std::ios::in);

    if(_sourceFile->is_open() == false)
	return false;

    // Read file type line
    *_sourceFile >> _currentLine;
    _lineCount++;

    opcodesSetup(_currentLine);

    // Prepare memory for scriptBuffer and headerPointers
    _headerPointers = new uint16_t[_pointerCount];
    _scriptBuffer = new uint8_t[0x100000];			// should be big enough :p

    // Set the script pointer to the script starting position (in buffer)
    _scriptPtr = (uint16_t*) (_scriptBuffer + (_pointerCount*2) + 0x1C);

    // Clear memory
    memset((void*) _headerPointers, 0, _pointerCount * 2);
    memset((void*) _scriptBuffer, 0, 0x100000);

    // Loop until end of source file
    while(!_sourceFile->eof()) {

	// Read next line from sourcecode file
	*_sourceFile >> _currentLine;

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
	    if(_sourceFile->eof() == true)
		break;

	    // Check if the opcode is valid
	    if(_opcode == 0xFFFF) {
		*_sourceFile >> _currentLine;
		continue;
	    }

	    // Skip the spaces and check the next character
	    while((nextChar = _sourceFile->get()) == 0x20) {

	    }

	    // Move the buffer back
	    _sourceFile->seekg(-1, std::ios::cur);

	    // Is it end of line? or is it a parameter?
	    if(nextChar != '\n')
		*_sourceFile >> _currentLine;

	}

	if(_opcode == 0xFFFF)
	    break;

	_lineCount++;

	// Write uint8_tcode
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

    if(!_modePreProcess && labelPos == (size_t)-1)
	*((uint8_t*) bb) = 01;

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

    *(_scriptPtr+1) = htobe16(line);

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
    *_sourceFile >> detailName;*/
}
#endif

