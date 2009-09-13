#ifndef EASTWOOD_MAPFILE_H
#define EASTWOOD_MAPFILE_H

#include <vector>
#include <istream>

namespace eastwood {

class MapFile
{
    public:
	MapFile(std::istream &stream);
	virtual ~MapFile();

	/*!
	  Returns the number of tileSets in the map-File.
	  @return	Number of tileSets
	  */
	inline uint16_t size() {
	    return _tileSet.size();
	};

	inline std::vector<uint16_t>::const_iterator getTilesBegin(uint16_t index) {
	    return _tileSet[index].begin();
	}

	inline std::vector<uint16_t>::const_iterator getTilesEnd(uint16_t index) {
	    return _tileSet[index].end();
	}


    private:
	std::istream &_stream;
	std::vector<std::vector<uint16_t> > _tileSet;
};

}
#endif // EASTWOOD_MAPFILE_H

