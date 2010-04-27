#ifndef EASTWOOD_DUNE2FILE_H
#define EASTWOOD_DUNE2FILE_H

/*
 * These are data structures used in the DUNE2.EXE binary and the file offsets
 * where they start from.
 * Acquired from Nyergud's editor, Olaf van der Spek's XCC Utils & segra.
 */

#include <vector>
#include <map>

#include "eastwood/StdDef.h"
#include "eastwood/ExeFile.h"

namespace eastwood { namespace dune2 {

enum	Version {
    D2_DEMO,
    D2_V1_00,
    D2_V1_07_US,
    D2_V1_07_EU,
    D2_V1_07_HS,
    D2_VERSIONS
};

struct	ObjectData {
    ObjectData();
    virtual ~ObjectData() {}
    uint16_t			typeIndex;
    uint16_t			idShort;
    std::string			name;
    uint16_t			idLong;
    std::string			picture;
    uint16_t			options;
    uint16_t			infantrySpawn;
    uint16_t			hitPoints;
    uint16_t			sight;
    uint16_t			sidebarIconID;
    uint16_t			cost;
    uint16_t			buildTime;
    uint16_t			techLevel;
    uint32_t			preReqs;
    uint8_t			buildOrder;
    uint8_t			upgradesNeeded;
    uint8_t			owner;
    uint16_t			gfxID;
    uint16_t			weaponDamage;
};

struct	StructureData : ObjectData {
    StructureData();
    uint8_t			field_22;
    uint8_t			field_23;
    uint8_t			field_24;
    uint8_t			field_25;
    uint8_t			field_26;
    uint8_t			field_27;
    uint8_t			field_28;
    uint8_t			field_29;
    uint8_t			field_2A;
    uint16_t			structureID;
    uint16_t			weaponDamageAdd;
    uint32_t			unitsCanEnter;
    uint16_t			spiceStorage;
    uint16_t			powerUsage;
    uint16_t			foundationSize;
    std::vector<const std::vector<uint16_t>*>	frameData;
    std::vector<uint16_t>	constructOpt;
    std::vector<uint16_t>	techUpgrade;
};

struct	UnitData : ObjectData {
    UnitData();
    uint16_t			sidebarCommand1;
    uint16_t			sidebarCommand2;
    uint16_t			sidebarCommand3;
    uint16_t			sidebarCommand4;
    uint8_t			field_2A;
    uint16_t			field_2B;
    uint16_t			field_2D;
    uint16_t			aggressivity;
    uint16_t			indexMin;
    uint16_t			indexMax;
    uint16_t			optsFitW;
    uint8_t			field_38;
    uint8_t			field_39;
    uint16_t			field_3A;
    uint16_t			movementType;
    uint16_t			movementPerFrame;
    uint16_t			speed;
    uint16_t			turningSpeed;
    uint16_t			turretGfxID;
    uint16_t			aiCommand;
    uint16_t			frameAngleMode;
    uint16_t			deathAnim;
    uint16_t			weaponRateFire;
    uint16_t			weaponRange;
    uint16_t			field_54;
    uint16_t			weaponType;
    uint16_t			weaponSound;
};

struct	HouseData {
    HouseData();
    std::string			name;
    uint16_t			weakness;
    uint16_t			lemonFactor;
    uint16_t			buildingDecay;
    uint16_t			color;
    uint16_t			palaceUnitRecharge;
    uint16_t			frigateTime;
    char			houseLetter;
    uint16_t			palaceMode;
    uint16_t			missionWinMusic;
    uint16_t			missionLoseMusic;
    uint16_t			missionBriefMusic;
    std::string			houseVoice;
};

// Unit 'Action' commands
struct	ActionData {
    ActionData();
    uint16_t			field_0;
    std::string			name;
    uint16_t			interruptAction;
    uint16_t			sidebarMode;
    uint16_t			responseSound;
};

// Stored internal File table
struct	FileData {
    FileData();
    std::string			name;
    uint32_t			fileSize;
    uint32_t			buffer;
    uint32_t			filePosition;
    uint8_t			parentIndex;
    uint8_t			field_11;
    uint8_t			fileType;
};

template <typename T>
struct Point {
    Point(T x = 0, T y = 0) : x(x), y(y) {}
    virtual ~Point() {}
    T				x;
    T				y;
};

template <typename T>
struct Rect : Point<T> {
    Rect(T x = 0, T y = 0, T w = 0, T h = 0) : Point<T>(x,y), w(w), h(h) {}
    T				w;
    T				h;
};
typedef Point<uint16_t>				UPoint;
typedef Point<int16_t>				SPoint;

typedef std::tr1::shared_ptr<ObjectData>	Object;
typedef std::tr1::shared_ptr<UnitData>		Unit;
typedef std::tr1::shared_ptr<StructureData>	Structure;
typedef std::tr1::shared_ptr<ObjectData>	Object;
typedef std::tr1::shared_ptr<HouseData>		House;
typedef std::tr1::shared_ptr<ActionData>	Action;
typedef std::tr1::shared_ptr<FileData>		File;

class Dune2File
{
    public:
	Dune2File(ExeFile &stream);

	Version						getVersion() const throw() 	{ return _version; }
	const std::vector<Structure>&			getStructureData() const	{ return _structureData; }
	const std::vector<Unit>&			getUnitData() const		{ return _unitData; }
	const std::vector<House>&			getHouseData() const		{ return _houseData; }
	const std::vector<Action>&			getActionData() const		{ return _actionData; }
	const std::vector<File>&			getFileData() const		{ return _fileData; }
	const File&					getFileParent(std::string name);
	const std::vector<std::string>&			getMovementNames() const	{ return _movementNames; }
	const std::vector<std::string>&			getTeamActionNames() const	{ return _teamActionNames; }
	const std::vector<std::vector<uint16_t> >&	getLayoutTiles() const		{ return _layoutTiles; }
	const std::vector<std::vector<int16_t> >&	getLayoutTilesUnk1() const	{ return _layoutTilesUnk1; }
	const std::vector<uint16_t>&			getLayoutTileCount() const	{ return _layoutTileCount; }
	const std::vector<std::vector<int16_t> >&	getLayoutTilesAround() const	{ return _layoutTilesAround; }
	const std::vector<UPoint>&			getLayoutSize() const		{ return _layoutSize; }
	const std::vector<UPoint>&			getLayoutTileDiff() const	{ return _layoutTileDiff; }
	const std::vector<int16_t>&			getAngleTable() const		{ return _angleTable; }
	const std::vector<uint16_t>&			getMapMoveMod() const		{ return _mapMoveMod; }
	const std::vector<int8_t>&			getMapMod() const		{ return _mapMod; }
	const std::vector<int8_t>&			getMapOffsetIndexes() const	{ return _mapOffsetIndexes; }
	const std::vector<int8_t>&			getMapOffsets() const		{ return _mapOffsets; }
	const std::vector<std::vector<uint16_t> >&	getAnims() const		{ return _anims; }
	const std::vector<std::vector<uint16_t> >&	getStructureAnims() const	{ return _structureAnims; }	
	const std::vector<std::vector<uint16_t> >&	getUnitAnims() const		{ return _unitAnims; }
	const std::vector<UPoint>&			getUnitAngleFrameAdjust() const { return _unitAngleFrameAdjust; }
	const std::vector<int8_t>&			getUnitFrameAdjust() const	{ return _unitFrameAdjust; }
	const std::vector<int16_t>&			getUnitTurretFrame() const	{ return _unitTurretFrameAdjust; }
	const std::vector<int16_t>&			getMovementUnk1() const		{ return _movementUnk1; }
	const std::vector<uint16_t>&			getMapTileColors() const	{ return _mapTileColors; }
	const std::vector<Rect<uint16_t> >&		getMapScales() const		{ return _mapScales; }
	const std::vector<uint16_t>&			getActionsAI() const		{ return _actionsAI; }
	const std::vector<int8_t>&			getMapSinTable() const		{ return _mapSinTable; }
	const std::vector<int8_t>&			getMapCosTable() const		{ return _mapCosTable; }	
	const std::vector<uint8_t>&			getEmc15CDA() const		{ return _emc15CDA; }


    private:
	void detectDune2Version();
	void readDataStructures();
	std::string stringGet(uint32_t p);

	ExeFile					&_stream;
	Version					_version;
	std::vector<Structure>			_structureData;
	std::vector<Unit>			_unitData;
	std::vector<House>			_houseData;
	std::vector<Action>			_actionData;
	std::vector<File>			_fileData;
	std::map<std::string, File>		_fileParents;
	std::vector<std::string>		_movementNames;
	std::vector<std::string>		_teamActionNames;
	std::vector<std::vector<uint16_t> >	_layoutTiles;
	std::vector<std::vector<int16_t> >	_layoutTilesUnk1;	
	std::vector<uint16_t>			_layoutTileCount;
	std::vector<std::vector<int16_t> >	_layoutTilesAround;
	std::vector<UPoint>			_layoutSize;
	std::vector<UPoint>			_layoutTileDiff;
	std::vector<int16_t>			_angleTable;
	std::vector<uint16_t>			_mapMoveMod;
	std::vector<int8_t>			_mapMod;
	std::vector<int8_t>			_mapOffsetIndexes;	
	std::vector<int8_t>			_mapOffsets;
	std::vector<std::vector<uint16_t> >	_anims;
	std::vector<std::vector<uint16_t> >	_structureAnims;	
	std::vector<std::vector<uint16_t> >	_unitAnims;
	std::vector<UPoint>			_unitAngleFrameAdjust;	
	std::vector<int8_t>			_unitFrameAdjust;
	std::vector<int16_t>			_unitTurretFrameAdjust;
	std::vector<int16_t>			_movementUnk1;
	std::vector<uint16_t>			_mapTileColors;
	std::vector<Rect<uint16_t> >		_mapScales;
	std::vector<uint16_t>			_actionsAI;
	std::vector<int8_t>			_mapSinTable;
	std::vector<int8_t>			_mapCosTable;
	std::vector<uint8_t>			_emc15CDA;

};

} } // namespace

#endif // EASTWOOD_DUNE2FILE_H
