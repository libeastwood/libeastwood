#include "eastwood/StdDef.h"
#include "eastwood/Dune2File.h"
#include <map>

namespace eastwood { namespace dune2 {

struct Address {
    uint16_t segment;
    uint16_t offset;
};

static const Address
    VersionOffset[D2_VERSIONS] = { {0x325f, 0xe}, {0x333a, 0x2}, {0x32fb, 0x2}, {0x3353, 0x2}, {0x334d, 0x2} },
    StructureOffset[D2_VERSIONS] = { {0x0, 0x0}, {0x0,0x0}, {0x0,0x0}, {0x0,0x0}, {0x3246, 0x6} },
    UnitOffset[D2_VERSIONS] = { {0x2ca7, 0x0}, {0x2bb5, 0x0}, {0x2b0b, 0x0}, {0x2b10, 0x0}, {0x2b0b, 0x0} },
    HouseOffset[D2_VERSIONS] = { {0x3574, 0xa}, {0x3668, 0x4}, {0x3615, 0xc}, {0x36c7, 0x8}, {0x36c0, 0xc} },
    FileOffset[D2_VERSIONS] = { {0x2e28, 0x0}, {0x2d1b, 0x0}, {0x2ca0, 0x0}, {0x2ca5, 0x0}, {0x2ca0, 0x0} },
    ActionOffset[D2_VERSIONS] = { {0x2e1c, 0xe}, {0x2d0f, 0xe}, {0x2c94, 0xe}, {0x2c99, 0xe}, {0x2c94, 0xe} },
    MovementOffset[D2_VERSIONS] = { {0x0, 0x0}, {0x0, 0x0}, {0x0, 0x0}, {0x0, 0x0}, {0x3342, 0x3a32 } },
    LayoutTilesOffset[D2_VERSIONS] = { {0x3342, 0x1920}, {0x3342, 0x28d4}, {0x3342, 0x2296}, {0x3342, 0x2db2}, {0x3342, 0x2c58 } },    
    AngleTableOffset[D2_VERSIONS] = { {0,0}, {0,0}, {0,0}, {0x3348, 0x23da}, {0x3342, 0x23ce} },
    MapMoveModOffset[D2_VERSIONS] = { {0,0}, {0,0}, {0,0}, {0,0}, {0x3342, 0x3776} },
    AnimPtrsOffset[D2_VERSIONS] = { {0,0}, {0,0}, {0,0}, {0,0}, {0x3342, 0x3206} },
    MapOffsetIndexesOffset[D2_VERSIONS] = { {0,0}, {0,0}, {0,0}, {0,0}, {0x3342, 0x2006} },
    MovementUnk1Offset[D2_VERSIONS] = { {0,0}, {0,0}, {0,0}, {0,0}, {0x3342, 0x2468} },
    Emc15CDAOffset[D2_VERSIONS] = { {0,0}, {0,0}, {0,0}, {0,0}, {0x0001, 0x5cca} },
    GlobalDataOffset[D2_VERSIONS] GNUC_ATTRIBUTE(unused) = { {0x3251, 0}, {0x332f, 0}, {0x32f0, 0}, {0x3348, 0}, {0x3342, 0 } };

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
    name(""),fileSize(0),buffer(0),filePosition(0),parentIndex(0),field_11(0),fileType(0)
    {}

MovementTile::MovementTile() :
    field_0(0), field_2(0), field_4(6), onRock(false), field_12(0),
    leaveUnitDecay(false), field_16(0), field_18(0), field_20(0), color(0),
    field_24(0), field_26(0)
    {}

Dune2File::Dune2File(ExeFile &stream) :
    _stream(stream), _version(D2_VERSIONS),
    _structureData(19),
    _unitData(27),
    _houseData(6),
    _actionData(14),
    _fileData(0),
    _fileParents(),
    _movementTiles(15),
    _movementNames(6),
    _teamActionNames(5),
    _layoutTiles(7,std::vector<uint16_t>(9)),
    _layoutTilesUnk1(7, std::vector<int16_t>(8)),
    _layoutTileCount(7),
    _layoutTilesAround(7, std::vector<int16_t>(16)),
    _layoutSize(7),
    _layoutTileDiff(7),
    _angleTable(205),
    _mapMoveMod(8),
    _mapMod(8),
    _mapOffsetIndexes(21),
    _mapOffsets(336),
    _anims(20, std::vector<uint16_t>(8)),
    _structureAnims(29),
    _unitAnims(24, std::vector<uint16_t>(8)),
    _unitAngleFrameAdjust(5, std::vector<UPoint>(8)),
    _unitFrameAdjust(8),
    _unitTurretFrame(2, std::vector<SPoint>(8)),
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
	    _version = static_cast<Version>(i);
	    break;
	}
    }
}

void Dune2File::readDataStructures()
{
    _stream.seekSegOff(StructureOffset[_version].segment, StructureOffset[_version].offset);
    // We create a map that indexes pointers to the vectors by using the cs:ip pointers
    // used in the binary as keys. This way we can just look them up when encountered in
    // structures later and get the pointer to where they're (now) located.
    std::map<uint32_t, const std::vector<uint16_t>*> animPtrMap;
    // If cs:ip pointer is a NULL pointer, we'll provide a null pointer as well ;)
    animPtrMap[0] = nullptr;

    std::streampos pos;
    for(std::vector<std::vector<uint16_t> >::iterator it = _structureAnims.begin(); it != _structureAnims.end(); ++it != _structureAnims.end() && _stream.seekg(pos)) {
	uint32_t addr = _stream.getU32LE();
	animPtrMap.insert(make_pair(addr, &(*it)));
	it->resize((_stream.getU32LE() & ((1<<5)-1)) / sizeof(uint16_t));
    	pos = _stream.tellg();
	_stream.seekSegOff(addr);
	_stream.readU16LE(&it->front(), it->size());
    }

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
	(*it)->frameData[0]		= animPtrMap[_stream.getU32LE()];
	(*it)->frameData[1]		= animPtrMap[_stream.getU32LE()];
	(*it)->frameData[2]		= animPtrMap[_stream.getU32LE()];
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

    _stream.seekg(static_cast<uint32_t>(pos)+224);
    uint32_t addr = _stream.getU32LE();
    GNUC_ATTRIBUTE(unused) uint32_t size = _stream.getU32LE();
    _stream.seekSegOff(addr);

    for(std::vector<std::vector<uint16_t> >::iterator it = _unitAnims.begin(); it != _unitAnims.end(); addr += it->size()*sizeof(uint16_t), ++it) {
	animPtrMap.insert(make_pair(addr, &(*it)));
	_stream.readU16LE(&it->front(), it->size());
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
	(*it)->houseLetter		= _stream.getU16LE(),
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
	file->name			= stringGet(_stream.getU32LE());
	file->fileSize			= _stream.getU32LE();
	file->buffer			= _stream.getU32LE();
	file->filePosition		= _stream.getU32LE();
	file->parentIndex		= _stream.get();
	file->field_11			= _stream.get();
	file->fileType			= _stream.get();
	_fileData.push_back(file);
	if(file->fileType & 0x10) {
	    _fileParents[file->name] = _fileData[file->parentIndex];
	}
    }

    _stream.seekSegOff(LayoutTilesOffset[_version].segment, LayoutTilesOffset[_version].offset);    
    for(std::vector<std::vector<uint16_t> >::iterator x = _layoutTiles.begin(); x != _layoutTiles.end(); ++x) {
	_stream.readU16LE(&x->front(), x->size());
    }
    for(std::vector<std::vector<int16_t> >::iterator x = _layoutTilesUnk1.begin(); x != _layoutTilesUnk1.end(); ++x) {
	_stream.readU16LE(reinterpret_cast<uint16_t*>(&x->front()), x->size());
    }
    _stream.readU16LE(&_layoutTileCount.front(), _layoutTileCount.size());
    for(std::vector<std::vector<int16_t> >::iterator x = _layoutTilesAround.begin(); x != _layoutTilesAround.end(); ++x) {
	_stream.readU16LE(reinterpret_cast<uint16_t*>(&x->front()), x->size());
    }
    for(std::vector<UPoint>::iterator it = _layoutSize.begin(); it != _layoutSize.end(); ++it) {
	it->x = _stream.getU16LE(),
	it->y = _stream.getU16LE();
    }
    for(std::vector<UPoint>::iterator it = _layoutTileDiff.begin(); it != _layoutTileDiff.end(); ++it) {
	it->x = _stream.getU16LE(),
	it->y = _stream.getU16LE();
    }

    _stream.seekSegOff(AngleTableOffset[_version].segment, AngleTableOffset[_version].offset);
    _stream.readU16LE(reinterpret_cast<uint16_t*>(&_angleTable.front()), _angleTable.size());

    _stream.seekSegOff(MapMoveModOffset[_version].segment, MapMoveModOffset[_version].offset);
    _stream.readU16LE(&_mapMoveMod.front(), _mapMoveMod.size());
    _stream.read(reinterpret_cast<char*>(&_mapMod.front()), _mapMod.size());

    _stream.seekSegOff(AnimPtrsOffset[_version].segment, AnimPtrsOffset[_version].offset);
    for(std::vector<std::vector<uint16_t> >::iterator x = _anims.begin(); x != _anims.end(); ++x) {
	uint32_t addr = _stream.getU32LE();
	std::streampos pos(_stream.tellg());
	_stream.seekSegOff(addr);
	_stream.readU16LE(&x->front(), x->size());
	_stream.seekg(pos);
    }

    // dunno what these are...
    _stream.ignore(66);
    for(std::vector<std::vector<UPoint> >::iterator row = _unitAngleFrameAdjust.begin(); row != _unitAngleFrameAdjust.end(); ++row)
	for(std::vector<UPoint>::iterator col = row->begin(); col != row->end(); ++col)
    	    col->x = _stream.getU16LE(), col->y = _stream.getU16LE();

    // Terminator and/or alignment padding?
    _stream.ignore(6);
    _stream.read(reinterpret_cast<char*>(&_unitFrameAdjust.front()), _unitFrameAdjust.size());

    // dunno what these are...
    _stream.ignore(28);
    for(std::vector<std::vector<SPoint> >::iterator row = _unitTurretFrame.begin(); row != _unitTurretFrame.end(); ++row)
	for(std::vector<SPoint>::iterator col = row->begin(); col != row->end(); ++col)
    	    col->x = _stream.getU16LE(), col->y = _stream.getU16LE();

    _stream.seekSegOff(MapOffsetIndexesOffset[_version].segment, MapOffsetIndexesOffset[_version].offset);
    _stream.read(reinterpret_cast<char*>(&_mapOffsetIndexes.front()), _mapOffsetIndexes.size());    
    _stream.read(reinterpret_cast<char*>(&_mapOffsets.front()), _mapOffsets.size());

    _stream.seekSegOff(MovementUnk1Offset[_version].segment, MovementUnk1Offset[_version].offset);
    _stream.readU16LE(reinterpret_cast<uint16_t*>(&_movementUnk1.front()), _movementUnk1.size());    

    _stream.ignore(36);
    _stream.readU16LE(&_mapTileColors.front(), _mapTileColors.size());

    _stream.seekSegOff(MovementOffset[_version].segment, MovementOffset[_version].offset);
    for(std::vector<MovementTile>::iterator it = _movementTiles.begin(); it != _movementTiles.end(); ++it) {
	it->field_0		= _stream.getU16LE();
	it->field_2		= _stream.getU16LE();
	_stream.read(reinterpret_cast<char*>(&it->field_4.front()), it->field_4.size());
	it->onRock		= _stream.getU16LE();
	it->field_12		= _stream.getU16LE();
	it->leaveUnitDecay	= _stream.getU16LE();
	it->field_16		= _stream.getU16LE();
	it->field_18		= _stream.getU16LE();
	it->field_20		= _stream.getU16LE();
	it->color		= _stream.getU16LE();
	it->field_24		= _stream.getU16LE();
	it->field_26		= _stream.getU16LE();
    }

    for(std::vector<std::string>::iterator it = _movementNames.begin(); it != _movementNames.end(); ++it) {
	*it += stringGet(_stream.getU32LE());
    }
    for(std::vector<std::string>::iterator it = _teamActionNames.begin(); it != _teamActionNames.end(); ++it) {
	*it += stringGet(_stream.getU32LE());
    }

    for(std::vector<MapInfo>::iterator map = _mapScales.begin(); map != _mapScales.end(); ++map) {
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

const File& Dune2File::getFileParent(std::string name)
{
    std::transform(name.begin(), name.end(), name.begin(), ::toupper);
    return _fileParents[name];
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
