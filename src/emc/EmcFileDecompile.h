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
 */

#include <fstream>

class EmcFileDecompile : public EmcFileBase {
    public:
	EmcFileDecompile(const char *fileName);
	~EmcFileDecompile();

	bool	execute();		// Decompile a script

    protected:
	void	o_goto();					
	void	o_setreturn();
	void	o_pushOp();
	void	o_push();
	void	o_pushWord();
	void	o_pushreg();
	void	o_pushframeMinArg();
	void	o_pushframePluArg();
	void	o_popret();
	void	o_popreg();
	void	o_popframeMinArg();
	void	o_popframePluArg();
	void	o_spadd();
	void	o_spsub();
	void	o_execute();
	void	o_ifnotgoto();
	void	o_negate();
	void	o_evaluate();
	void	o_return();

	void	o_execute_Unit_GetDetail();

    private:
	std::ofstream	_destinationFile;
	uint16_t	_scriptLastPush;		// Last push operation value

	// Runtime Data
	uint8_t		_opcodeCurrent;			// Current Opcode
	size_t		_stackCount;			// Stack Count
	uint16_t	_scriptData,
			_scriptDataNext;	 
	uint16_t	*_scriptPtrEnd;			// address of beginning and end of script data

	bool		headerRead();			// Read the header

	bool		scriptDecompile();		// Decompile the script
	bool		scriptNextStart();		// Are we at the start of a section?
	bool		scriptLoad();			 

	inline void	dataPrint(uint16_t data) {	// Print a uint16_t to screen in decimal
	    if(!_modePreProcess)
		_destinationFile << std::dec << std::uppercase  << data;
	}

};

