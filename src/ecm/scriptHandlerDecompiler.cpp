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
 * $Id: scriptHandlerDecompiler.cpp 25 2009-04-28 09:58:25Z segra $
 * 
 */

#include "stdafx.h"
#include "scriptHandler.h"
#include "scriptHandlerDecompiler.h"

namespace script {

	_scriptHandlerDecompiler::_scriptHandlerDecompiler( const char *fileName ) : _scriptHandler(fileName) {
		std::string	sourceFilename = std::string(_fileName), targetFilename;
		size_t		posPeriod = sourceFilename.find(".");

		// Prepare output filename
		if(posPeriod == std::string::npos)
			posPeriod = sourceFilename.length() - 1;

		targetFilename = sourceFilename.substr( 0, posPeriod );
		targetFilename.append(".txt");

		_scriptLastPush = 0;
		_opcodesExecute = 0;

		opcodesSetup();

		// Open target file
		_destinationFile.open( targetFilename.c_str(), std::ios::out );
	}

	_scriptHandlerDecompiler::~_scriptHandlerDecompiler() {
		_destinationFile.close();
	}

	bool _scriptHandlerDecompiler::scriptLoad() {
		std::ifstream		 fileScript;
		size_t			 scriptSize;


		fileScript.open( _fileName, std::ios::in | std::ios::binary );

		if( fileScript.is_open() == false )
			return false;

		// Read File Size
		fileScript.seekg( 0, std::ios::end );
		scriptSize = fileScript.tellg();
		fileScript.seekg( std::ios::beg );

		// Load file into _scriptBuffer
		_scriptBuffer = new uint8_t[ scriptSize ];
		if( fileScript.read( (char*) _scriptBuffer, scriptSize ) == false)
			return false;
		
		fileScript.close();

		return true;
	}

	bool _scriptHandlerDecompiler::headerRead() {
		uint16_t		*buffer = (uint16_t*) (_scriptBuffer + 0x12);
		size_t		 ptrCount;

		// Number of script functions
		_pointerCount = readWord( buffer ) / 2;
		buffer++;

		_headerPointers = new uint16_t[_pointerCount];

		for(ptrCount = 0; ptrCount < _pointerCount; ptrCount++) {
			_headerPointers[ptrCount] = readWord( buffer );
			buffer++;
		}

		// Skip 'DATA' tag
		buffer += 2;	// 2 Words

		// Not sure if this uint16_t is actually part of a DWORD, or 2 useless uint8_ts?
		buffer++;

		// Size of the upcoming script data
		_scriptSize = readWord( buffer );
		buffer++;

		// Start of script data
		_scriptStart = (uint8_t*) buffer;

		if(_pointerCount == 0x06) {
			_destinationFile << "[House]" << std::endl;
			_scriptType		 = _scriptHOUSE;
			_objectNames	 = nameHouses;
			opcodesHousesSetup();
		}

		if(_pointerCount == 0x13) {
			_destinationFile << "[Build]" << std::endl;
			_scriptType		 = _scriptBUILD;
			_objectNames	 = nameStructures;
			opcodesBuildingsSetup();
		}

		if(_pointerCount == 0x1B) {
			_destinationFile << "[Unit]" << std::endl;
			_scriptType		 = _scriptUNIT;
			_objectNames	 = nameUnits;
			opcodesUnitsSetup();
		}

		return true;
	}

	bool _scriptHandlerDecompiler::scriptNextStart( ) {
		bool found = false;

		// Loop through each pointer and write the section names out
		for( int count = 0; count < _pointerCount; count++ ) {

			// In TEAM.EMC for example, two objects use the same script
			if( _scriptPos == (uint16_t) _headerPointers[count]) {
				if(found==false)
					_destinationFile << std::endl;

				// Write the section name in square brackets
				_destinationFile << "[" << _objectNames[count] << "]" << std::endl;
				found = true;
			}
		}

		return found;
	}

	bool _scriptHandlerDecompiler::execute() {
		
		std::cout << "Preprocessing " << _fileName << std::endl;

		// Load the script into a _scriptBuffer, then read the header information
		scriptLoad();
		headerRead();

		// Map out all labels
		_modePreProcess = true;
		if(scriptDecompile() == false)
			return false;

		// Decompile the script 
		_modePreProcess = false;
		
		_destinationFile << "[General]" << std::endl;

		std::cout << "Decompiling....." << std::endl;
		return scriptDecompile();
	}

	bool _scriptHandlerDecompiler::scriptDecompile( ) {

		_lineCount		= 0;
		_opcodeCurrent  = 0;
		_scriptData		= _scriptDataNext = 0;
		_scriptPtr		= 0;
		_scriptPtrEnd	= 0;
		_scriptPos		= 0;
		_stackCount		= 0xF;

		_scriptPos		= 0;
		_scriptPtr		= (uint16_t*)  _scriptStart;
		_scriptPtrEnd	= (uint16_t*) (_scriptStart + _scriptSize);

		while( _scriptPtr <  _scriptPtrEnd ) {

			if( !_modePreProcess ) {
				scriptNextStart();

				// Check if label location, print label if so
				if( scriptLabel( _scriptPos ) > -1)
					_destinationFile << "l" << _scriptPos << ":" << std::endl;
			}

			_scriptDataNext = 0;
			_scriptData		= readWord( _scriptPtr );
			_scriptPtr++;
			_scriptPos++;

			// Keep the opcode
			_opcodeCurrent = _scriptData >> 8;
			_opcodeCurrent &= 0x1F;
			
			if( _scriptData & 0x8000 ) {
				// Opcode uses 13 bits
				_opcodeCurrent = 0;
				_scriptData &= 0x7FFF;
			} else
				// Opcode only requires 1 uint8_t
				if( _scriptData & 0x4000 ) {
					_scriptData &= 0xFF;
			} else 	
				// Opcode uses the next WORD, grab it
				if( _scriptData & 0x2000 ) {
					_scriptDataNext = readWord( _scriptPtr );
					_scriptPtr++;
					_scriptPos++;
				}
			

			// Print opcode
			if( !_modePreProcess )
				_destinationFile << std::setw(20) << std::left << _opcodes[ _opcodeCurrent ].description;

			// Excute opcode
			(this->*_opcodes[ _opcodeCurrent ].function)( );

			//_destinationFile  << setw(20) << " ";
			//_destinationFile  << hex << uppercase << "S: 0x" << _stackCount << std::endl;
			if( !_modePreProcess )
				_destinationFile << std::endl;

			_lineCount++;
		}

		return true;
	}

}
