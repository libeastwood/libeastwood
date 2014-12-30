#ifndef EASTWOOD_SHPFILE_H
#define EASTWOOD_SHPFILE_H

#include <istream>
#include <vector>

#include "eastwood/Decode.h"

namespace eastwood {

enum TileType {
    TILE_NORMAL = (1<<16),
    TILE_FLIPH = (1<<20),
    TILE_FLIPV = (1<<24),
    TILE_ROTATE = (1<<28)
};

struct ShpFileEntry
{
    uint32_t startOffset;
    uint32_t endOffset;
};

class ShpFile : public Decode
{
    public:
	ShpFile(std::istream &stream, Palette palette);
	~ShpFile();

	/*!
	  This method returns a SDL_Surface containing the nth picture in this shp-File.
	  The returned SDL_Surface should be freed with SDL_FreeSurface() if no longer needed.
	  @param	IndexOfFile	specifies which picture to return (zero based)
	  @return	nth picture in this shp-File
	  */
	Surface getSurface(const uint16_t fileIndex);


	/*!
	  This method returns a SDL_Surface containing an array of pictures from this shp-File.
	  All pictures must be of the same size. tilesX/tilesY specifies how many pictures are in this row/column.
	  Afterwards there must be tilesX*tilesY many parameters. Every parameter specifies which picture 
	  of this shp-File should be used. This indices must be ORed with a parameter specifing hwo they should
	  be in the result surface. There are 4 modes and you must OR exactly one:
	  - TILE_NORMAL	Normal
	  - TILE_FLIPH	mirrored horizontally
	  - TILE_FLIPV	mirrored vertically
	  - TILE_ROTATE	Rotated by 180 degress

	  Example:
	  @code
	  picture = myShpfile->getSurfaceArray(4,1, TILE_NORMAL | 20, TILE_FLIPH | 23, TILE_ROTATE | 67, TILE_NORMAL | 68);
	  @endcode
	  This example would create a surface with four pictures in it. From the left to the right there are
	  picture 20,23,67 and 68. picture 23 is mirrored horizontally, 67 is rotated.<br><br>
	  The returned SDL_Surface should be freed with SDL_FreeSurface() if no longer needed.
	  @param	tilesX	how many pictures in one row
	  @param	tilesY	how many pictures in one column
	  @return	picture in this shp-File containing all specified pictures
	  */
	Surface getSurfaceArray(const uint8_t tilesX, const uint8_t tilesY, ...);
	Surface getSurfaceArray(const uint8_t tilesX, const uint8_t tilesY, const uint32_t *tiles);

	auto size() const noexcept { return _size; }

    private:
	void readIndex();

	std::vector<ShpFileEntry> _index;
	uint16_t _size;
};

}
#endif // EASTWOOD_SHPFILE_H
