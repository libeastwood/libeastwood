#ifndef EASTWOOD_DUNE2FILE_H
#define EASTWOOD_DUNE2FILE_H

/*
 * These are data structures used in the DUNE2.EXE binary and the file offsets
 * where they start from.
 * Acquired from Nyergud's editor, Olaf van der Spek's XCC Utils & segra.
 */

#include <vector>

#include "eastwood/StdDef.h"
#include "eastwood/ExeFile.h"

namespace eastwood {

enum	D2ExeVersion {
    D2_DEMO,
    D2_V1_00,
    D2_V1_07_US,
    D2_V1_07_EU,
    D2_V1_07_HS,
    D2_VERSIONS
};

struct Address {
    uint16_t			segment;
    uint16_t			offset;
};

struct	D2ExeObjectData {
    D2ExeObjectData();
    virtual ~D2ExeObjectData() {}
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

struct	D2ExeStructureData : D2ExeObjectData {
    D2ExeStructureData();
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
    std::vector<uint32_t>	frameData;
    std::vector<uint16_t>	constructOpt;
    std::vector<uint16_t>	techUpgrade;
};

struct	D2ExeUnitData : D2ExeObjectData {
    D2ExeUnitData();
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

struct	D2ExeHouseData {
    D2ExeHouseData();
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

// Stored internal File table
struct	D2ExeFileData {
    D2ExeFileData();
    std::string			name;
    uint16_t			field_4;
    uint16_t			field_6;
    uint16_t			field_8;
    uint16_t			field_A;
    uint16_t			field_C;
    uint16_t			field_E;
    uint8_t			parentIndex;
    uint8_t			field_11;
    uint8_t			fileType;
};

// Unit 'Action' commands
struct	D2ExeActionData {
    D2ExeActionData();
    uint16_t			field_0;
    std::string			name;
    uint16_t			interruptAction;
    uint16_t			sidebarMode;
    uint16_t			responseSound;
};

class Dune2File
{
    public:
	Dune2File(ExeFile &stream);

	D2ExeVersion getVersion() const throw() { return _version; }
	const std::vector<D2ExeStructureData> getStructureData() const { return _structureData; }
	const std::vector<D2ExeUnitData> getUnitData() const { return _unitData; }
	const std::vector<D2ExeHouseData> getHouseData() const { return _houseData; }
	const std::vector<D2ExeFileData> getFileData() const { return _fileData; }
	const std::vector<D2ExeActionData> getActionData() const { return _actionData; }
	const std::vector<std::string> getMovementData() const { return _movementData; }
	const std::vector<uint16_t> getLayoutTileCount() const { return _layoutTileCount; }
	const std::vector<std::vector<int16_t> > getLayoutTilesAround() const { return _layoutTilesAround; }
	const std::vector<int16_t> getAngleTable() const { return _angleTable; }
	const std::vector<uint16_t> getMapMoveMod() const { return _mapMoveMod; }
	const std::vector<int8_t> getMapMod() const { return _mapMod; }
	const std::vector<uint32_t> getAnimPtrs() const { return _animPtrs; }
	const std::vector<int16_t> getUnitAngleFrameAdjust() const { return _unitAngleFrameAdjust; }
	const std::vector<int8_t> getFrameAdjust() const { return _unitFrameAdjust; }
	const std::vector<int16_t> getUnitSiegeTurretFrame() const { return _unitSiegeTurretFrameAdjust; }

	std::vector<uint16_t> animPtrGet(uint32_t p);

	std::string stringGet(uint32_t p);

    private:
	void detectDune2Version();
	template <typename T>
	void readData(std::vector<T> &data, const Address *offsets);
	void readDataStructures();

	ExeFile &_stream;
	D2ExeVersion _version;
	std::vector<D2ExeStructureData> _structureData;
	std::vector<D2ExeUnitData> _unitData;
	std::vector<D2ExeHouseData> _houseData;
	std::vector<D2ExeActionData> _actionData;
	std::vector<D2ExeFileData> _fileData;
	std::vector<std::string> _movementData;
	std::vector<uint16_t> _layoutTileCount;
	std::vector<std::vector<int16_t> > _layoutTilesAround;
	std::vector<int16_t> _angleTable;
	std::vector<uint16_t> _mapMoveMod;
	std::vector<int8_t> _mapMod;
	std::vector<uint32_t> _animPtrs;
	std::vector<int16_t> _unitAngleFrameAdjust;
	std::vector<int8_t> _unitFrameAdjust;
	std::vector<int16_t> _unitSiegeTurretFrameAdjust;

};

}

#endif // EASTWOOD_DUNE2FILE_H
