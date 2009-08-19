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
		string	sourceFilename = string(_fileName), targetFilename;
		int		posPeriod = sourceFilename.find(".");

		// Prepare output filename
		if(posPeriod == string::npos)
			posPeriod = sourceFilename.length() - 1;

		targetFilename = sourceFilename.substr( 0, posPeriod );
		targetFilename.append(".txt");

		_scriptLastPush = 0;
		_opcodesExecute = 0;

		opcodesSetup();

		// Open target file
		_destinationFile.open( targetFilename.c_str(), ios::out );
	}

	_scriptHandlerDecompiler::~_scriptHandlerDecompiler() {
		_destinationFile.close();
	}

	bool _scriptHandlerDecompiler::scriptLoad() {
		ifstream		 fileScript;
		size_t			 scriptSize;


		fileScript.open( _fileName, ios::in | ios::binary );

		if( fileScript.is_open() == false )
			return false;

		// Read File Size
		fileScript.seekg( 0, ios::end );
		scriptSize = fileScript.tellg();
		fileScript.seekg( ios::beg );

		// Load file into _scriptBuffer
		_scriptBuffer = new byte[ scriptSize ];
		if( fileScript.read( (char*) _scriptBuffer, scriptSize ) == false)
			return false;
		
		fileScript.close();

		return true;
	}

	bool _scriptHandlerDecompiler::headerRead() {
		word		*buffer = (word*) (_scriptBuffer + 0x12);
		size_t		 ptrCount;

		// Number of script functions
		_pointerCount = readWord( buffer ) / 2;
		buffer++;

		_headerPointers = new word[_pointerCount];

		for(ptrCount = 0; ptrCount < _pointerCount; ptrCount++) {
			_headerPointers[ptrCount] = readWord( buffer );
			buffer++;
		}

		// Skip 'DATA' tag
		buffer += 2;	// 2 Words

		// Not sure if this word is actually part of a DWORD, or 2 useless bytes?
		buffer++;

		// Size of the upcoming script data
		_scriptSize = readWord( buffer );
		buffer++;

		// Start of script data
		_scriptStart = (byte*) buffer;

		if(_pointerCount == 0x06) {
			_destinationFile << "[House]" << endl;
			_scriptType		 = _scriptHOUSE;
			_objectNames	 = nameHouses;
			opcodesHousesSetup();
		}

		if(_pointerCount == 0x13) {
			_destinationFile << "[Build]" << endl;
			_scriptType		 = _scriptBUILD;
			_objectNames	 = nameStructures;
			opcodesBuildingsSetup();
		}

		if(_pointerCount == 0x1B) {
			_destinationFile << "[Unit]" << endl;
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
			if( _scriptPos == (word) _headerPointers[count]) {
				if(found==false)
					_destinationFile << endl;

				// Write the section name in square brackets
				_destinationFile << "[" << _objectNames[count] << "]" << endl;
				found = true;
			}
		}

		return found;
	}

	bool _scriptHandlerDecompiler::execute() {
		
		cout << "Preprocessing " << _fileName << endl;

		// Load the script into a _scriptBuffer, then read the header information
		scriptLoad();
		headerRead();

		// Map out all labels
		_modePreProcess = true;
		if(scriptDecompile() == false)
			return false;

		// Decompile the script 
		_modePreProcess = false;
		
		_destinationFile << "[General]" << endl;

		cout << "Decompiling....." << endl;
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
		_scriptPtr		= (word*)  _scriptStart;
		_scriptPtrEnd	= (word*) (_scriptStart + _scriptSize);

		while( _scriptPtr <  _scriptPtrEnd ) {

			if( !_modePreProcess ) {
				scriptNextStart();

				// Check if label location, print label if so
				if( scriptLabel( _scriptPos ) > -1)
					_destinationFile << "l" << _scriptPos << ":" << endl;
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
				// Opcode only requires 1 byte
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
				_destinationFile << setw(20) << left << _opcodes[ _opcodeCurrent ].description;

			// Excute opcode
			(this->*_opcodes[ _opcodeCurrent ].function)( );

			//_destinationFile  << setw(20) << " ";
			//_destinationFile  << hex << uppercase << "S: 0x" << _stackCount << endl;
			if( !_modePreProcess )
				_destinationFile << endl;

			_lineCount++;
		}

		return true;
	}

}
