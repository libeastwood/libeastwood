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
 * $Id: dst.cpp 21 2009-04-11 01:11:32Z segra $
 * 
 */

#include "stdafx.h"
#include "scriptHandler.h"
#include "scriptHandlerDecompiler.h"
#include "scriptHandlerCompiler.h"

using namespace script;

int main( int argc, char *argv[]) {
	bool			result = false;
	_scriptHandler *script = 0;

	cout << "Dune II Script Tools v" << version << endl << endl;

	if(argc < 3) {
		cout << "Usage:" << endl;
		cout << "d: Decompile" << endl;
		cout << "c: Compile" << endl;
		cout << "dst d script.emc" << endl;
		cout << "dst c script.txt" << endl;
		return 0;
	}

	// Decompile Mode
	if( tolower(*argv[1]) == 'd' )
		script = new _scriptHandlerDecompiler( argv[2] );

	// Compile Mode
	else if( tolower(*argv[1]) == 'c' )
		script = new _scriptHandlerCompiler( argv[2] );

	// Do It
	result = script->execute();
	cout << endl;

	if( !result ) {
			cout << "Failed" << endl;
			cout << "Line Number:" << script->labelCountGet() << endl;
	}	else
			cout << "Done" << endl;

	delete script;
}
