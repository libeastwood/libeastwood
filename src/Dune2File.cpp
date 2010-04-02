#include "eastwood/StdDef.h"

#include "eastwood/Dune2File.h"

namespace eastwood {

static const Address
    D2ExeVersionOffset[D2_VERSIONS] = { {0x325f, 0xe}, {0x333a, 0x2}, {0x32fb, 0x2}, {0x3353, 0x2}, {0x334d, 0x2} },
    D2ExeStructureOffset[D2_VERSIONS] = { {0x2c2f, 0xa}, {0x2b3d, 0xa}, {0x2a98, 0xa}, {0x2a9d, 0xa}, {0x2a98, 0xa} },
    D2ExeUnitOffset[D2_VERSIONS] = { {0x2ca7, 0x0}, {0x2bb5, 0x0}, {0x2b0b, 0x0}, {0x2b10, 0x0}, {0x2b0b, 0x0} },
    D2ExeHouseOffset[D2_VERSIONS] = { {0x3574, 0xa}, {0x3668, 0x4}, {0x3615, 0xc}, {0x36c7, 0x8}, {0x36c0, 0xc} },
    D2ExeFileOffset[D2_VERSIONS] = { {0x2e28, 0x0}, {0x2d1b, 0x0}, {0x2ca0, 0x0}, {0x2ca5, 0x0}, {0x2ca0, 0x0} },
    D2ExeActionOffset[D2_VERSIONS] = { {0x2e1c, 0xe}, {0x2d0f, 0xe}, {0x2c94, 0xe}, {0x2c99, 0xe}, {0x2c94, 0xe} },
    D2ExeMovementOffset[D2_VERSIONS] = { {0x3342, 0x2d70}, {0x3342, 0x3caa}, {0x3342, 0x3786}, {0x3342, 0x429e}, {0x3342, 0x4232 } };

Dune2File::Dune2File(ExeFile &stream) :
    _stream(stream), _version(D2_VERSIONS),
    _structureData(D2ExeStructureEntries),
    _unitData(D2ExeUnitEntries),
    _houseData(D2ExeHouseEntries),
    _actionData(D2ExeActionEntries),
    _fileData(0),
    _movementData(D2ExeMovementEntries)    
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

template <typename T>
void Dune2File::readData(std::vector<T> &data, const Address *offsets)
{
    T empty;
    memset(&empty, 0, sizeof(T));

    _stream.seekSegOff(offsets[_version].segment, offsets[_version].offset);

    while(_stream.good()) {
	data.resize(data.size()+1);
	_stream.read((char*)&data.back(), sizeof(T));
	if(memcmp(&data.back(), &empty, sizeof(T)) == 0) {
	    data.pop_back();
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

    readData<D2ExeFileData>(_fileData, D2ExeFileOffset);

    _stream.seekSegOff(D2ExeActionOffset[_version].segment, D2ExeActionOffset[_version].offset);
    _stream.read((char*)&_actionData.front(), _actionData.size()  * sizeof(_actionData[0]));

    _stream.seekSegOff(D2ExeMovementOffset[_version].segment, D2ExeMovementOffset[_version].offset);
    char buf[16];
    for(std::vector<std::string>::iterator it = _movementData.begin(); it != _movementData.end(); ++it) {
    	_stream.getline(buf, sizeof(buf), 0);
	*it += buf;
    }

}

std::vector<uint16_t> Dune2File::animPtrGet(uint32_t p) {
    _stream.seekSegOff(p);
    // one 64K (32K*2) segment
    // TODO: 64K is more of a max value and probably way more than we actually need...
    std::vector<uint16_t> ret(1<<15);
    _stream.readU16LE(&ret.front(), ret.size());
    return ret;
}

std::string Dune2File::stringGet(uint32_t p)
{
    char buf[64];
    _stream.seekSegOff(p);

    _stream.getline(buf, sizeof(buf), 0);
    return std::string(buf);
}

}
