#ifndef EASTWOOD_MAPFILE_H
#define EASTWOOD_MAPFILE_H

/*!
    A class for loading MAP files.
    
	A map file contains information on how to assemble a complete picture or
	animation sequence from several tiles.
*/

#include <vector>

#include "eastwood/IStream.h"

// Some very nice documentation on this format, or more specifically the
// ICON.MAP that comes with Dune 2 can be found at:
// http://www.junkyard.dk/icon_map.asp

namespace eastwood {

class MapFile
{
    public:
	MapFile(std::istream &stream);
	~MapFile();

	/*!
	 Get a tile set.
	 @return	A vector containing tile indexes.
	 */
	const std::vector<uint16_t>& operator[] (uint16_t i) const {
	    return _tileSet.at(i);
	}

	/*!
	  Returns the number of tileSets in the map-File.
	  @return	Number of tileSets
	  */
	uint16_t size() const throw() {
	    return _tileSet.size();
	}

    private:
	IStream &_stream;
	std::vector<std::vector<uint16_t> > _tileSet;
};

}
#endif // EASTWOOD_MAPFILE_H

