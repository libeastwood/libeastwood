#ifndef EASTWOOD_ICNFILE_H
#define EASTWOOD_ICNFILE_H

/*!
    A class for loading a *.ICN-File and the corresponding *.MAP-File.
    
	This class can read icn-Files and return the contained pictures as a SDL_Surface. An icn-File contains
	small 16x16 pixel tiles. The map-file contains the information how to build up a complete picture with
	this small tiles.
*/

#include <vector>

#include "Decode.h"
#include "MapFile.h"

namespace eastwood {

class IcnFile : public Decode
{
public:
	IcnFile(const std::istream &stream, MapFile &map, Palette *palette);
	~IcnFile();

	Surface getSurface(int IndexOfFile);

	Surface getTiles(int index, bool frameByFrame = true);

/// Returns an array of pictures in the icn-File
/*!
	This method returns a SDL_Surface containing multiple tiles/pictures. Which tiles to include is specified by MapfileIndex. The
	MapfileIndex specifies the tileSet. One tileset constists of multiple tiles of the icn-File.
	The last 3 parameters specify how to arrange the tiles:
	 - If all 3 parameters are 0 then a "random" layout is choosen, which should look good.
	 - If tilesX and tilesY is set to non-zero values then the result surface contains tilesX*tilesY tiles and this tilesN-times side by side.
	 - If all there parameters are non-zero then the result surface is exactly in this arrangement.

	tilesX*tilesY*tilesN must always the number of tiles in this tileSet. Otherwise NULL is returned.<br><br>
	Example:
	@code
	TileSet = 10,11,12,13,14,15,16,17,18,19,20,21
	tilesX = 2; tilesY = 2; tilesN = 3

	returned picture:
	 10 11 14 15 18 19
	 12 13 16 17 20 21
	@endcode
	<br>
	The returned SDL_Surface should be freed with SDL_FreeSurface() if no longer needed.
	@param	MapfileIndex	specifies which tileSet to use (zero based)
	@param	tilesX			how many tiles in x direction
	@param	tilesY			how many tiles in y direction
	@param	tilesN			how many tilesX*tilesY blocks in a row
	@return	the result surface with tilesX*tilesY*tilesN tiles
*/
//  	Surface getSurfaceArray(uint32_t MapfileIndex, int tilesX = 0, int tilesY = 0, int tilesN = 0);

/*!
	This method returns a SDL_Surface containing multiple tiles/pictures. The returned surface contains all
	tiles from StartIndex to EndIndex.
	The returned SDL_Surface should be freed with SDL_FreeSurface() if no longer needed.
	@param	StartIndex		The first tile to use
	@param	EndIndex		The last tile to use
	@return	the result surface with (EndIndex-StartIndex+1) tiles. NULL on errors.
*/
//	Surface getSurfaceRow(uint32_t StartIndex, uint32_t EndIndex);

	/// Returns the number of tiles
/*!
	Returns the number of tiles in the icn-File.
    @return	Number of tiles
*/
	inline int size() {
	    return (_SSET != NULL) ? _SSET->size() / ((_width * _height) / 2) : 0;
	}
	
private:
	void readHeader();
	void createImage(int index, uint8_t *dest, uint16_t pitch);
	MapFile &_map;

	std::vector<uint8_t>	*_SSET, // Structure Set Block
				*_RPAL, // Reference Palette
				*_RTBL; // Reference Table
};

}
#endif // EASTWOOD_ICNFILE_H
