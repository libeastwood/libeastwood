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

#include "eastwood/EmcFileBase.h"

namespace eastwood {

class EmcFileAssemble : public EmcFileBase {
    public:
	EmcFileAssemble(std::istream &input, std::ostream &output);
	virtual ~EmcFileAssemble();

	bool	execute();			// Assemble a script

    private:
	std::string	_currentLine,
			_opcodeCurrent,
			_data;
	uint16_t	_opcode;

	bool		headerCreate();		// Insert the header
	bool		scriptAssemble( );	// Assemble to the byte code
	bool		scriptSave();		// Save the buffer to disk
	int 		scriptSectionCheck();	// Check for section name in _currentLine

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

};

}
#endif // EASTWOOD_EMC_EMCFILECOMPILE_H
