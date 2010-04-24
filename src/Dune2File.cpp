#include "eastwood/StdDef.h"
#include "eastwood/Dune2File.h"

namespace eastwood { namespace dune2 {

struct Address {
    uint16_t segment;
    uint16_t offset;
};

static const Address
    VersionOffset[D2_VERSIONS] = { {0x325f, 0xe}, {0x333a, 0x2}, {0x32fb, 0x2}, {0x3353, 0x2}, {0x334d, 0x2} },
    StructureOffset[D2_VERSIONS] = { {0x2c2f, 0xa}, {0x2b3d, 0xa}, {0x2a98, 0xa}, {0x2a9d, 0xa}, {0x2a98, 0xa} },
    UnitOffset[D2_VERSIONS] = { {0x2ca7, 0x0}, {0x2bb5, 0x0}, {0x2b0b, 0x0}, {0x2b10, 0x0}, {0x2b0b, 0x0} },
    HouseOffset[D2_VERSIONS] = { {0x3574, 0xa}, {0x3668, 0x4}, {0x3615, 0xc}, {0x36c7, 0x8}, {0x36c0, 0xc} },
    FileOffset[D2_VERSIONS] = { {0x2e28, 0x0}, {0x2d1b, 0x0}, {0x2ca0, 0x0}, {0x2ca5, 0x0}, {0x2ca0, 0x0} },
    ActionOffset[D2_VERSIONS] = { {0x2e1c, 0xe}, {0x2d0f, 0xe}, {0x2c94, 0xe}, {0x2c99, 0xe}, {0x2c94, 0xe} },
    MovementOffset[D2_VERSIONS] = { {0x0, 0x0}, {0x0, 0x0}, {0x0, 0x}, {0x3348, 0x3BE2}, {0x3342, 0x3bd6 } },
    LayoutTilesOffset[D2_VERSIONS] = { {0x3342, 0x1920}, {0x3342, 0x28d4}, {0x3342, 0x2296}, {0x3342, 0x2db2}, {0x3342, 0x2c58 } },    
    AngleTableOffset[D2_VERSIONS] = { {0,0}, {0,0}, {0,0}, {0x3348, 0x23da}, {0x3342, 0x23ce} },
    MapMoveModOffset[D2_VERSIONS] = { {0,0}, {0,0}, {0,0}, {0,0}, {0x3342, 0x3776} },
    AnimPtrsOffset[D2_VERSIONS] = { {0,0}, {0,0}, {0,0}, {0,0}, {0x3342, 0x3206} },
    MapOffsetIndexesOffset[D2_VERSIONS] = { {0,0}, {0,0}, {0,0}, {0,0}, {0x3342, 0x2006} },
    MovementUnk1Offset[D2_VERSIONS] = { {0,0}, {0,0}, {0,0}, {0,0}, {0x3342, 0x2468} },
    Emc15CDAOffset[D2_VERSIONS] = { {0,0}, {0,0}, {0,0}, {0,0}, {0x0001, 0x5cca} },    
    GlobalDataOffset[D2_VERSIONS] = { {0x3251, 0}, {0x332f, 0}, {0x32f0, 0}, {0x3348, 0}, {0x3342, 0 } };

ObjectData::ObjectData() :
    typeIndex(0),idShort(0),name(""),idLong(0),picture(""),options(0),infantrySpawn(0),
    hitPoints(0),sight(0),sidebarIconID(0),cost(0),buildTime(0),techLevel(0),preReqs(0),
    buildOrder(0),upgradesNeeded(0),owner(0),gfxID(0),weaponDamage(0)
    {}

StructureData::StructureData() : ObjectData(),
    field_22(0),field_23(0),field_24(0),field_25(0),field_26(0),field_27(0),field_28(0),
    field_29(0),field_2A(0),structureID(0),weaponDamageAdd(0),unitsCanEnter(0),spiceStorage(0),
    powerUsage(0),foundationSize(0),frameData(3),constructOpt(8),techUpgrade(3)
    {}

UnitData::UnitData() : ObjectData(),
    sidebarCommand1(0),sidebarCommand2(0),sidebarCommand3(0),sidebarCommand4(0),field_2A(0),
    field_2B(0),field_2D(0),aggressivity(0),indexMin(0),indexMax(0),optsFitW(0),field_38(0),
    field_39(0),field_3A(0), movementType(0),movementPerFrame(0),speed(0),turningSpeed(0),
    turretGfxID(0),aiCommand(0),frameAngleMode(0),deathAnim(0),weaponRateFire(0),weaponRange(0),
    field_54(0),weaponType(0),weaponSound(0)
    {}

HouseData::HouseData() :
    name(""),weakness(0),lemonFactor(0),buildingDecay(0),color(0),palaceUnitRecharge(0),frigateTime(0),
    houseLetter(0),palaceMode(0),missionWinMusic(0),missionLoseMusic(0),missionBriefMusic(0),houseVoice("")
    {}

ActionData::ActionData() :
    field_0(0),name(""),interruptAction(0),sidebarMode(0),responseSound(0)
    {}

FileData::FileData() :
    name(""),field_4(0),field_6(0),field_8(0),field_A(0),field_C(0),field_E(0),parentIndex(0),
    field_11(0),fileType(0)
    {}

Dune2File::Dune2File(ExeFile &stream) :
    _stream(stream), _version(D2_VERSIONS),
    _structureData(19),
    _unitData(27),
    _houseData(6),
    _actionData(14),
    _fileData(0),
    _movementNames(6),
    _teamActionNames(5),
    _layoutTiles(7),
    _layoutTilesUnk1(7),    
    _layoutTileCount(7),
    _layoutTilesAround(7),
    _layoutSize(7),
    _layoutTileDiff(7),
    _angleTable(205),
    _mapMoveMod(8),
    _mapMod(8),
    _mapOffsetIndexes(21),
    _mapOffsets(336),
    _anims(20),
    _unitAngleFrameAdjust(83),
    _unitFrameAdjust(8),
    _unitTurretFrameAdjust(36),
    _movementUnk1(16),
    _mapTileColors(83),
    _mapScales(3),
    _actionsAI(4),
    _mapSinTable(256),
    _mapCosTable(256),
    _emc15CDA(16)
{
    detectDune2Version();
    readDataStructures();
}

void Dune2File::detectDune2Version()
{
    char tmp[10];
    for(int i = D2_DEMO; i < D2_VERSIONS; i++) {
    	_stream.seekSegOff(VersionOffset[i].segment, VersionOffset[i].offset);
	_stream.read(tmp, sizeof(tmp));

	if(strncmp(tmp, "DUNE2.EXE", sizeof(tmp)) == 0) {
	    _version = (Version)i;
	    break;
	}
    }
}

void Dune2File::readDataStructures()
{
    _stream.seekSegOff(StructureOffset[_version].segment, StructureOffset[_version].offset);
    uint16_t idx = 0;    
    for(std::vector<Structure>::iterator it = _structureData.begin(); it != _structureData.end(); ++it, ++idx) {
	it->reset(new StructureData);
	(*it)->typeIndex		= idx;
	(*it)->idShort			= _stream.getU16LE();
	(*it)->name			= stringGet(_stream.getU32LE());
	(*it)->idLong			= _stream.getU16LE();
	(*it)->picture			= stringGet(_stream.getU32LE());
	(*it)->options			= _stream.getU16LE();
	(*it)->infantrySpawn		= _stream.getU16LE();
	(*it)->hitPoints		= _stream.getU16LE();
	(*it)->sight			= _stream.getU16LE();
	(*it)->sidebarIconID		= _stream.getU16LE();
	(*it)->cost			= _stream.getU16LE();
	(*it)->buildTime		= _stream.getU16LE();
	(*it)->techLevel		= _stream.getU16LE();
	(*it)->preReqs			= _stream.getU32LE();
	(*it)->buildOrder		= _stream.get();
	(*it)->upgradesNeeded		= _stream.get();
	(*it)->field_22			= _stream.get();
	(*it)->field_23			= _stream.get();
	(*it)->field_24			= _stream.get();
	(*it)->field_25			= _stream.get();
	(*it)->field_26			= _stream.get();
	(*it)->field_27			= _stream.get();
	(*it)->field_28			= _stream.get();
	(*it)->field_29			= _stream.get();
	(*it)->field_2A			= _stream.get();
	(*it)->structureID		= _stream.getU16LE();
	(*it)->weaponDamage		= _stream.getU16LE();
	(*it)->weaponDamageAdd		= _stream.getU16LE();
	(*it)->owner			= _stream.get();
	(*it)->unitsCanEnter		= _stream.getU32LE();
	(*it)->spiceStorage		= _stream.getU16LE();
	(*it)->powerUsage		= _stream.getU16LE();
	(*it)->foundationSize		= _stream.getU16LE();
	(*it)->gfxID			= _stream.getU16LE();
	(*it)->frameData[0]		= _stream.getU32LE();
	(*it)->frameData[1]		= _stream.getU32LE();
	(*it)->frameData[2]		= _stream.getU32LE();
	(*it)->constructOpt[0]		= _stream.getU16LE();	
	(*it)->constructOpt[1]		= _stream.getU16LE();
	(*it)->constructOpt[2]		= _stream.getU16LE();
	(*it)->constructOpt[3]		= _stream.getU16LE();
	(*it)->constructOpt[4]		= _stream.getU16LE();
	(*it)->constructOpt[5]		= _stream.getU16LE();
	(*it)->constructOpt[6]		= _stream.getU16LE();
	(*it)->constructOpt[7]		= _stream.getU16LE();
	(*it)->techUpgrade[0]		= _stream.getU16LE();
	(*it)->techUpgrade[1]		= _stream.getU16LE();
	(*it)->techUpgrade[2]		= _stream.getU16LE();
    }

    _stream.seekSegOff(UnitOffset[_version].segment, UnitOffset[_version].offset);
    idx = 0;
    for(std::vector<Unit>::iterator it = _unitData.begin(); it != _unitData.end(); ++it, ++idx) {
	it->reset(new UnitData);	
	(*it)->typeIndex		= idx;
	(*it)->idShort			= _stream.getU16LE(),
	(*it)->name			= stringGet(_stream.getU32LE()),
	(*it)->idLong			= _stream.getU16LE(),
	(*it)->picture			= stringGet(_stream.getU32LE()),
	(*it)->options			= _stream.getU16LE(),
	(*it)->infantrySpawn		= _stream.getU16LE(),
	(*it)->hitPoints		= _stream.getU16LE(),
	(*it)->sight			= _stream.getU16LE(),
	(*it)->sidebarIconID		= _stream.getU16LE(),
	(*it)->cost			= _stream.getU16LE(),
	(*it)->buildTime		= _stream.getU16LE(),
	(*it)->techLevel		= _stream.getU16LE(),
	(*it)->preReqs			= _stream.getU32LE(),
	(*it)->buildOrder		= _stream.get(),
	(*it)->upgradesNeeded		= _stream.get(),	
	(*it)->sidebarCommand1		= _stream.getU16LE(),
	(*it)->sidebarCommand2		= _stream.getU16LE(),
	(*it)->sidebarCommand3		= _stream.getU16LE(),
	(*it)->sidebarCommand4		= _stream.getU16LE(),
	(*it)->field_2A			= _stream.get(),
	(*it)->field_2B			= _stream.getU16LE(),
	(*it)->field_2D			= _stream.getU16LE(),
	(*it)->aggressivity		= _stream.getU16LE(),
	(*it)->owner			= _stream.get(),
	(*it)->indexMin			= _stream.getU16LE(),
	(*it)->indexMax			= _stream.getU16LE(),
	(*it)->optsFitW			= _stream.getU16LE(),
	(*it)->field_38			= _stream.get(),
	(*it)->field_39			= _stream.get(),
	(*it)->field_3A			= _stream.getU16LE(),
	(*it)->movementType		= _stream.getU16LE(),
	(*it)->movementPerFrame		= _stream.getU16LE(),
	(*it)->speed			= _stream.getU16LE(),
	(*it)->turningSpeed		= _stream.getU16LE(),
	(*it)->gfxID			= _stream.getU16LE(),
	(*it)->turretGfxID		= _stream.getU16LE(),
	(*it)->aiCommand		= _stream.getU16LE(),
	(*it)->frameAngleMode		= _stream.getU16LE(),
	(*it)->deathAnim		= _stream.getU16LE(),
	(*it)->weaponRateFire		= _stream.getU16LE(),
	(*it)->weaponRange		= _stream.getU16LE(),
	(*it)->weaponDamage		= _stream.getU16LE(),
	(*it)->field_54			= _stream.getU16LE(),
	(*it)->weaponType		= _stream.getU16LE(),
	(*it)->weaponSound		= _stream.getU16LE();
    }

    _stream.seekSegOff(HouseOffset[_version].segment, HouseOffset[_version].offset);
    for(std::vector<House>::iterator it = _houseData.begin(); it != _houseData.end(); ++it) {
	it->reset(new HouseData);	
	(*it)->name			= stringGet(_stream.getU32LE()),
	(*it)->weakness			= _stream.getU16LE(),
	(*it)->lemonFactor		= _stream.getU16LE(),
	(*it)->buildingDecay		= _stream.getU16LE(),
	(*it)->color			= _stream.getU16LE(),
	(*it)->palaceUnitRecharge	= _stream.getU16LE(),
	(*it)->frigateTime		= _stream.getU16LE();
	(*it)->houseLetter		= _stream.get(),_stream.ignore(1),
	(*it)->palaceMode		= _stream.getU16LE(),
	(*it)->missionWinMusic		= _stream.getU16LE(),
	(*it)->missionLoseMusic		= _stream.getU16LE(),
	(*it)->missionBriefMusic	= _stream.getU16LE(),
	(*it)->houseVoice		= stringGet(_stream.getU32LE());	
    }

    _stream.seekSegOff(ActionOffset[_version].segment, ActionOffset[_version].offset);
    for(std::vector<Action>::iterator it = _actionData.begin(); it != _actionData.end(); ++it) {
	it->reset(new ActionData);	
	(*it)->field_0			= _stream.getU16LE(),
	(*it)->name			= stringGet(_stream.getU32LE()),
	(*it)->interruptAction		= _stream.getU16LE(),
	(*it)->sidebarMode		= _stream.getU16LE(),
	(*it)->responseSound		= _stream.getU16LE();
    }	

    _stream.seekSegOff(FileOffset[_version].segment, FileOffset[_version].offset);
    while(_stream.good() && _stream.peek() != 0) {
	File file(new FileData);
	file->name			= stringGet(_stream.getU32LE()),
	file->field_4			= _stream.getU16LE(),
	file->field_6			= _stream.getU16LE(),
	file->field_8			= _stream.getU16LE(),
	file->field_A			= _stream.getU16LE(),
	file->field_C			= _stream.getU16LE(),
	file->field_E			= _stream.getU16LE(),
	file->parentIndex		= _stream.get(),
	file->field_11			= _stream.get(),
	file->fileType			= _stream.get();
	_fileData.push_back(file);
    }

    _stream.seekSegOff(LayoutTilesOffset[_version].segment, LayoutTilesOffset[_version].offset);    
    for(std::vector<std::vector<uint16_t> >::iterator x = _layoutTiles.begin(); x != _layoutTiles.end(); ++x) {
	x->resize(9);
	_stream.readU16LE(&x->front(), x->size());
    }
    for(std::vector<std::vector<int16_t> >::iterator x = _layoutTilesUnk1.begin(); x != _layoutTilesUnk1.end(); ++x) {
	x->resize(8);
	_stream.readU16LE(reinterpret_cast<uint16_t*>(&x->front()), x->size());
    }
    _stream.readU16LE(&_layoutTileCount.front(), _layoutTileCount.size());
    for(std::vector<std::vector<int16_t> >::iterator x = _layoutTilesAround.begin(); x != _layoutTilesAround.end(); ++x) {
	x->resize(16);
	_stream.readU16LE(reinterpret_cast<uint16_t*>(&x->front()), x->size());
    }
    for(std::vector<Point<uint16_t> >::iterator it = _layoutSize.begin(); it != _layoutSize.end(); ++it) {
	it->x = _stream.getU16LE(),
	it->y = _stream.getU16LE();
    }
    for(std::vector<Point<uint16_t> >::iterator it = _layoutTileDiff.begin(); it != _layoutTileDiff.end(); ++it) {
	it->x = _stream.getU16LE(),
	it->y = _stream.getU16LE();
    }

    _stream.seekSegOff(AngleTableOffset[_version].segment, AngleTableOffset[_version].offset);
    _stream.readU16LE(reinterpret_cast<uint16_t*>(&_angleTable.front()), _angleTable.size());

    _stream.seekSegOff(MapMoveModOffset[_version].segment, MapMoveModOffset[_version].offset);
    _stream.readU16LE(&_mapMoveMod.front(), _mapMoveMod.size());
    _stream.read((char*)&_mapMod.front(), _mapMod.size());

    _stream.seekSegOff(AnimPtrsOffset[_version].segment, AnimPtrsOffset[_version].offset);
    for(std::vector<std::vector<uint16_t> >::iterator x = _anims.begin(); x != _anims.end(); ++x) {
	uint32_t addr = _stream.getU32LE();
	std::streampos pos(_stream.tellg());
	_stream.seekSegOff(addr);
	x->resize(8);
	_stream.readU16LE(&x->front(), x->size());
	_stream.seekg(pos);
    }

    // dunno what these are...
    _stream.ignore(66);
    _stream.readU16LE(&_unitAngleFrameAdjust.front(), _unitAngleFrameAdjust.size());
    _stream.read(reinterpret_cast<char*>(&_unitFrameAdjust.front()), _unitFrameAdjust.size());

    // dunno what these are...
    _stream.ignore(28);
    _stream.readU16LE(reinterpret_cast<uint16_t*>(&_unitTurretFrameAdjust.front()), _unitTurretFrameAdjust.size());

    _stream.seekSegOff(MapOffsetIndexesOffset[_version].segment, MapOffsetIndexesOffset[_version].offset);
    _stream.read(reinterpret_cast<char*>(&_mapOffsetIndexes.front()), _mapOffsetIndexes.size());    
    _stream.read(reinterpret_cast<char*>(&_mapOffsets.front()), _mapOffsets.size());

    _stream.seekSegOff(MovementUnk1Offset[_version].segment, MovementUnk1Offset[_version].offset);
    _stream.readU16LE(reinterpret_cast<uint16_t*>(&_movementUnk1.front()), _movementUnk1.size());    

    _stream.ignore(36);
    _stream.readU16LE(&_mapTileColors.front(), _mapTileColors.size());

    _stream.seekSegOff(MovementOffset[_version].segment, MovementOffset[_version].offset);
    for(std::vector<std::string>::iterator it = _movementNames.begin(); it != _movementNames.end(); ++it) {
	*it += stringGet(_stream.getU32LE());
    }
    for(std::vector<std::string>::iterator it = _teamActionNames.begin(); it != _teamActionNames.end(); ++it) {
	*it += stringGet(_stream.getU32LE());
    }

    for(std::vector<Rect<uint16_t> >::iterator map = _mapScales.begin(); map != _mapScales.end(); ++map) {
	map->x = _stream.getU16LE();
	map->y = _stream.getU16LE();
	map->w = _stream.getU16LE();
	map->h = _stream.getU16LE();
    }
    _stream.ignore(4);
    _stream.readU16LE(&_actionsAI.front(), _actionsAI.size());
    _stream.ignore(26);
    _stream.read(reinterpret_cast<char*>(&_mapSinTable.front()), _mapSinTable.size());
    _stream.read(reinterpret_cast<char*>(&_mapCosTable.front()), _mapCosTable.size());    

    _stream.seekSegOff(Emc15CDAOffset[_version].segment, Emc15CDAOffset[_version].offset);
    _stream.read(reinterpret_cast<char*>(&_emc15CDA.front()), _emc15CDA.size());    

}

std::string Dune2File::stringGet(uint32_t p)
{
    std::streampos pos(_stream.tellg());
    _stream.seekSegOff(p);

    char buf[64];
    _stream.getline(buf, sizeof(buf), 0);
    _stream.seekg(pos);
    return std::string(buf);
}

} } // namespace
