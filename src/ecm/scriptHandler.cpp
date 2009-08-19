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
 * $Id: scriptHandler.cpp 21 2009-04-11 01:11:32Z segra $
 * 
 */

#include "stdafx.h"
#include "scriptHandler.h"
#include "scriptHandlerDecompiler.h"
#include "scriptHandlerCompiler.h"


	// Constructor
	_scriptHandler::_scriptHandler( const char *fileName ) {
		_fileName = fileName;
		
		_headerPointers	 = 0;
		_pointerCount	 = 0;

		_scriptBuffer	 = 0;
		_scriptPtr		 = 0;
		_scriptPos		 = 0;

		_modePreProcess = true;
	}

	// Destructor
	_scriptHandler::~_scriptHandler() {
		
		if( _scriptBuffer )
			delete _scriptBuffer;

		if( _headerPointers )
			delete _headerPointers;
	}

	// Search the opcode table for a string opcode name
	uint16_t _scriptHandler::scriptOpcodeFind( std::string opcodeStr, const _Opcode *opcodes ) {
		const _Opcode *opcode;
		uint16_t  count =0;

		// Search opcode list
		for( opcode = opcodes; opcode->description; opcode++, count++ ) {
			if( opcodeStr.compare(opcode->description) == 0)
				return count;
		}

		// Return invalid opcode
		return 0xFFFF;
	}

	// Setup the opcode name/function table
	void _scriptHandler::opcodesSetup() {
		static const _Opcode scriptOpcodes[] = {
		    {"Goto",				&_scriptHandler::o_goto},				// 0
		    {"SetReturn",		&_scriptHandler::o_setreturn},			// 1
		    {"PushOp",			&_scriptHandler::o_pushOp},				// 2
		    {"PushWord",			&_scriptHandler::o_pushWord},			// 3
		    {"Push",				&_scriptHandler::o_push},				// 4
		    {"PushReg",			&_scriptHandler::o_pushreg},				// 5
		    {"PushFrameMinArg",	&_scriptHandler::o_pushframeMinArg},		// 6
		    {"PushFramePluArg",	&_scriptHandler::o_pushframePluArg},		// 7
		    {"Pop",				&_scriptHandler::o_popret},				// 8
		    {"PopReg",			&_scriptHandler::o_popreg},				// 9
		    {"PopFrameMinArg",	&_scriptHandler::o_popframeMinArg},		// A
		    {"PopFramePluArg",	&_scriptHandler::o_popframePluArg},		// B
		    {"AddSP",			&_scriptHandler::o_spadd},				// C
		    {"SubSP",			&_scriptHandler::o_spsub},				// D
		    {"Execute",			&_scriptHandler::o_execute},				// E
		    {"IfNotGoto",		&_scriptHandler::o_ifnotgoto},			// F
		    {"Negate",			&_scriptHandler::o_negate},				// 10
		    {"Evaluate",			&_scriptHandler::o_evaluate},			// 11
		    {"Return",			&_scriptHandler::o_return},				// 12
		};
		
		// The 'Evaluate' opcode functions
		static const _Opcode scriptOpcodesEvaluate[] = {
		    {"IfEither",					&_scriptHandler::o_evaluate_IfEither},				// 0
			{"IfEither1",				&_scriptHandler::o_evaluate_IfEither},				// 1
			{"Equal",					&_scriptHandler::o_evaluate_Equal},					// 2
			{"NotEqual",					&_scriptHandler::o_evaluate_NotEqual},				// 3
			{"CompareGreaterEqual",		&_scriptHandler::o_evaluate_CompareGreaterEqual},	// 4
			{"CompareGreater",			&_scriptHandler::o_evaluate_CompareGreater},			// 5
			{"CompareLessEqual",			&_scriptHandler::o_evaluate_CompareLessEqual},		// 6
			{"CompareLess",				&_scriptHandler::o_evaluate_CompareLess},			// 7
			{"Add",						&_scriptHandler::o_evaluate_Add},					// 8
			{"Subtract",					&_scriptHandler::o_evaluate_Subtract},				// 9
			{"Multiply",					&_scriptHandler::o_evaluate_Multiply},				// A
			{"Divide",					&_scriptHandler::o_evaluate_Divide},					// B
			{"ShiftRight",				&_scriptHandler::o_evaluate_ShiftRight},				// C
			{"ShiftLeft",				&_scriptHandler::o_evaluate_ShiftLeft},				// D
			{"And",						&_scriptHandler::o_evaluate_And},					// E
			{"Or",						&_scriptHandler::o_evaluate_Or},						// F
			{"DivideRemainder",			&_scriptHandler::o_evaluate_DivideRemainder},		// 10
			{"XOR",						&_scriptHandler::o_evaluate_XOR},					// 11
		};

		_opcodes		  = scriptOpcodes;
		_opcodesEvaluate  = scriptOpcodesEvaluate;
	}

	// The 'Building' Execute functions
	void _scriptHandler::opcodesBuildingsSetup() {
		static const _Opcode scriptOpcodesExecuteBuildings[] = {
			{"Delay",					&_scriptHandler::o_execute_Building_Null},					// 0
			{"Null",						&_scriptHandler::o_execute_Building_Null},					// 1
			{"VerifyAttached",			&_scriptHandler::o_execute_Building_Null},					// 2	
			{"Attach",					&_scriptHandler::o_execute_Building_Null},					// 3
			{"SetFrame",					&_scriptHandler::o_execute_Building_Null},					// 4
			{"Text",						&_scriptHandler::o_execute_Building_Null},					// 5
			{"Disattach",				&_scriptHandler::o_execute_Building_Null},					// 6
			{"Deploy",					&_scriptHandler::o_execute_Building_Null},					// 7
			{"GetTarget",				&_scriptHandler::o_execute_Building_Null},					// 8
			{"SetAngle",					&_scriptHandler::o_execute_Building_Null},					// 9
			{"GetAngle",					&_scriptHandler::o_execute_Building_Null},					// A
			{"ShootTarget",				&_scriptHandler::o_execute_Building_Null},					// B
			{"Null",						&_scriptHandler::o_execute_Building_Null},					// C
			{"GetFrame",					&_scriptHandler::o_execute_Building_Null},					// D
			{"PlaySfx",					&_scriptHandler::o_execute_Building_Null},					// E
			{"Reveal",					&_scriptHandler::o_execute_Building_Null},					// F
			{"Null",						&_scriptHandler::o_execute_Building_Null},					// 10
			{"Null",						&_scriptHandler::o_execute_Building_Null},					// 11
			{"Null",						&_scriptHandler::o_execute_Building_Null},					// 12
			{"Null",						&_scriptHandler::o_execute_Building_Null},					// 13
			{"Null",						&_scriptHandler::o_execute_Building_Null},					// 14
			{"RefineSpice",				&_scriptHandler::o_execute_Building_Null},					// 15
			{"Explode",					&_scriptHandler::o_execute_Building_Null},					// 16
			{"Destroy",					&_scriptHandler::o_execute_Building_Null},					// 17
			{"Null",						&_scriptHandler::o_execute_Building_Null},					// 18
		};

		_opcodesExecute = scriptOpcodesExecuteBuildings;

	}

	// The 'Units' Execute functions
	void _scriptHandler::opcodesUnitsSetup() {
		static const _Opcode scriptOpcodesExecuteUnits[] = {
			{"GetDetail",				&_scriptHandler::o_execute_Unit_GetDetail},				// 0 	
			{"sub_272E7",				&_scriptHandler::o_execute_Unit_Null},					// 1
			{"Text",   					&_scriptHandler::o_execute_Unit_Null},					// 2
			{"getObjectDistance?",		&_scriptHandler::o_execute_Unit_Null},					// 3
			{"sub_279AB",				&_scriptHandler::o_execute_Unit_Null},					// 4
			{"sub_27186",				&_scriptHandler::o_execute_Unit_Null},					// 5
			{"sub_27127",				&_scriptHandler::o_execute_Unit_Null},					// 6
			{"sub_27019",				&_scriptHandler::o_execute_Unit_Null},					// 7
			{"Attack",					&_scriptHandler::o_execute_Unit_Null},					// 8
			{"MCVDeploy",				&_scriptHandler::o_execute_Unit_Null},					// 9
			{"SidebarCmd4",				&_scriptHandler::o_execute_Unit_Null},					// A
			{"Flash",					&_scriptHandler::o_execute_Unit_Null},					// B
			{"sub_27638",				&_scriptHandler::o_execute_Unit_Null},					// C
			{"HouseCompare",				&_scriptHandler::o_execute_Unit_Null},					// D
			{"sub_26A69",				&_scriptHandler::o_execute_Unit_Null},					// E
			{"Destroy",					&_scriptHandler::o_execute_Unit_Null},					// F
			{"Delay",					&_scriptHandler::o_execute_Unit_Null},					// 10
			{"sub_1D2EE",				&_scriptHandler::o_execute_Unit_Null},					// 11
			{"sub_26AB4",				&_scriptHandler::o_execute_Unit_Null},					// 12
			{"sub_26745",				&_scriptHandler::o_execute_Unit_Null},					// 13
			{"sub_25F69",				&_scriptHandler::o_execute_Unit_Null},					// 14
			{"Null",						&_scriptHandler::o_execute_Unit_Null},					// 15
			{"sub_2677F",				&_scriptHandler::o_execute_Unit_Null},					// 16
			{"RandomNumber",				&_scriptHandler::o_execute_Unit_Null},					// 17
			{"sub_1D3D4",				&_scriptHandler::o_execute_Unit_Null},					// 18
			{"sub_27356",				&_scriptHandler::o_execute_Unit_Null},					// 19
			{"sub_26689",				&_scriptHandler::o_execute_Unit_Null},					// 1A
			{"sub_266B9",				&_scriptHandler::o_execute_Unit_Null},					// 1B
			{"sub_25F3F",				&_scriptHandler::o_execute_Unit_Null},					// 1C
			{"DamageGet",				&_scriptHandler::o_execute_Unit_Null},					// 1D
			{"Dock?",					&_scriptHandler::o_execute_Unit_Null},					// 1E
			{"EMCDataTest?",				&_scriptHandler::o_execute_Unit_Null},					// 1F
			{"CheckHarvestReturn",		&_scriptHandler::o_execute_Unit_Null},					// 20
			{"CreateSoldier",			&_scriptHandler::o_execute_Unit_Null},					// 21
			{"DeliverToBuilding",		&_scriptHandler::o_execute_Unit_Null},					// 22
			{"PlaceInUnit",				&_scriptHandler::o_execute_Unit_Null},					// 23
			{"CarryAllHoldingClear",		&_scriptHandler::o_execute_Unit_Null},					// 24
			{"BuildingFreeFind",			&_scriptHandler::o_execute_Unit_Null},					// 25
			{"PlaySFX",					&_scriptHandler::o_execute_Unit_Null},					// 26
			{"DestroyedMessage",			&_scriptHandler::o_execute_Unit_Null},					// 27
			{"MapReveal",				&_scriptHandler::o_execute_Unit_Null},					// 28
			{"MapGetTile",				&_scriptHandler::o_execute_Unit_Null},					// 29
			{"Harvest",					&_scriptHandler::o_execute_Unit_Null},					// 2A
			{"Null",						&_scriptHandler::o_execute_Unit_Null},					// 2B
			{"GetHoldingType",			&_scriptHandler::o_execute_Unit_Null},					// 2C
			{"GetType",					&_scriptHandler::o_execute_Unit_Null},					// 2D
			{"IndexGet ",				&_scriptHandler::o_execute_Unit_Null},					// 2E
			{"sub_27E8B",				&_scriptHandler::o_execute_Unit_Null},					// 2F
			{"GetMapPieceForUnit",		&_scriptHandler::o_execute_Unit_Null},					// 30
			{"sub_28001",				&_scriptHandler::o_execute_Unit_Null},					// 31
			{"TypeCount",				&_scriptHandler::o_execute_Unit_Null},					// 32
			{"sub_28090",				&_scriptHandler::o_execute_Unit_Null},					// 33
			{"Null",						&_scriptHandler::o_execute_Unit_Null},					// 34
			{"Null",						&_scriptHandler::o_execute_Unit_Null},					// 35
			{"GetNearObjectTypeIndex",   &_scriptHandler::o_execute_Unit_Null},					// 36
			{"sub_282BC",				&_scriptHandler::o_execute_Unit_Null},					// 37
			{"GetField64",				&_scriptHandler::o_execute_Unit_Null},					// 38
			{"Null",						&_scriptHandler::o_execute_Unit_Null},					// 39
			{"GetAttackObjectIndexType",	&_scriptHandler::o_execute_Unit_Null},					// 3A
			{"ObjectIsValid?",			&_scriptHandler::o_execute_Unit_Null},					// 3B
			{"DelayAnd?",				&_scriptHandler::o_execute_Unit_Null},					// 3C
			{"sub_27053",				&_scriptHandler::o_execute_Unit_Null},					// 3D
			{"ObjectDistanceCalc",		&_scriptHandler::o_execute_Unit_Null},					// 3E
			{"Null",						&_scriptHandler::o_execute_Unit_Null},					// 3F
		};

		_opcodesExecute = scriptOpcodesExecuteUnits;
	}

	// The 'Houses' Execute functions
	void _scriptHandler::opcodesHousesSetup() {
		static const _Opcode scriptOpcodesExecuteHouses[] = {
			{"Delay",					&_scriptHandler::o_execute_House_Null},		// 0 	
			{"HouseText",				&_scriptHandler::o_execute_House_Null},		// 1
			{"HouseLastUnitIndexGet",	&_scriptHandler::o_execute_House_Null},		// 2
			{"sub_24E93",				&_scriptHandler::o_execute_House_Null},		// 3
			{"sub_2503A",				&_scriptHandler::o_execute_House_Null},		// 4
			{"sub_251BA",				&_scriptHandler::o_execute_House_Null},		// 5
			{"sub_2533D",				&_scriptHandler::o_execute_House_Null},		// 6
			{"sub_253FF",				&_scriptHandler::o_execute_House_Null},		// 7
			{"sub_2563B",				&_scriptHandler::o_execute_House_Null},		// 8
			{"sub_25697",				&_scriptHandler::o_execute_House_Null},		// 9
			{"DelayAnd",					&_scriptHandler::o_execute_House_Null},		// A
			{"sub_1CFC4",				&_scriptHandler::o_execute_House_Null},		// B
			{"UnitCountGet",				&_scriptHandler::o_execute_House_Null},		// C
			{"WindtrapCountGet",			&_scriptHandler::o_execute_House_Null},		// D
			{"Null",						&_scriptHandler::o_execute_House_Null},		// E
		};

		_opcodesExecute = scriptOpcodesExecuteHouses;
	}

