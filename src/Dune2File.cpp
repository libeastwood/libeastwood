#include <fstream>
#include "StdDef.h"

#include "Dune2File.h"

namespace eastwood {

Dune2File::Dune2File(std::istream &stream) :
    _stream(stream), _version(D2_VERSIONS),
    _structureData(std::vector<D2ExeStructureData>(D2ExeStructureNum)),
    _unitData(std::vector<D2ExeUnitData>(D2ExeUnitNum)),
    _houseData(std::vector<D2ExeHouseData>(D2ExeHouseNum)),
    _actionData(std::vector<D2ExeActionData>(D2ExeActionNum)),
    _fileData(std::vector<D2ExeFileData>(D2ExeFileNum))
{
    detectDune2Version();
    readDataStructures();
}

void Dune2File::detectDune2Version()
{
    int i;
    for(i = D2_DEMO; i < D2_VERSIONS; i++) {
	_stream.seekg(D2ExeVersionOffset[i], std::ios::beg);
	uint8_t tmp[4];
	readLE(_stream, tmp, sizeof(tmp));
	if(memcmp(tmp, "DUNE", sizeof(tmp)) == 0)
	    break;
    }
    _version = (D2ExeVersion)i;
}

void Dune2File::readDataStructures()
{
    _stream.seekg(D2ExeStructureOffset[_version], std::ios::beg);
    readLE(_stream, (uint8_t*)&_structureData.front(), sizeof(D2ExeStructureData)*D2ExeStructureNum);
    _stream.seekg(D2ExeStructureOffset[_version], std::ios::beg);
    readLE(_stream, (uint8_t*)&_unitData.front(), sizeof(D2ExeUnitData)*D2ExeUnitNum);
    _stream.seekg(D2ExeFileOffset[_version], std::ios::beg);
    readLE(_stream, (uint8_t*)&_houseData.front(), sizeof(D2ExeHouseData)*D2ExeHouseNum);
}

}
