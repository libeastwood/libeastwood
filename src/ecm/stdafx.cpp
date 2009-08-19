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
 * $Id: stdafx.cpp 21 2009-04-11 01:11:32Z segra $
 * 
 */

#include "stdafx.h"

const char *version = "1.1";

// Object Names
const char *nameHouses[0x06] = {
	"Harkonnen",
	"Atreides",
	"Ordos",
	"Fremen",
	"Sardaukar",
	"Mercenary"
};

const char *nameStructures[0x13] = {
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

const char *nameUnits[0x1B] = {
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

// Unit GetDetail Function DetailNames
const char *nameUnitDetails[0x14] = {
	"HitPointRepairCalc?",   
	"BuildingTypeIndex",     
	"WeaponRange",           
	"IndexGet",              
	"Field64Get",            
	"AttackObjectIndexTypeGet",                   
	"Return",                  
	"TypeIndex",             
	"IndexGetAsObject",      
	"Field6BGet",                 
	"loc_27562",          
	"Field49AndField4BGet",                 
	"WeaponCoolDown?",                 
	"Field36And4h",               
	"HouseIDGet",            
	"loc_275C1",                 
	"TurretGet?",     
	"loc_2752F",                 
	"DoesTurretRotate",          
	"CheckIfHuman"              
};
