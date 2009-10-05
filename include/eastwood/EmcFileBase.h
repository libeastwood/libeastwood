#ifndef	EASTWOOD_EMC_EMCFILEBASE_H
#define	EASTWOOD_EMC_EMCFILEBASE_H

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
#include <vector>

#include "eastwood/IStream.h"
#include "eastwood/OStream.h"

namespace eastwood {

class EmcFileBase;

// Opcode Definitions
typedef void (EmcFileBase::*opcodefuncPtr)();

struct _Opcode {
    const char *description;
    opcodefuncPtr function;
};

struct labelPosition {
    uint16_t _scriptPos;
    std::string _name;
};

enum script_t {
    script_INVALID = -1,
    script_BUILD = 0,
    script_UNIT,
    script_TEAM
};

static const char emcHeader[] = { 0x46,0x4F,0x52,0x4D,0x00,0x00,0x00,0x00,0x45,0x4D,0x43,0x32,0x4F,0x52,0x44,0x52 };

class EmcFileBase {
    public:
	EmcFileBase(const std::istream &input, const std::ostream &output);
	virtual ~EmcFileBase();

	uint16_t	scriptOpcodeFind(std::string opcodeStr, const _Opcode *opcodes);	// Search the opcode table for 'Opcode' string

	inline
	size_t		labelCountGet() {
	    return _lineCount;
	}

	inline
	script_t	type() const {
	    return _scriptType;
	}

	// Virtual Functions
	virtual bool	 execute()		= 0;

    protected:
	// Opcode Prepare
	virtual void	 opcodesSetup(std::string currentLine = "");
	virtual void	 opcodesBuildingsSetup();
	virtual void	 opcodesUnitsSetup();
	virtual void	 opcodesTeamsSetup();

	// Opcodes
	virtual void	 o_Goto()		= 0;					
	virtual void	 o_SetReturn()		= 0;
	virtual void	 o_PushOp()		= 0;
	virtual void	 o_Push() 		= 0;
	virtual void	 o_PushWord()		= 0;
	virtual void	 o_PushReg()		= 0;
	virtual void	 o_PushFrameMinArg()	= 0;
	virtual void	 o_PushFramePluArg()	= 0;
	virtual void	 o_Pop()		= 0;
	virtual void	 o_PopReg()		= 0;
	virtual void	 o_PopFrameMinArg()	= 0;
	virtual void	 o_PopFramePluArg()	= 0;
	virtual void	 o_AddSP()		= 0;
	virtual void	 o_SubSP()		= 0;
	virtual void	 o_Execute()		= 0;
	virtual void	 o_IfNotGoto()		= 0;
	virtual void	 o_Negate()		= 0;
	virtual void	 o_Evaluate()		= 0;
	virtual void	 o_Return()		= 0;

	// Opcode Evaluation Modes
	virtual void	 o_evaluate_IfEither(){}
	virtual void	 o_evaluate_IfEither1(){ o_evaluate_IfEither(); }
	virtual void	 o_evaluate_Equal(){}
	virtual void	 o_evaluate_NotEqual(){}
	virtual void	 o_evaluate_CompareGreaterEqual(){}
	virtual void	 o_evaluate_CompareGreater(){}
	virtual void	 o_evaluate_CompareLessEqual(){}
	virtual void	 o_evaluate_CompareLess(){}
	virtual void	 o_evaluate_Add(){}
	virtual void	 o_evaluate_Subtract(){}
	virtual void	 o_evaluate_Multiply(){}
	virtual void	 o_evaluate_Divide(){}
	virtual void	 o_evaluate_ShiftRight(){}
	virtual void	 o_evaluate_ShiftLeft(){}
	virtual void	 o_evaluate_And(){}
	virtual void	 o_evaluate_Or(){}
	virtual void	 o_evaluate_DivideRemainder(){}
	virtual void	 o_evaluate_XOR(){}

	// Opcode Execute Functions
	// Buildings
	virtual void	 o_execute_Building_Null(){}

	// Units
	virtual void	 o_execute_Unit_Null(){}
	virtual void	 o_execute_Unit_GetDetail(){}

	// Teams
	virtual void	 o_execute_Team_Null(){}

	IStream 	&_inputStream;
	OStream		&_outputStream;

	// Opcode Functions
	const _Opcode	*_opcodes;		// Script Opcodes
	const _Opcode	*_opcodesEvaluate;	// Evaluate Command Opcodes
	const _Opcode	*_opcodesExecute;	// Execute Functions

	uint16_t	*_headerPointers;	// function pointers

	uint16_t	_pointerCount;		// Number of script "function" pointers
	size_t		_scriptSize;		// Size of script

	volatile bool	_modePreProcess;	// In Pre-Process mode?

	const char	**_objectNames;		// Pointer to current script object names
	const char	**_objectFunctions;	// Pointer to current script executable functions

	size_t		_lineCount;				
	const uint8_t	*_scriptBuffer;		// script uint8_t stream
	uint16_t	*_scriptPtr;		// Pointer in _scriptBuffer to current opcode
	uint16_t	_scriptPos;		// Line number of current opcode
	const uint8_t	*_scriptStart;		// pointer in _scriptBuffer to start of actual script
	script_t	_scriptType;		// Type of script (BUILD/TEAM/UNIT)

	std::vector<labelPosition> _scriptLabels;	// List of memory locations which can/are jumped to

	inline
	size_t scriptLabelGet(std::string label) {
	    size_t pos = scriptLabel(label);

	    if(pos == (size_t)-1)
		return -1;

	    return _scriptLabels[pos]._scriptPos;
	}

	inline
	size_t scriptLabel(std::string label) {
	    size_t pos = 0;

	    for(std::vector<labelPosition>::const_iterator labelIT = _scriptLabels.begin();
		    labelIT != _scriptLabels.end();
		    labelIT++, pos++) {

		if(label.compare((*labelIT)._name) == 0)
		    return pos;
	    }

	    // No labels here
	    return -1;
	}

	inline
	size_t scriptLabel(uint16_t position) {
	    size_t pos = 0;

	    for(std::vector<labelPosition>::const_iterator labelIT = _scriptLabels.begin();
		    labelIT != _scriptLabels.end();
		    labelIT++, pos++) {
		if((*labelIT)._scriptPos == position)
		    return pos;
	    }

	    // No labels here
	    return -1;
	}

	inline
	void scriptLabelAdd(std::string label, uint16_t position) {
	    labelPosition LP = { 0, "" };
	    size_t labelPos = scriptLabel(position),
		   labelEndPos = label.find(":");

	    if(labelEndPos == std::string::npos)
		labelEndPos = label.length();

	    if(labelPos == (size_t)-1) {
		LP._name += label.substr(0, labelEndPos);
		LP._scriptPos = position;

		_scriptLabels.push_back(LP);
	    }
	}
};

}
#endif // EASTWOOD_EMC_EMCFILEBASE_H
