#ifndef	EASTWOOD_EMC_EMCFILEDECOMPILE_H
#define	EASTWOOD_EMC_EMCFILEDECOMPILE_H

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

namespace eastwood {

class EmcFileDisassemble : public EmcFileBase {
    public:
	EmcFileDisassemble(std::istream &input, std::ostream &output);
	virtual ~EmcFileDisassemble();

	bool	execute();		// Disassemble a script

    protected:
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

	void	o_execute_Unit_GetDetail();

    private:
	uint16_t	_scriptLastPush;		// Last push operation value

	// Runtime Data
	uint8_t		_opcodeCurrent;			// Current Opcode
	size_t		_stackCount;			// Stack Count
	uint16_t	_scriptData,
			_scriptDataNext;	 
	uint16_t	*_scriptPtrEnd;			// address of beginning and end of script data

	bool		headerRead();			// Read the header

	bool		scriptDisassemble();		// Disassemble the script
	bool		scriptNextStart();		// Are we at the start of a section?
	bool		scriptLoad();			 

	inline void	dataPrint(uint16_t data) {	// Print a uint16_t to screen in decimal
	    if(!_modePreProcess)
		_outputStream << std::dec << std::uppercase  << data;
	}

};

}
#endif // EASTWOOD_EMC_EMCFILEDECOMPILE_H
