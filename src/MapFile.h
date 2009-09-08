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
	uint16_t size() {
	    return _tileSet.size();
	};
	std::vector<uint16_t>::const_iterator getTiles(uint16_t index) {
	    return _tileSet[index].begin();
	}

    private:
	std::istream &_stream;
	std::vector<std::vector<uint16_t> > _tileSet;
};

}
#endif // EASTWOOD_MAPFILE_H

