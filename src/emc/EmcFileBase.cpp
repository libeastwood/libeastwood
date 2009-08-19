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

#include "EmcFileBase.h"
#include "EmcFileDecompile.h"
#include "EmcFileCompile.h"


EmcFileBase::EmcFileBase(const char *fileName) {
    _fileName		= fileName;

    _headerPointers	= 0;
    _pointerCount	= 0;

    _scriptBuffer	= NULL;
    _scriptPtr		= NULL;
    _scriptPos		= 0;

    _modePreProcess	= true;
}

// Destructor
EmcFileBase::~EmcFileBase() {
    if(_scriptBuffer)
	delete [] _scriptBuffer;

    if(_headerPointers)
	delete [] _headerPointers;
}

// Search the opcode table for a string opcode name
uint16_t EmcFileBase::scriptOpcodeFind(std::string opcodeStr, const _Opcode *opcodes) {
    const _Opcode *opcode;
    uint16_t count = 0;

    // Search opcode list
    for(opcode = opcodes; opcode->description; opcode++, count++) {
	if(opcodeStr.compare(opcode->description) == 0)
	    return count;
    }

    // Return invalid opcode
    return 0xFFFF;
}

// Setup the opcode name/function table
void EmcFileBase::opcodesSetup() {
    static const _Opcode scriptOpcodes[] = {
	{"Goto",		&EmcFileBase::o_goto},
	{"SetReturn",		&EmcFileBase::o_setreturn},
	{"PushOp",		&EmcFileBase::o_pushOp},
	{"PushWord",		&EmcFileBase::o_pushWord},
	{"Push",		&EmcFileBase::o_push},
	{"PushReg",		&EmcFileBase::o_pushreg},
	{"PushFrameMinArg",	&EmcFileBase::o_pushframeMinArg},
	{"PushFramePluArg",	&EmcFileBase::o_pushframePluArg},
	{"Pop",			&EmcFileBase::o_popret},
	{"PopReg",		&EmcFileBase::o_popreg},
	{"PopFrameMinArg",	&EmcFileBase::o_popframeMinArg},
	{"PopFramePluArg",	&EmcFileBase::o_popframePluArg},
	{"AddSP",		&EmcFileBase::o_spadd},
	{"SubSP",		&EmcFileBase::o_spsub},
	{"Execute",		&EmcFileBase::o_execute},
	{"IfNotGoto",		&EmcFileBase::o_ifnotgoto},
	{"Negate",		&EmcFileBase::o_negate},
	{"Evaluate",		&EmcFileBase::o_evaluate},
	{"Return",		&EmcFileBase::o_return},
    };

    // The 'Evaluate' opcode functions
    static const _Opcode scriptOpcodesEvaluate[] = {
	{"IfEither",		&EmcFileBase::o_evaluate_IfEither},
	{"IfEither1",		&EmcFileBase::o_evaluate_IfEither},
	{"Equal",		&EmcFileBase::o_evaluate_Equal},
	{"NotEqual",		&EmcFileBase::o_evaluate_NotEqual},
	{"CompareGreaterEqual",	&EmcFileBase::o_evaluate_CompareGreaterEqual},
	{"CompareGreater",	&EmcFileBase::o_evaluate_CompareGreater},
	{"CompareLessEqual",	&EmcFileBase::o_evaluate_CompareLessEqual},
	{"CompareLess",		&EmcFileBase::o_evaluate_CompareLess},
	{"Add",			&EmcFileBase::o_evaluate_Add},
	{"Subtract",		&EmcFileBase::o_evaluate_Subtract},
	{"Multiply",		&EmcFileBase::o_evaluate_Multiply},
	{"Divide",		&EmcFileBase::o_evaluate_Divide},
	{"ShiftRight",		&EmcFileBase::o_evaluate_ShiftRight},
	{"ShiftLeft",		&EmcFileBase::o_evaluate_ShiftLeft},
	{"And",			&EmcFileBase::o_evaluate_And},
	{"Or",			&EmcFileBase::o_evaluate_Or},
	{"DivideRemainder",	&EmcFileBase::o_evaluate_DivideRemainder},
	{"XOR",			&EmcFileBase::o_evaluate_XOR},
    };

    _opcodes		  = scriptOpcodes;
    _opcodesEvaluate  = scriptOpcodesEvaluate;
}

// The 'Building' Execute functions
void EmcFileBase::opcodesBuildingsSetup() {
    static const _Opcode scriptOpcodesExecuteBuildings[] = {
	{"Delay",		&EmcFileBase::o_execute_Building_Null},
	{"Null",		&EmcFileBase::o_execute_Building_Null},
	{"VerifyAttached",	&EmcFileBase::o_execute_Building_Null},	
	{"Attach",		&EmcFileBase::o_execute_Building_Null},
	{"SetFrame",		&EmcFileBase::o_execute_Building_Null},
	{"Text",		&EmcFileBase::o_execute_Building_Null},
	{"Disattach",		&EmcFileBase::o_execute_Building_Null},
	{"Deploy",		&EmcFileBase::o_execute_Building_Null},
	{"GetTarget",		&EmcFileBase::o_execute_Building_Null},
	{"SetAngle",		&EmcFileBase::o_execute_Building_Null},
	{"GetAngle",		&EmcFileBase::o_execute_Building_Null},
	{"ShootTarget",		&EmcFileBase::o_execute_Building_Null},
	{"Null",		&EmcFileBase::o_execute_Building_Null},
	{"GetFrame",		&EmcFileBase::o_execute_Building_Null},
	{"PlaySfx",		&EmcFileBase::o_execute_Building_Null},
	{"Reveal",		&EmcFileBase::o_execute_Building_Null},
	{"Null",		&EmcFileBase::o_execute_Building_Null},
	{"Null",		&EmcFileBase::o_execute_Building_Null},
	{"Null",		&EmcFileBase::o_execute_Building_Null},
	{"Null",		&EmcFileBase::o_execute_Building_Null},
	{"Null",		&EmcFileBase::o_execute_Building_Null},
	{"RefineSpice",		&EmcFileBase::o_execute_Building_Null},
	{"Explode",		&EmcFileBase::o_execute_Building_Null},
	{"Destroy",		&EmcFileBase::o_execute_Building_Null},
	{"Null",		&EmcFileBase::o_execute_Building_Null},
    };

    _opcodesExecute = scriptOpcodesExecuteBuildings;

}

// The 'Units' Execute functions
void EmcFileBase::opcodesUnitsSetup() {
    static const _Opcode scriptOpcodesExecuteUnits[] = {
	{"GetDetail",			&EmcFileBase::o_execute_Unit_GetDetail}, 	
	{"sub_272E7",			&EmcFileBase::o_execute_Unit_Null},
	{"Text",   			&EmcFileBase::o_execute_Unit_Null},
	{"getObjectDistance?",		&EmcFileBase::o_execute_Unit_Null},
	{"sub_279AB",			&EmcFileBase::o_execute_Unit_Null},
	{"sub_27186",			&EmcFileBase::o_execute_Unit_Null},
	{"sub_27127",			&EmcFileBase::o_execute_Unit_Null},
	{"sub_27019",			&EmcFileBase::o_execute_Unit_Null},
	{"Attack",			&EmcFileBase::o_execute_Unit_Null},
	{"MCVDeploy",			&EmcFileBase::o_execute_Unit_Null},
	{"SidebarCmd4",			&EmcFileBase::o_execute_Unit_Null},
	{"Flash",			&EmcFileBase::o_execute_Unit_Null},
	{"sub_27638",			&EmcFileBase::o_execute_Unit_Null},
	{"HouseCompare",		&EmcFileBase::o_execute_Unit_Null},
	{"sub_26A69",			&EmcFileBase::o_execute_Unit_Null},
	{"Destroy",			&EmcFileBase::o_execute_Unit_Null},
	{"Delay",			&EmcFileBase::o_execute_Unit_Null},
	{"sub_1D2EE",			&EmcFileBase::o_execute_Unit_Null},
	{"sub_26AB4",			&EmcFileBase::o_execute_Unit_Null},
	{"sub_26745",			&EmcFileBase::o_execute_Unit_Null},
	{"sub_25F69",			&EmcFileBase::o_execute_Unit_Null},
	{"Null",			&EmcFileBase::o_execute_Unit_Null},
	{"sub_2677F",			&EmcFileBase::o_execute_Unit_Null},
	{"RandomNumber",		&EmcFileBase::o_execute_Unit_Null},
	{"sub_1D3D4",			&EmcFileBase::o_execute_Unit_Null},
	{"sub_27356",			&EmcFileBase::o_execute_Unit_Null},
	{"sub_26689",			&EmcFileBase::o_execute_Unit_Null},
	{"sub_266B9",			&EmcFileBase::o_execute_Unit_Null},
	{"sub_25F3F",			&EmcFileBase::o_execute_Unit_Null},
	{"DamageGet",			&EmcFileBase::o_execute_Unit_Null},
	{"Dock?",			&EmcFileBase::o_execute_Unit_Null},
	{"EMCDataTest?",		&EmcFileBase::o_execute_Unit_Null},
	{"CheckHarvestReturn",		&EmcFileBase::o_execute_Unit_Null},
	{"CreateSoldier",		&EmcFileBase::o_execute_Unit_Null},
	{"DeliverToBuilding",		&EmcFileBase::o_execute_Unit_Null},
	{"PlaceInUnit",			&EmcFileBase::o_execute_Unit_Null},
	{"CarryAllHoldingClear",	&EmcFileBase::o_execute_Unit_Null},
	{"BuildingFreeFind",		&EmcFileBase::o_execute_Unit_Null},
	{"PlaySFX",			&EmcFileBase::o_execute_Unit_Null},
	{"DestroyedMessage",		&EmcFileBase::o_execute_Unit_Null},
	{"MapReveal",			&EmcFileBase::o_execute_Unit_Null},
	{"MapGetTile",			&EmcFileBase::o_execute_Unit_Null},
	{"Harvest",			&EmcFileBase::o_execute_Unit_Null},
	{"Null",			&EmcFileBase::o_execute_Unit_Null},
	{"GetHoldingType",		&EmcFileBase::o_execute_Unit_Null},
	{"GetType",			&EmcFileBase::o_execute_Unit_Null},
	{"IndexGet ",			&EmcFileBase::o_execute_Unit_Null},
	{"sub_27E8B",			&EmcFileBase::o_execute_Unit_Null},
	{"GetMapPieceForUnit",		&EmcFileBase::o_execute_Unit_Null},
	{"sub_28001",			&EmcFileBase::o_execute_Unit_Null},
	{"TypeCount",			&EmcFileBase::o_execute_Unit_Null},
	{"sub_28090",			&EmcFileBase::o_execute_Unit_Null},
	{"Null",			&EmcFileBase::o_execute_Unit_Null},
	{"Null",			&EmcFileBase::o_execute_Unit_Null},
	{"GetNearObjectTypeIndex",	&EmcFileBase::o_execute_Unit_Null},
	{"sub_282BC",			&EmcFileBase::o_execute_Unit_Null},
	{"GetField64",			&EmcFileBase::o_execute_Unit_Null},
	{"Null",			&EmcFileBase::o_execute_Unit_Null},
	{"GetAttackObjectIndexType",	&EmcFileBase::o_execute_Unit_Null},
	{"ObjectIsValid?",		&EmcFileBase::o_execute_Unit_Null},
	{"DelayAnd?",			&EmcFileBase::o_execute_Unit_Null},
	{"sub_27053",			&EmcFileBase::o_execute_Unit_Null},
	{"ObjectDistanceCalc",		&EmcFileBase::o_execute_Unit_Null},
	{"Null",			&EmcFileBase::o_execute_Unit_Null},
    };

    _opcodesExecute = scriptOpcodesExecuteUnits;
}

// The 'Houses' Execute functions
void EmcFileBase::opcodesHousesSetup() {
    static const _Opcode scriptOpcodesExecuteHouses[] = {
	{"Delay",			&EmcFileBase::o_execute_House_Null}, 	
	{"HouseText",			&EmcFileBase::o_execute_House_Null},
	{"HouseLastUnitIndexGet",	&EmcFileBase::o_execute_House_Null},
	{"sub_24E93",			&EmcFileBase::o_execute_House_Null},
	{"sub_2503A",			&EmcFileBase::o_execute_House_Null},
	{"sub_251BA",			&EmcFileBase::o_execute_House_Null},
	{"sub_2533D",			&EmcFileBase::o_execute_House_Null},
	{"sub_253FF",			&EmcFileBase::o_execute_House_Null},
	{"sub_2563B",			&EmcFileBase::o_execute_House_Null},
	{"sub_25697",			&EmcFileBase::o_execute_House_Null},
	{"DelayAnd",			&EmcFileBase::o_execute_House_Null},
	{"sub_1CFC4",			&EmcFileBase::o_execute_House_Null},
	{"UnitCountGet",		&EmcFileBase::o_execute_House_Null},
	{"WindtrapCountGet",		&EmcFileBase::o_execute_House_Null},
	{"Null",			&EmcFileBase::o_execute_House_Null},
    };

    _opcodesExecute = scriptOpcodesExecuteHouses;
}

