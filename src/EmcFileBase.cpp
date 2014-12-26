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

#include "eastwood/StdDef.h"

#include "eastwood/EmcFileBase.h"
#include "eastwood/EmcFileDisassemble.h"
#include "eastwood/EmcFileAssemble.h"

#include "eastwood/Exception.h"

namespace eastwood {

EmcFileBase::EmcFileBase(std::istream &input, std::ostream &output) :
    _inputStream(reinterpret_cast<IStream&>(input)),
    _outputStream(reinterpret_cast<OStream&>(output)),
    _opcodes(NULL), _opcodesEvaluate(NULL), _opcodesExecute(NULL),
    _headerPointers(0), _pointerCount(0), _scriptSize(0), _modePreProcess(true),
    _objectNames(NULL), _objectFunctions(NULL), _lineCount(0),
   _scriptBuffer(NULL), _scriptPtr(NULL), _scriptPos(0), _scriptStart(0),
    _scriptType(script_INVALID), _scriptLabels(std::vector<labelPosition>())
{
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
void EmcFileBase::opcodesSetup(std::string currentLine) {
    // Object Names
    static const char *nameTeams[] = {
	"Normal",
	"Staging",
	"Flee",
	"Kamikaze",
	"Guard",
	"Unknown"
    };
    static const size_t houseSize = sizeof(nameTeams)/sizeof(*nameTeams);

    static const char *nameStructures[] = {
	"Concrete",
	"Concrete4",
	"Palace",
	"LightFactory",
	"HeavyFactory",
	"Hi-Tech",
	"IX",
	"WOR",
	"ConstructionYard",
	"Windtrap",
	"Barracks",
	"Starport",
	"Refinery",
	"Repair",
	"Wall",
	"Turret",
	"R-Turret",
	"SpiceSilo",
	"Outpost"
    };
    static const size_t structureSize = sizeof(nameStructures)/sizeof(*nameStructures);

    static const char *nameUnits[] = {
	"Carryall",
	"Ornithopter",
	"Infantry",
	"Troopers",
	"Soldier",
	"Trooper",
	"Saboteur",
	"Launcher",
	"Deviator",
	"Tank",
	"SiegeTank",
	"Devastator",
	"SonicTank",
	"Trike",
	"RaiderTrike",	
	"Quad",
	"Harvester",
	"MCV",
	"DeathHand",
	"Rocket",
	"ARocket",
	"GRocket",
	"MiniRocket",
	"Bullet",
	"SonicBlast",
	"Sandworm",
	"Frigate"
    };
    static const size_t unitSize = sizeof(nameUnits)/sizeof(*nameUnits);


#define OPCODE(x) { #x, &EmcFileBase::o_##x }
    static const _Opcode scriptOpcodes[] = {
	OPCODE(Goto),
	OPCODE(SetReturn),
	OPCODE(PushOp),
	OPCODE(PushWord),
	OPCODE(Push),
	OPCODE(PushReg),
	OPCODE(PushFrameMinArg),
	OPCODE(PushFramePluArg),
	OPCODE(Pop),
	OPCODE(PopReg),
	OPCODE(PopFrameMinArg),
	OPCODE(PopFramePluArg),
	OPCODE(AddSP),
	OPCODE(SubSP),
	OPCODE(Execute),
	OPCODE(IfNotGoto),
	OPCODE(Negate),
	OPCODE(Evaluate),
	OPCODE(Return),
    };
#undef OPCODE

    // The 'Evaluate' opcode functions
#define OPCODE(x) { #x, &EmcFileBase::o_evaluate_##x }
    static const _Opcode scriptOpcodesEvaluate[] = {
	OPCODE(IfEither),
	OPCODE(IfEither1),
	OPCODE(Equal),
	OPCODE(NotEqual),
	OPCODE(CompareGreaterEqual),
	OPCODE(CompareGreater),
	OPCODE(CompareLessEqual),
	OPCODE(CompareLess),
	OPCODE(Add),
	OPCODE(Subtract),
	OPCODE(Multiply),
	OPCODE(Divide),
	OPCODE(ShiftRight),
	OPCODE(ShiftLeft),
	OPCODE(And),
	OPCODE(Or),
	OPCODE(DivideRemainder),
	OPCODE(XOR),
    };

    _opcodes = scriptOpcodes;
    _opcodesEvaluate = scriptOpcodesEvaluate;
#undef OPCODE

    if(currentLine == "[Team]")
	_pointerCount = houseSize;
    else if(currentLine == "[Build]")
	_pointerCount = structureSize;
    else if(currentLine == "[Unit]")
	_pointerCount = unitSize;

    switch(_pointerCount) {
	case houseSize:
	    _scriptType = script_TEAM;
	    _objectNames = nameTeams;
	    opcodesTeamsSetup();
	    break;
	case structureSize:
	    _scriptType = script_BUILD;
	    _objectNames = nameStructures;
	    opcodesBuildingsSetup();
	    break;
	case unitSize:
	    _scriptType = script_UNIT;
	    _objectNames = nameUnits;
	    opcodesUnitsSetup();
	    break;
	default:
	    throw Exception(LOG_ERROR, __FUNCTION__, "Unable to determine script type!");
	    break;
    }
}

// The 'Building' Execute functions
void EmcFileBase::opcodesBuildingsSetup() {
#define OPCODE(x) { #x, &EmcFileBase::o_execute_Building_Null }
    static const _Opcode scriptOpcodesExecuteBuildings[] = {
	OPCODE(Delay),
	OPCODE(Null),
	OPCODE(VerifyAttached),
	OPCODE(Attach),
	OPCODE(SetFrame),
	OPCODE(Text),
	OPCODE(Disattach),
	OPCODE(Deploy),
	OPCODE(GetTarget),
	OPCODE(SetAngle),
	OPCODE(GetAngle),
	OPCODE(ShootTarget),
	OPCODE(Null),
	OPCODE(GetFrame),
	OPCODE(PlaySfx),
	OPCODE(Reveal),
	OPCODE(Null),
	OPCODE(Null),
	OPCODE(Null),
	OPCODE(Null),
	OPCODE(Null),
	OPCODE(RefineSpice),
	OPCODE(Explode),
	OPCODE(Destroy),
	OPCODE(Null),
    };

    _opcodesExecute = scriptOpcodesExecuteBuildings;
#undef OPCODE
}

// The 'Units' Execute functions
void EmcFileBase::opcodesUnitsSetup() {
#define OPCODE(x) { #x, &EmcFileBase::o_execute_Unit_Null }
    static const _Opcode scriptOpcodesExecuteUnits[] = {
	{"GetDetail",	&EmcFileBase::o_execute_Unit_GetDetail},	// 0
	OPCODE(ActionStart),						// 1
	OPCODE(Text),							// 2
	OPCODE(ObjectDistance),						// 3
	OPCODE(sub_279AB),						// 4
	OPCODE(sub_27186),						// 5
	OPCODE(GetAngleBetween),					// 6
	OPCODE(BaseTurnToAngle),					// 7
	OPCODE(Attack),							// 8
	OPCODE(MCVDeploy),						// 9
	OPCODE(SideBarCmd4),						// A
	OPCODE(Flash),							// B
	OPCODE(PathFind),						// C
	OPCODE(HouseCompare),						// D
	OPCODE(sub_26A29),						// E
	OPCODE(Destroy),						// F
	OPCODE(Delay),							// 10
	OPCODE(sub_12D2EE),						// 11
	OPCODE(sub_26AB4),						// 12
	OPCODE(sub_26745),						// 13
	OPCODE(sub_25F69),						// 14
	OPCODE(Null),							// 15
	OPCODE(sub_2677F),						// 16
	OPCODE(RandomNumber),						// 17
	OPCODE(BuildingIsFree),						// 18
	OPCODE(SetMoveDestTurnTo),					// 19
	OPCODE(StopMove),						// 1A
	OPCODE(SpeedUpdate),						// 1B
	OPCODE(sub_25F3F),						// 1C
	OPCODE(DamageGet),						// 1D
	OPCODE(Dock?),							// 1E
	OPCODE(TestState100),						// 1F
	OPCODE(CheckHarvestReturn),					// 20
	OPCODE(CreateSoldier),						// 21
	OPCODE(DeliverToBuilding),					// 22
	OPCODE(PlaceInUnit),						// 23
	OPCODE(ScriptObjectHoldingSetToReg4),				// 24
	OPCODE(BuildingFreeFind),					// 25
	OPCODE(PlaySFX),						// 26
	OPCODE(DestroyedMessage),					// 27
	OPCODE(MapReveal),						// 28
	OPCODE(MapGetTile),						// 29
	OPCODE(Harvest),						// 2A
	OPCODE(Null),							// 2B
	OPCODE(GetHoldingScriptObject),					// 2C
	OPCODE(ScriptObjectTypeGet),					// 2D
	OPCODE(ScriptObjectIndexGet),					// 2E
	OPCODE(sub_27E8B),						// 2F
	OPCODE(GetMapPieceForUnit),					// 30
	OPCODE(RandomRotate),						// 31
	OPCODE(UnitOfTypeCountGet),					// 32
	OPCODE(BuildingFindMoveTo),					// 33
	OPCODE(Null),							// 34
	OPCODE(Null),							// 35
	OPCODE(FindNearUnitGetScriptObject),				// 36
	OPCODE(sub_282BC),						// 37
	OPCODE(GetBaseCurrentAngle),					// 38
	OPCODE(Null),							// 39
	OPCODE(SetTargetScriptObject),					// 3A
	OPCODE(ObjectIsValid?),						// 3B
	OPCODE(DelayAnd?),						// 3C
	OPCODE(TurnToMapDest),						// 3D
	OPCODE(ScriptObjectDistanceCalc),				// 3E
	OPCODE(Null),							// 3F
    };

    _opcodesExecute = scriptOpcodesExecuteUnits;
#undef OPCODE
}

// The 'Team' Execute functions
void EmcFileBase::opcodesTeamsSetup() {
#define OPCODE(x) { #x, &EmcFileBase::o_execute_Team_Null }
    static const _Opcode scriptOpcodesExecuteTeams[] = {
	OPCODE(Delay),
	OPCODE(Text?),
	OPCODE(UnitsUsingThisTeam),
	OPCODE(FindUnitForTeam),
	OPCODE(sub_2503A),
	OPCODE(sub_251BA),
	OPCODE(sub_2533D),
	OPCODE(ActionStart),
	OPCODE(AiModeSet),
	OPCODE(ScriptClearPrepare),
	OPCODE(DelayRandom),
	OPCODE(sub_1CFC4),
	OPCODE(Field6Get),
	OPCODE(ScriptObjectTargetGet),
	OPCODE(Null),
    };

    _opcodesExecute = scriptOpcodesExecuteTeams;
#undef OPCODE
}

}
