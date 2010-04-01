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
    uint16_t segment;
    uint16_t offset;
};

static const
int	D2ExeStructureEntries = 19,
	D2ExeUnitEntries = 27,
	D2ExeHouseEntries = 6,
	D2ExeActionEntries = 14;

PACK
struct	D2ExeStructureData {
    uint16_t	idShort;
    uint32_t	name;
    uint16_t	idLong;
    uint32_t	picture;
    uint16_t	buildOpts;
    uint16_t	infantrySpawn;
    uint16_t	hitPoints;
    uint16_t	sight;    
    uint16_t	sidebarIconID;
    uint16_t	cost;
    uint16_t	buildTime;
    uint16_t	techlevel;
    uint32_t	preReqs;
    uint8_t	buildOrder;
    uint8_t	cYUpgradesNeeded;
    uint8_t	field_22;
    uint8_t	field_23;
    uint8_t	field_24;
    uint8_t	field_25;
    uint8_t	field_26;
    uint8_t	field_27;
    uint8_t	field_28;
    uint8_t	field_29;
    uint8_t	field_2A;
    uint16_t	structureID;
    uint16_t	weaponDamage;
    uint16_t	weaponDamageAdd;
    uint8_t	owner;
    uint16_t	repairFacility;
    uint32_t	unitsCanEnter;
    uint16_t	spiceStorage;
    uint16_t	powerUsage;
    uint16_t	foundationSize;
    uint16_t	structureGfxID;
    uint32_t	field_3E;
    uint32_t	field_42;
    uint32_t	field_46;
    uint16_t	constructOpt1;
    uint16_t	constructOpt2;
    uint16_t	constructOpt3;
    uint16_t	constructOpt4;
    uint16_t	constructOpt5;
    uint16_t	constructOpt6;
    uint16_t	constructOpt7;
    uint16_t	constructOpt8;
    uint16_t	techUpgrade1;
    uint16_t	techUpgrade2;
    uint16_t	techUpgrade3;
} GNUC_ATTRIBUTE((packed));
PACK

PACK
struct	D2ExeUnitData {
   uint16_t	idShort;
   uint32_t	name;
   uint16_t	idLong;
   uint32_t	picture;
   uint16_t	options1;
   uint16_t	infantrySpawn; 
   uint16_t	hitPoints;
   uint16_t	sight; 
   uint16_t	sidebarIconID; 
   uint16_t	cost; 
   uint16_t	buildTime;
   uint16_t	techLevel;
   uint32_t	preReqs;
   uint8_t	buildOrder;
   uint8_t	upgradesNeeded;
   uint16_t	sidebarCommand1;
   uint16_t	sidebarCommand2;
   uint16_t	sidebarCommand3;
   uint16_t	sidebarCommand4;
   uint8_t	field_2A;        
   uint16_t	field_2B;        
   uint16_t	field_2D;        
   uint16_t	aggressivity;
   uint8_t	owner;
   uint16_t	indexMin;
   uint16_t	indexMax;
   uint16_t	optsFitW;
   uint8_t	field_38;
   uint8_t	field_39;
   uint16_t	field_3A;
   uint16_t	movementType;
   uint16_t	movementPerFrame;
   uint16_t	speed;
   uint16_t	turningSpeed;
   uint16_t	unitGfxID;
   uint16_t	turretGfxID;
   uint16_t	aiCommand;
   uint16_t	frameAngleMode;
   uint16_t	deathAnim;
   uint16_t	weaponRateFire;
   uint16_t	weaponRange;
   uint16_t	weaponDamage;
   uint16_t	field_54;
   uint16_t	weaponType;
   uint16_t	weaponSound;
} GNUC_ATTRIBUTE((packed));
PACK

PACK
struct	D2ExeHouseData {
   uint32_t	name;
   uint16_t	weakness;
   uint16_t	lemonFactor;
   uint16_t	buildingDecay;
   uint16_t	color;
   uint16_t	palaceUnitRecharge;
   uint16_t	frigateTime;
   uint16_t	houseLetter;
   uint16_t	palaceMode;
   uint16_t	missionWinMusic;
   uint16_t	missionLoseMusic;
   uint16_t	missionBriefMusic;
   uint32_t	houseVoice;
} GNUC_ATTRIBUTE((packed));
PACK

// Stored internal File table
PACK
struct	D2ExeFileData {
   uint32_t	name;
   uint16_t	field_4;
   uint16_t	field_6;
   uint16_t	field_8;
   uint16_t	field_A;
   uint16_t	field_C;
   uint16_t	field_E;
   uint8_t	parentIndex;
   uint8_t	field_11;
   uint8_t	fileType;
} GNUC_ATTRIBUTE((packed));
PACK

// Unit 'Action' commands
PACK
struct	D2ExeActionData {
   uint16_t	field_0;
   uint32_t	name;
   uint16_t	interruptAction;
   uint16_t	sidebarMode;
   uint16_t	responseSound;
} GNUC_ATTRIBUTE((packed));
PACK

class Dune2File
{
    public:
	Dune2File(ExeFile &stream);

	D2ExeVersion getVersion() const throw() { return _version; }
	D2ExeStructureData getStructureData(int index) const { return _structureData.at(index); }
	D2ExeUnitData getUnitData(int index) const { return _unitData.at(index); }
	D2ExeHouseData getHouseData(int index) const { return _houseData.at(index); }
	D2ExeFileData getFileData(int index) const { return _fileData.at(index); }
	D2ExeActionData getActionData(int index) const { return _actionData.at(index); }

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

};

}

#endif // EASTWOOD_DUNE2FILE_H
