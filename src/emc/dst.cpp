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
#include "StdDef.h"

#include "emc/EmcFileBase.h"
#include "emc/EmcFileDisassemble.h"
#include "emc/EmcFileAssemble.h"

using namespace eastwood;

static const char *version = "1.1";

int main( int argc, char *argv[]) {
    bool result = false;
    EmcFileBase *script = NULL;
    EmcFileDisassemble *decompiler = NULL;
    EmcFileAssemble *compiler = NULL;


    std::cout << "Dune II Script Tools v" << version << std::endl << std::endl;

    if(argc < 3) {
	std::cout << "Usage:" << std::endl;
	std::cout << "d: Disassemble" << std::endl;
	std::cout << "c: Assemble" << std::endl;
	std::cout << "dst d script.emc" << std::endl;
	std::cout << "dst c script.txt" << std::endl;
	return 0;
    }

    // Disassemble Mode
    if( tolower(*argv[1]) == 'd' )
	script = decompiler = new EmcFileDisassemble( argv[2] );

    // Assemble Mode
    else if( tolower(*argv[1]) == 'c' )
	script = compiler = new EmcFileAssemble( argv[2] );

    // Do It
    result = script->execute();
    std::cout << std::endl;

    if( !result ) {
	std::cout << "Failed" << std::endl;
	std::cout << "Line Number:" << script->labelCountGet() << std::endl;
    }	else
	std::cout << "Done" << std::endl;

    if(decompiler)
	delete decompiler;
    else if(compiler)
	delete compiler;
}
