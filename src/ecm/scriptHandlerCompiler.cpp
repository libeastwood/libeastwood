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
 * $Id: scriptHandlerCompiler.cpp 26 2009-05-18 04:49:54Z segra $
 * 
 */

#include "stdafx.h"
#include "scriptHandler.h"
#include "scriptHandlerCompiler.h"

	_scriptHandlerCompiler::_scriptHandlerCompiler( const char *fileName ) : _scriptHandler(fileName) {
		
		_sourceFile		= 0;

		opcodesSetup();
	}

	_scriptHandlerCompiler::~_scriptHandlerCompiler() {
		
	}


	bool _scriptHandlerCompiler::headerCreate() {
		uint16_t		*buffer = (uint16_t*) (_scriptBuffer);
		uint16_t		*bufferStart = buffer;
		const char	 header[0x10] = { 0x46,0x4F,0x52,0x4D,0x00,0x00,0x00,0x00,0x45,0x4D,0x43,0x32,0x4F,0x52,0x44,0x52 };

		// Copy the header in
		memcpy( (uint8_t*) buffer, header, 0x10 );

		buffer += 0x3;

		// Not sure what these values' means yet, so i copied them from each different script
		if( _scriptType == _scriptBUILD )
			*buffer = 0x2E04;

		if( _scriptType == _scriptUNIT )
			*buffer = 0x6A15;

		if( _scriptType == _scriptHOUSE ) 
			*buffer = 0x5A01;

		// 0x6 is WORDS, not uint8_ts like 0x10 above
		buffer += 0x6;

		// Write pointer counter
		*buffer = swapWord(_pointerCount * 2);

		buffer++;

		// Write ID script pointers
		for(size_t ptrCount = 0; ptrCount < _pointerCount; ptrCount++) {
			*buffer = swapWord(_headerPointers[ptrCount]);
			buffer++;
		}

		// Write DATA std::string
		*buffer = 'AD';
		buffer++;
		*buffer = 'AT';
		buffer++;

		// Not sure if this uint16_t is actually part of a DWORD, or 2 useless uint8_ts?
		buffer++;

		// Write total script size
		*buffer = swapWord(_scriptSize);

		return true;
	}

	bool _scriptHandlerCompiler::scriptSave( ) {
		std::ofstream		 _targetFile;
		std::string file;

		file.append( _fileName );
		file.append( ".EMC" );

		_targetFile.open(file.c_str(), std::ios::binary | std::ios::out);
		
		if(_targetFile.is_open() == false)
			return false;

		// Set the header/pointers in the buffer
		headerCreate();

		// Write the header, the pointers and the script
		_targetFile.write( (char*)_scriptBuffer, _scriptSize + (_pointerCount*2) + 0x1C);
		_targetFile.close();

		return true;
	}

	int _scriptHandlerCompiler::scriptSectionCheck( ) {
		int		count = 0;
		size_t	posStart = _currentLine.find("[") + 1;
		size_t	posEnd;

		// Not a section header
		if( posStart == std::string::npos )
			return -2;

		posEnd = _currentLine.find("]", posStart);;

		// Read the section name
		std::string name = _currentLine.substr( posStart, posEnd - posStart );

		// Check for the section name
		for( const char **namePtr = _objectNames; *namePtr; *namePtr++, count++ ) {
			if( name.compare( *namePtr ) == 0 )
				return count;
		}
		
		// General Section
		return -1;
	}

	bool _scriptHandlerCompiler::execute() {

		std::cout << "Preprocessing " << _fileName << std::endl;

		// Run the script compiler in pre process mode (find all jump locations)
		if(scriptCompile() == false)
			return false;

		// Cleanup script buffer
		if( _scriptBuffer )
			delete _scriptBuffer;

		// Cleanup header pointers
		if( _headerPointers )
			delete _headerPointers;

		// Disable PreProcess mode
		_modePreProcess = false;

		std::cout << "Compiling....." << std::endl;
		// Properly compile the script
		if(scriptCompile() == false )
			return false;

		// Save the _scriptBuffer to disk
		return scriptSave();
	}

	// -1 Decompiles the script at _scriptStart
	bool _scriptHandlerCompiler::scriptCompile( ) {
		char	nextChar;
		int		objectID = 0;

		_lineCount = 0;

		// Reset the script position and the size
		_scriptSize		= 0;
		_scriptPos		= 0;

		// Cleanup previous file operation
		if(_sourceFile)
			delete _sourceFile;

		// Open source file 
		_sourceFile = new std::ifstream();
		_sourceFile->open( _fileName, std::ios::in );

		if( _sourceFile->is_open() == false )
			return false;

		// Read file type line
		*_sourceFile >> _currentLine;
		_lineCount++;

		// House Script
		if(_currentLine == "[House]") {
			_scriptType		 = _scriptHOUSE;
			_pointerCount = 0x06;

			_objectNames	 = nameHouses;
			opcodesHousesSetup();
		}

		// Building Script
		if(_currentLine == "[Build]") {
			_scriptType		 = _scriptBUILD;
			_pointerCount = 0x13;
			_objectNames	 = nameStructures;
			opcodesBuildingsSetup();
		}

		// Unit Script
		if(_currentLine == "[Unit]") {
			_scriptType		 = _scriptUNIT;
			_pointerCount = 0x1B;
			_objectNames	 = nameUnits;
			opcodesUnitsSetup();
		}

		// Prepare memory for scriptBuffer and headerPointers
		_headerPointers = new uint16_t[_pointerCount];
		_scriptBuffer	= new uint8_t[0x100000];			// should be big enough :p

		// Set the script pointer to the script starting position (in buffer)
		_scriptPtr		= (uint16_t*) (_scriptBuffer + (_pointerCount*2) + 0x1C);

		// Clear memory
		memset( (void*) _headerPointers, 0, _pointerCount * 2);
		memset( (void*) _scriptBuffer, 0, 0x100000 );

		// Loop until end of source file
		while( !_sourceFile->eof() ) {
			
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
				_opcode = scriptOpcodeFind( _currentLine, _opcodes );
				
				// Check for label
				if( _modePreProcess &&  (_currentLine.find( ":" ) != std::string::npos) ) {

					// Add the label to the labels vector
					scriptLabelAdd( _currentLine, _scriptPos );
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
				while( (nextChar = _sourceFile->get()) == 0x20 ) {

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
			(this->*_opcodes[ _opcode ].function)( );

			// Debugging Use
			//if( ((uint8_t*) _scriptPtr) - _scriptBuffer > 0x89B)
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
