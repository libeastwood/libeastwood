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


_scriptHandler::_scriptHandler(const char *fileName) {
    _fileName		= fileName;

    _headerPointers	= 0;
    _pointerCount	= 0;

    _scriptBuffer	= 0;
    _scriptPtr		= 0;
    _scriptPos		= 0;

    _modePreProcess	= true;
}

// Destructor
_scriptHandler::~_scriptHandler() {

    if(_scriptBuffer)
	delete _scriptBuffer;

    if(_headerPointers)
	delete _headerPointers;
}

// Search the opcode table for a string opcode name
uint16_t _scriptHandler::scriptOpcodeFind(std::string opcodeStr, const _Opcode *opcodes) {
    const _Opcode *opcode;
    uint16_t  count =0;

    // Search opcode list
    for(opcode = opcodes; opcode->description; opcode++, count++) {
	if(opcodeStr.compare(opcode->description) == 0)
	    return count;
    }

    // Return invalid opcode
    return 0xFFFF;
}

// Setup the opcode name/function table
void _scriptHandler::opcodesSetup() {
    static const _Opcode scriptOpcodes[] = {
	{"Goto",		&_scriptHandler::o_goto},
	{"SetReturn",		&_scriptHandler::o_setreturn},
	{"PushOp",		&_scriptHandler::o_pushOp},
	{"PushWord",		&_scriptHandler::o_pushWord},
	{"Push",		&_scriptHandler::o_push},
	{"PushReg",		&_scriptHandler::o_pushreg},
	{"PushFrameMinArg",	&_scriptHandler::o_pushframeMinArg},
	{"PushFramePluArg",	&_scriptHandler::o_pushframePluArg},
	{"Pop",			&_scriptHandler::o_popret},
	{"PopReg",		&_scriptHandler::o_popreg},
	{"PopFrameMinArg",	&_scriptHandler::o_popframeMinArg},
	{"PopFramePluArg",	&_scriptHandler::o_popframePluArg},
	{"AddSP",		&_scriptHandler::o_spadd},
	{"SubSP",		&_scriptHandler::o_spsub},
	{"Execute",		&_scriptHandler::o_execute},
	{"IfNotGoto",		&_scriptHandler::o_ifnotgoto},
	{"Negate",		&_scriptHandler::o_negate},
	{"Evaluate",		&_scriptHandler::o_evaluate},
	{"Return",		&_scriptHandler::o_return},
    };

    // The 'Evaluate' opcode functions
    static const _Opcode scriptOpcodesEvaluate[] = {
	{"IfEither",		&_scriptHandler::o_evaluate_IfEither},
	{"IfEither1",		&_scriptHandler::o_evaluate_IfEither},
	{"Equal",		&_scriptHandler::o_evaluate_Equal},
	{"NotEqual",		&_scriptHandler::o_evaluate_NotEqual},
	{"CompareGreaterEqual",	&_scriptHandler::o_evaluate_CompareGreaterEqual},
	{"CompareGreater",	&_scriptHandler::o_evaluate_CompareGreater},
	{"CompareLessEqual",	&_scriptHandler::o_evaluate_CompareLessEqual},
	{"CompareLess",		&_scriptHandler::o_evaluate_CompareLess},
	{"Add",			&_scriptHandler::o_evaluate_Add},
	{"Subtract",		&_scriptHandler::o_evaluate_Subtract},
	{"Multiply",		&_scriptHandler::o_evaluate_Multiply},
	{"Divide",		&_scriptHandler::o_evaluate_Divide},
	{"ShiftRight",		&_scriptHandler::o_evaluate_ShiftRight},
	{"ShiftLeft",		&_scriptHandler::o_evaluate_ShiftLeft},
	{"And",			&_scriptHandler::o_evaluate_And},
	{"Or",			&_scriptHandler::o_evaluate_Or},
	{"DivideRemainder",	&_scriptHandler::o_evaluate_DivideRemainder},
	{"XOR",			&_scriptHandler::o_evaluate_XOR},
    };

    _opcodes		  = scriptOpcodes;
    _opcodesEvaluate  = scriptOpcodesEvaluate;
}

// The 'Building' Execute functions
void _scriptHandler::opcodesBuildingsSetup() {
    static const _Opcode scriptOpcodesExecuteBuildings[] = {
	{"Delay",		&_scriptHandler::o_execute_Building_Null},
	{"Null",		&_scriptHandler::o_execute_Building_Null},
	{"VerifyAttached",	&_scriptHandler::o_execute_Building_Null},	
	{"Attach",		&_scriptHandler::o_execute_Building_Null},
	{"SetFrame",		&_scriptHandler::o_execute_Building_Null},
	{"Text",		&_scriptHandler::o_execute_Building_Null},
	{"Disattach",		&_scriptHandler::o_execute_Building_Null},
	{"Deploy",		&_scriptHandler::o_execute_Building_Null},
	{"GetTarget",		&_scriptHandler::o_execute_Building_Null},
	{"SetAngle",		&_scriptHandler::o_execute_Building_Null},
	{"GetAngle",		&_scriptHandler::o_execute_Building_Null},
	{"ShootTarget",		&_scriptHandler::o_execute_Building_Null},
	{"Null",		&_scriptHandler::o_execute_Building_Null},
	{"GetFrame",		&_scriptHandler::o_execute_Building_Null},
	{"PlaySfx",		&_scriptHandler::o_execute_Building_Null},
	{"Reveal",		&_scriptHandler::o_execute_Building_Null},
	{"Null",		&_scriptHandler::o_execute_Building_Null},
	{"Null",		&_scriptHandler::o_execute_Building_Null},
	{"Null",		&_scriptHandler::o_execute_Building_Null},
	{"Null",		&_scriptHandler::o_execute_Building_Null},
	{"Null",		&_scriptHandler::o_execute_Building_Null},
	{"RefineSpice",		&_scriptHandler::o_execute_Building_Null},
	{"Explode",		&_scriptHandler::o_execute_Building_Null},
	{"Destroy",		&_scriptHandler::o_execute_Building_Null},
	{"Null",		&_scriptHandler::o_execute_Building_Null},
    };

    _opcodesExecute = scriptOpcodesExecuteBuildings;

}

// The 'Units' Execute functions
void _scriptHandler::opcodesUnitsSetup() {
    static const _Opcode scriptOpcodesExecuteUnits[] = {
	{"GetDetail",			&_scriptHandler::o_execute_Unit_GetDetail}, 	
	{"sub_272E7",			&_scriptHandler::o_execute_Unit_Null},
	{"Text",   			&_scriptHandler::o_execute_Unit_Null},
	{"getObjectDistance?",		&_scriptHandler::o_execute_Unit_Null},
	{"sub_279AB",			&_scriptHandler::o_execute_Unit_Null},
	{"sub_27186",			&_scriptHandler::o_execute_Unit_Null},
	{"sub_27127",			&_scriptHandler::o_execute_Unit_Null},
	{"sub_27019",			&_scriptHandler::o_execute_Unit_Null},
	{"Attack",			&_scriptHandler::o_execute_Unit_Null},
	{"MCVDeploy",			&_scriptHandler::o_execute_Unit_Null},
	{"SidebarCmd4",			&_scriptHandler::o_execute_Unit_Null},
	{"Flash",			&_scriptHandler::o_execute_Unit_Null},
	{"sub_27638",			&_scriptHandler::o_execute_Unit_Null},
	{"HouseCompare",		&_scriptHandler::o_execute_Unit_Null},
	{"sub_26A69",			&_scriptHandler::o_execute_Unit_Null},
	{"Destroy",			&_scriptHandler::o_execute_Unit_Null},
	{"Delay",			&_scriptHandler::o_execute_Unit_Null},
	{"sub_1D2EE",			&_scriptHandler::o_execute_Unit_Null},
	{"sub_26AB4",			&_scriptHandler::o_execute_Unit_Null},
	{"sub_26745",			&_scriptHandler::o_execute_Unit_Null},
	{"sub_25F69",			&_scriptHandler::o_execute_Unit_Null},
	{"Null",			&_scriptHandler::o_execute_Unit_Null},
	{"sub_2677F",			&_scriptHandler::o_execute_Unit_Null},
	{"RandomNumber",		&_scriptHandler::o_execute_Unit_Null},
	{"sub_1D3D4",			&_scriptHandler::o_execute_Unit_Null},
	{"sub_27356",			&_scriptHandler::o_execute_Unit_Null},
	{"sub_26689",			&_scriptHandler::o_execute_Unit_Null},
	{"sub_266B9",			&_scriptHandler::o_execute_Unit_Null},
	{"sub_25F3F",			&_scriptHandler::o_execute_Unit_Null},
	{"DamageGet",			&_scriptHandler::o_execute_Unit_Null},
	{"Dock?",			&_scriptHandler::o_execute_Unit_Null},
	{"EMCDataTest?",		&_scriptHandler::o_execute_Unit_Null},
	{"CheckHarvestReturn",		&_scriptHandler::o_execute_Unit_Null},
	{"CreateSoldier",		&_scriptHandler::o_execute_Unit_Null},
	{"DeliverToBuilding",		&_scriptHandler::o_execute_Unit_Null},
	{"PlaceInUnit",			&_scriptHandler::o_execute_Unit_Null},
	{"CarryAllHoldingClear",	&_scriptHandler::o_execute_Unit_Null},
	{"BuildingFreeFind",		&_scriptHandler::o_execute_Unit_Null},
	{"PlaySFX",			&_scriptHandler::o_execute_Unit_Null},
	{"DestroyedMessage",		&_scriptHandler::o_execute_Unit_Null},
	{"MapReveal",			&_scriptHandler::o_execute_Unit_Null},
	{"MapGetTile",			&_scriptHandler::o_execute_Unit_Null},
	{"Harvest",			&_scriptHandler::o_execute_Unit_Null},
	{"Null",			&_scriptHandler::o_execute_Unit_Null},
	{"GetHoldingType",		&_scriptHandler::o_execute_Unit_Null},
	{"GetType",			&_scriptHandler::o_execute_Unit_Null},
	{"IndexGet ",			&_scriptHandler::o_execute_Unit_Null},
	{"sub_27E8B",			&_scriptHandler::o_execute_Unit_Null},
	{"GetMapPieceForUnit",		&_scriptHandler::o_execute_Unit_Null},
	{"sub_28001",			&_scriptHandler::o_execute_Unit_Null},
	{"TypeCount",			&_scriptHandler::o_execute_Unit_Null},
	{"sub_28090",			&_scriptHandler::o_execute_Unit_Null},
	{"Null",			&_scriptHandler::o_execute_Unit_Null},
	{"Null",			&_scriptHandler::o_execute_Unit_Null},
	{"GetNearObjectTypeIndex",	&_scriptHandler::o_execute_Unit_Null},
	{"sub_282BC",			&_scriptHandler::o_execute_Unit_Null},
	{"GetField64",			&_scriptHandler::o_execute_Unit_Null},
	{"Null",			&_scriptHandler::o_execute_Unit_Null},
	{"GetAttackObjectIndexType",	&_scriptHandler::o_execute_Unit_Null},
	{"ObjectIsValid?",		&_scriptHandler::o_execute_Unit_Null},
	{"DelayAnd?",			&_scriptHandler::o_execute_Unit_Null},
	{"sub_27053",			&_scriptHandler::o_execute_Unit_Null},
	{"ObjectDistanceCalc",		&_scriptHandler::o_execute_Unit_Null},
	{"Null",			&_scriptHandler::o_execute_Unit_Null},
    };

    _opcodesExecute = scriptOpcodesExecuteUnits;
}

// The 'Houses' Execute functions
void _scriptHandler::opcodesHousesSetup() {
    static const _Opcode scriptOpcodesExecuteHouses[] = {
	{"Delay",			&_scriptHandler::o_execute_House_Null}, 	
	{"HouseText",			&_scriptHandler::o_execute_House_Null},
	{"HouseLastUnitIndexGet",	&_scriptHandler::o_execute_House_Null},
	{"sub_24E93",			&_scriptHandler::o_execute_House_Null},
	{"sub_2503A",			&_scriptHandler::o_execute_House_Null},
	{"sub_251BA",			&_scriptHandler::o_execute_House_Null},
	{"sub_2533D",			&_scriptHandler::o_execute_House_Null},
	{"sub_253FF",			&_scriptHandler::o_execute_House_Null},
	{"sub_2563B",			&_scriptHandler::o_execute_House_Null},
	{"sub_25697",			&_scriptHandler::o_execute_House_Null},
	{"DelayAnd",			&_scriptHandler::o_execute_House_Null},
	{"sub_1CFC4",			&_scriptHandler::o_execute_House_Null},
	{"UnitCountGet",		&_scriptHandler::o_execute_House_Null},
	{"WindtrapCountGet",		&_scriptHandler::o_execute_House_Null},
	{"Null",			&_scriptHandler::o_execute_House_Null},
    };

    _opcodesExecute = scriptOpcodesExecuteHouses;
}

