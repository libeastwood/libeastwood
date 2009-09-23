#ifndef EASTWOOD_MAPFILE_H
#define EASTWOOD_MAPFILE_H

#include <vector>

#include "eastwood/IStream.h"

namespace eastwood {

class MapFile
{
    public:
	MapFile(const std::istream &stream);
	virtual ~MapFile();

	std::vector<uint16_t>& operator[] (uint16_t i) { return _tileSet->at(i); }

	/*!
	  Returns the number of tileSets in the map-File.
	  @return	Number of tileSets
	  */
	inline uint16_t size() {
	    return _tileSet->size();
	};

    private:
	IStream &_stream;
	std::vector<std::vector<uint16_t> > *_tileSet;
};

}
#endif // EASTWOOD_MAPFILE_H

