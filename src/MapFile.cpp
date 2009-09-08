#include "StdDef.h"

#include "Exception.h"
#include "MapFile.h"


namespace eastwood {

MapFile::MapFile(std::istream &stream) :
    _stream(stream), _tileSet(std::vector<std::vector<uint16_t> >(0))
{
    size_t mapSize = getStreamSize(_stream);
    uint16_t numTileSets = readU16LE(_stream);

    _tileSet = std::vector<std::vector<uint16_t> >(numTileSets);

    for(uint16_t i = 0, pre = numTileSets, cur = 0; i < numTileSets; i++, pre=cur) {
	uint16_t size = (cur= (i == numTileSets-1) ? mapSize/2 : readU16LE(_stream)) - pre;
	_tileSet[i] = std::vector<uint16_t>(size);
    }

    for(std::vector<std::vector<uint16_t> >::iterator it = _tileSet.begin();
	    it != _tileSet.end(); it++) {
	for(std::vector<uint16_t>::iterator it2 = it->begin();
		it2 != it->end(); it2++)
	    *it2 = readU16LE(_stream);
    }
}

MapFile::~MapFile()
{
}

}

