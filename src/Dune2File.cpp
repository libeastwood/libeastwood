#include "StdDef.h"
#include <iostream>

#include "Dune2File.h"

namespace eastwood {

struct Address {
    uint16_t segment;
    uint16_t offset;
};

static const Address
    D2ExeVersionOffset[D2_VERSIONS] = { {0x325f, 0xe}, {0x333a, 0x2}, {0x32fb, 0x2}, {0x3353, 0x2}, {0x334d, 0x2} },
    D2ExeStructureOffset[D2_VERSIONS] = { {0x2c2f, 0xa}, {0x2b3d, 0xa}, {0x2a98, 0xa}, {0x2a9d, 0xa}, {0x2a98, 0xa} },
    D2ExeUnitOffset[D2_VERSIONS] = { {0x2ca7, 0x0}, {0x2bb5, 0x0}, {0x2b0b, 0x0}, {0x2b10, 0x0}, {0x2b0b, 0x0} },

    D2ExeHouseOffset[D2_VERSIONS] = { {0x3574, 0xa}, {0x3668, 0x4}, {0x3615, 0xc}, {0x36c7, 0x8}, {0x36c0, 0xc} },
    D2ExeFileOffset[D2_VERSIONS] = { {0x2e28, 0x0}, {0x2d1b, 0x0}, {0x2ca0, 0x0}, {0x2ca5, 0x0}, {0x2ca0, 0x0} },

    D2ExeActionOffset[D2_VERSIONS] = { {0x2e1c, 0xe}, {0x2d0f, 0xe}, {0x2c94, 0xe}, {0x2c99, 0xe}, {0x2c94, 0xe} };

Dune2File::Dune2File(ExeFile &stream) :
    _stream(stream), _version(D2_VERSIONS),
    _structureData(std::vector<D2ExeStructureData>(D2ExeStructureEntries)),
    _unitData(std::vector<D2ExeUnitData>(D2ExeUnitEntries)),
    _houseData(std::vector<D2ExeHouseData>(D2ExeHouseEntries)),
    _actionData(std::vector<D2ExeActionData>(D2ExeActionEntries)),
    _fileData(std::vector<D2ExeFileData>(D2ExeFileEntries))
{
    detectDune2Version();
    readDataStructures();
}

void Dune2File::detectDune2Version()
{
    char tmp[10];
    for(int i = D2_DEMO; i < D2_VERSIONS; i++) {
    	_stream.seekSegOff(D2ExeVersionOffset[i].segment, D2ExeVersionOffset[i].offset);
	_stream.read(tmp, sizeof(tmp));

	if(strncmp(tmp, "DUNE2.EXE", sizeof(tmp)) == 0) {
	    _version = (D2ExeVersion)i;
	    break;
	}
    }
}

void Dune2File::readDataStructures()
{
    _stream.seekSegOff(D2ExeStructureOffset[_version].segment, D2ExeStructureOffset[_version].offset);
    //FIXME: Need to fix endianness...
    _stream.read((char*)&_structureData.front(), _structureData.size() * sizeof(_structureData[0]));

    _stream.seekSegOff(D2ExeUnitOffset[_version].segment, D2ExeUnitOffset[_version].offset);
    _stream.read((char*)&_unitData.front(), _unitData.size() * sizeof(_unitData[0]));

    _stream.seekSegOff(D2ExeHouseOffset[_version].segment, D2ExeHouseOffset[_version].offset);
    _stream.read((char*)&_houseData.front(), _houseData.size() * sizeof(_houseData[0]));

    _stream.seekSegOff(D2ExeFileOffset[_version].segment, D2ExeFileOffset[_version].offset);
    _stream.read((char*)&_fileData.front(), _fileData.size() * sizeof(_fileData[0]));

    _stream.seekSegOff(D2ExeActionOffset[_version].segment, D2ExeActionOffset[_version].offset);
    _stream.read((char*)&_actionData.front(), _actionData.size()  * sizeof(_actionData[0]));


}

std::string Dune2File::stringGet(uint32_t p)
{
    std::string str(64,0);
    _stream.seekSegOff(p);

    _stream.getline((char*)str.data(), str.size(), 0);
    return str;
}

}
