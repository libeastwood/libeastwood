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

	std::vector<uint16_t>& operator[] (uint16_t i) { return (*_tileSet)[i]; }

	/*!
	  Returns the number of tileSets in the map-File.
	  @return	Number of tileSets
	  */
	inline uint16_t size() {
	    return _tileSet->size();
	};

    private:
	std::istream &_stream;
	std::vector<std::vector<uint16_t> > *_tileSet;
};

}
#endif // EASTWOOD_MAPFILE_H

