#ifndef ICNFILE_H_INCLUDED
#define ICNFILE_H_INCLUDED

#include "SDL.h"

/*!
    A class for loading a *.ICN-File and the corresponding *.MAP-File.
    
	This class can read icn-Files and return the contained pictures as a SDL_Surface. An icn-File contains
	small 16x16 pixel tiles. The map-file contains the information how to build up a complete picture with
	this small tiles.
*/
class Icnfile
{
private:
    //! Internal structure for the MAP-File.
	struct MapfileEntry
	{
		Uint32 NumTiles;
		Uint16* TileIndex;
	};
public:
	Icnfile(unsigned char * bufFiledata, int bufsize, 
	        unsigned char * bufMapdata, int mapsize);
	~Icnfile();


	SDL_Surface *getSurface(Uint32 IndexOfFile, SDL_Palette *palette);

/// Returns an array of pictures in the icn-File
/*!
	This method returns a SDL_Surface containing multiple tiles/pictures. Which tiles to include is specified by MapfileIndex. The
	MapfileIndex specifies the tileset. One tileset constists of multiple tiles of the icn-File.
	The last 3 parameters specify how to arrange the tiles:
	 - If all 3 parameters are 0 then a "random" layout is choosen, which should look good.
	 - If tilesX and tilesY is set to non-zero values then the result surface contains tilesX*tilesY tiles and this tilesN-times side by side.
	 - If all there parameters are non-zero then the result surface is exactly in this arrangement.

	tilesX*tilesY*tilesN must always the number of tiles in this tileset. Otherwise NULL is returned.<br><br>
	Example:
	@code
	Tileset = 10,11,12,13,14,15,16,17,18,19,20,21
	tilesX = 2; tilesY = 2; tilesN = 3

	returned picture:
	 10 11 14 15 18 19
	 12 13 16 17 20 21
	@endcode
	<br>
	The returned SDL_Surface should be freed with SDL_FreeSurface() if no longer needed.
	@param	MapfileIndex	specifies which tileset to use (zero based)
	@param	tilesX			how many tiles in x direction
	@param	tilesY			how many tiles in y direction
	@param	tilesN			how many tilesX*tilesY blocks in a row
	@return	the result surface with tilesX*tilesY*tilesN tiles
*/
  	SDL_Surface *getSurfaceArray(Uint32 MapfileIndex, SDL_Palette *palette, int tilesX = 0, int tilesY = 0, int tilesN = 0);

/*!
	This method returns a SDL_Surface containing multiple tiles/pictures. The returned surface contains all
	tiles from StartIndex to EndIndex.
	The returned SDL_Surface should be freed with SDL_FreeSurface() if no longer needed.
	@param	StartIndex		The first tile to use
	@param	EndIndex		The last tile to use
	@return	the result surface with (EndIndex-StartIndex+1) tiles. NULL on errors.
*/
	SDL_Surface *getSurfaceRow(Uint32 StartIndex, Uint32 EndIndex, SDL_Palette *palette);

	/// Returns the number of tiles
/*!
	Returns the number of tiles in the icn-File.
    @return	Number of tiles
*/
	int getNumFiles();
	
	/// Returns the number of tilesets
/*!
	Returns the number of tilesets in the map-File.
	@return	Number of tilesets
*/
	int getNumTilesets() { return NumTilesets; };

private:
	unsigned char* Filedata;
	Uint32 IcnFilesize;

	Uint32 NumFiles;

	Uint16	NumTilesets;
	MapfileEntry* Tileset;
	
	unsigned char* SSET;
	Uint32 SSET_Length;
	unsigned char* RPAL;
	Uint32 RPAL_Length;
	unsigned char* RTBL;
	Uint32 RTBL_Length;

};

#endif // ICNFILE_H_INCLUDED
