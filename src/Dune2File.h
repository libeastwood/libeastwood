#ifndef EASTWOOD_DUNE2FILE_H
#define EASTWOOD_DUNE2FILE_H

/*
 * These are data structures used in the DUNE2.EXE binary and the file offsets
 * where they start from.
 * Acquired from Nyergud's editor & Olaf van der Spek's XCC Utils.
 */

#include <istream>

namespace eastwood {

enum Dune2Version {
    D2_DEMO,
    D2_V1_00,
    D2_V1_07_US,
    D2_V1_07_EU,
    D2_V1_07_HS,
    D2_V1_00_RU, // TODO
    D2_VERSIONS
};
off_t versionOffset[D2_VERSIONS] = { 225278, 229282, 228274, 229682, 229586, 0 };

off_t buildingOffset[D2_VERSIONS] = { 199930, 196570, 193930, 194010, 193930, 0 };
struct buildingType {
    uint16_t	shortNameID;
    uint16_t	nameCodeRef;
    uint32_t	longNameID;
    uint32_t	wsaFileCodeRef;
    uint8_t	u005b1:1;
    uint8_t	constructionScreen:2;
    uint8_t	u005b3:3;
    uint8_t	noConreteRequired:4;
    uint8_t	u005b4:5;
    uint8_t	u005b5:6;
    uint8_t	u005b6:7;
    uint8_t	canBeCaptured:8;
    uint8_t	u005b;
    uint16_t	infantrySpawnCells;
    uint16_t	hitPoints;
    uint16_t	sight;
    uint16_t	sidebarIconGfxID;
    uint16_t	cost;
    uint16_t	buildTime;
    uint16_t	techLevel;
    uint32_t	prerequisities;
    uint8_t	buildOrder;
    uint8_t	cyUpgradesNeeded;
    uint8_t	u016;
    uint8_t	u017;
    uint8_t	u018;
    uint8_t	u019;
    uint8_t	u020;
    uint8_t	u021;
    uint8_t	u022;
    uint8_t	u023;
    uint8_t	u024;
    uint16_t	structureID;
    uint16_t	weaponDamage;
    uint16_t	threatLevelForAI;
    uint32_t	owner;
    uint16_t	repairFacility;
    uint16_t	unitsCanEnter;
    uint16_t	spiceStorage;
    uint16_t	powerConsumed;
    uint16_t	foundationSize;
    uint16_t	structureGfxID;
    uint16_t	u035;
    uint16_t	u036;
    uint16_t	u037;
    uint16_t	u038;
    uint16_t	u039;
    uint16_t	u040;
    uint16_t	constructionOptn1;
    uint16_t	constructionOptn2;
    uint16_t	constructionOptn3;
    uint16_t	constructionOptn4;
    uint16_t	constructionOptn5;
    uint16_t	constructionOptn6;
    uint16_t	constructionOptn7;
    uint16_t	constructionOptn8;
    uint16_t	upgradeTechLvl1;	
    uint16_t	upgradeTechLvl2;	
    uint16_t	upgradeTechLvl3;	
};

off_t unitOffset[D2_VERSIONS] = { 201840, 198480, 195760, 195840, 195760, 0 };
struct unitType {
    uint16_t	shortNameID;
    uint16_t	nameCodeRef;
    uint32_t	longNameID;
    uint32_t	wsaFileCodeRef;
    uint8_t	airUnitShadow:1;
    uint8_t	u005b2:2;
    uint8_t	u005b3:3;
    uint8_t	u005b4:4;
    uint8_t	u005b5:5;
    uint8_t	wormCamouflage:6;
    uint8_t	turretRotates:7;
    uint8_t	u005b8:8;
    uint8_t	pickupForRepairs:1;
    uint8_t	u006b2:2;
    uint8_t	u006b3:3;
    uint8_t	u006b4:4;
    uint8_t	u006b5:5;
    uint8_t	u006b6:6;
    uint8_t	u006b7:7;
    uint8_t	u006b8:8;
    uint16_t	u007a;
    uint16_t	hitPoints;
    uint16_t	sight;
    uint16_t	sidebarIconGfxID;
    uint16_t	cost;
    uint16_t	buildTime;
    uint16_t	techLevel;
    uint32_t	prerequisities;
    uint8_t	u015;
    uint8_t	upgradesNeeded;
    uint16_t	sidebarCommand1;
    uint16_t	sidebarCommand2;
    uint16_t	sidebarCommand3;
    uint16_t	sidebarCommand4;
    uint16_t	u021;
    uint8_t	u022;
    uint16_t	u023;
    uint16_t	threatLevelForAI;
    uint32_t	owner;
    uint16_t	unitArrayRangeMin;
    uint16_t	unitArrayRangeMax;
    uint8_t	u028:1;
    uint8_t	explodeOnTarget:2;
    uint8_t	explodeWhenDying:3;
    uint8_t	sonicImmunity:4;
    uint8_t	bumpyMovement:5;
    uint8_t	trackedCrushing:6;
    uint8_t	hasUnderControl:7;
    uint8_t	stayOnMap:8;
    uint8_t	u029b1:1;
    uint8_t	u029b2:2;
    uint8_t	firesTwice:3;
    uint8_t	u029b4:4;
    uint8_t	u029b5:5;
    uint8_t	u029b6:6;
    uint8_t	u029b7:7;
    uint8_t	notAWeaponType:8;
    uint8_t	u030;
    uint8_t	u031;
    uint16_t	u032;
    uint16_t	movementType;
    uint16_t	u034;
    uint16_t	speed;
    uint16_t	turningSpeed;
    uint16_t	unitGfxID;
    uint16_t	turretGfxID;
    uint16_t	u039;
    uint16_t	u040;
    uint16_t	deathAnimation;
    uint16_t	weaponFireRate;
    uint16_t	weaponRange;
    uint16_t	weaponDamage;
    uint16_t	u045;
    uint16_t	weaponType;
    uint16_t	weaponSound;
};

Dune2Version detectDune2Version(std::istream &stream);

}
#endif // EASTWOOD_DUNE2FILE_H
