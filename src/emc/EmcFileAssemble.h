#ifndef	EASTWOOD_EMC_EMCFILECOMPILE_H
#define	EASTWOOD_EMC_EMCFILECOMPILE_H

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

#include <string>
#include "../StdDef.h"


class EmcFileAssemble : public EmcFileBase {
    public:
	EmcFileAssemble( const char *fileName );
	~EmcFileAssemble();

	bool	execute();			// Assemble a script

    private:
	std::ifstream	*_sourceFile;		// Source-Code file	

	std::string	_currentLine,
			_opcodeCurrent,
			_data;
	uint16_t	_opcode;

	bool		headerCreate();		// Insert the header
	bool		scriptAssemble( );	// Assemble to the byte code
	bool		scriptSave();		// Save the buffer to disk
	int 		scriptSectionCheck();	// Check for section name in _currentLine

	void	o_Goto();
	void	o_SetReturn();
	void	o_PushOp();
	void	o_Push();
	void	o_PushWord();
	void	o_PushReg();
	void	o_PushFrameMinArg();
	void	o_PushFramePluArg();
	void	o_Pop();
	void	o_PopReg();
	void	o_PopFrameMinArg();
	void	o_PopFramePluArg();
	void	o_AddSP();
	void	o_SubSP();
	void	o_Execute();
	void	o_IfNotGoto();
	void	o_Negate();
	void	o_Evaluate();
	void	o_Return();

};

#endif // EASTWOOD_EMC_EMCFILECOMPILE_H
