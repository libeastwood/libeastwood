#ifndef EASTWOOD_ICNFILE_H
#define EASTWOOD_ICNFILE_H

/*!
    A class for loading a ICN files.
    
	This class can read icn files and return pictures from it as Surface objects.
	An icn file contains small 16x16 pixel tiles which together with the information
	from a MAP file can be used to create pictures or animation sequences from these.
*/

#include <vector>

#include "eastwood/MapFile.h"
#include "eastwood/Palette.h"
#include "eastwood/Surface.h"

namespace eastwood {

class IcnFile
{
public:
	IcnFile(std::istream &stream, const Palette &palette, const MapFile &map);
	IcnFile(std::istream &stream, const Palette &palette);
	IcnFile(std::istream &stream);
	~IcnFile();

	Surface getSurface(uint16_t IndexOfFile);

	Surface getTiles(uint16_t index, bool frameByFrame = true);

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
	uint16_t size() const throw() {
	    return _SSET.size();
	}
	
private:
	void readHeader(std::istream &stream);
	void createImage(uint16_t index, uint8_t *dest, uint16_t pitch);
	Palette					_palette;
	MapFile					_map;

	std::vector<std::vector<uint8_t> >	_SSET, // Structure Set Chunk
						_RPAL; // RIFF Palette
	std::vector<uint8_t>			_RTBL; // Reference Table
	uint8_t					_bpp;
	uint16_t				_tileSize,
						_width,
					       	_height;

};

}
#endif // EASTWOOD_ICNFILE_H
