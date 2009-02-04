#include <SDL.h>
#include <SDL_endian.h>
#include <stdlib.h>
#include <string.h>


#include "IcnFile.h"
#include "Log.h"

#define	SIZE_X	16
#define SIZE_Y	16

  

IcnFile::IcnFile(const unsigned char *bufFileData, size_t bufSize,
                 const unsigned char *bufMapData, size_t mapSize,
				 SDL_Palette *palette) {
	m_fileData = bufFileData;
	m_icnFileSize = bufSize;
	m_palette = palette;

	// now we can start creating the TileSetindex
	if(mapSize < 2) {
		LOG_ERROR("IcnFile","Mapfile: This *.map-File is too short!");
		exit(EXIT_FAILURE);		
	}
	
	m_numTileSets = SDL_SwapLE16( *((Uint16 *) bufMapData));
	
	if(mapSize < (Uint16)(m_numTileSets * 2)) {
		LOG_ERROR("IcnFile", "Mapfile: This *.map-File is too short!");
		exit(EXIT_FAILURE);		
	}	
	
	if((m_tileSet = (MapfileEntry*) malloc(sizeof(MapfileEntry)*m_numTileSets)) == NULL) {
		LOG_ERROR("IcnFile","Mapfile: Allocating memory failed!");
		exit(EXIT_FAILURE);
	}
	
	// calculate size for all entries
	Uint16 index = SDL_SwapLE16( ((Uint16*) bufMapData)[0]);
	for(int i = 1; i < m_numTileSets; i++) {
		Uint16 tmp = SDL_SwapLE16( ((Uint16*) bufMapData)[i]);
		m_tileSet[i-1].numTiles = tmp - index;
		index = tmp;
	}
	m_tileSet[m_numTileSets-1].numTiles = (mapSize/2) - index;
	
	for(int i = 0; i < m_numTileSets; i++) {
	
		if((m_tileSet[i].tileIndex = (Uint16*) malloc(sizeof(Uint16)*m_tileSet[i].numTiles)) == NULL) {
			LOG_ERROR("IcnFile","Mapfile: Allocating memory failed!");
			exit(EXIT_FAILURE);
		}
		
		index = SDL_SwapLE16( ((Uint16*) bufMapData)[i]);
		
		if(mapSize < (index+m_tileSet[i].numTiles)*2 ) {
			LOG_ERROR("IcnFile","Mapfile: This *.map-File is too short!");
			exit(EXIT_FAILURE);			
		}
		
		// now we can read in
		for(unsigned int j = 0; j < m_tileSet[i].numTiles; j++) {
			m_tileSet[i].tileIndex[j] = SDL_SwapLE16( ((Uint16*) bufMapData)[index+j]);
		}
	}
	// reading MAP-File is now finished
	

	// check if we can access first section;
	if(m_icnFileSize < 0x20) {
		LOG_ERROR("IcnFile", "Invalid ICN-File: No SSET-Section found!");
		exit(EXIT_FAILURE);
	}
	
	
	m_SSET = (unsigned char*)(m_fileData+0x18);
	
	// check SSET-Section
	if(		(m_SSET[0] != 'S')
		||	(m_SSET[1] != 'S')
		||	(m_SSET[2] != 'E')
		||	(m_SSET[3] != 'T')) {
		LOG_ERROR("IcnFile", "Invalid ICN-File: No SSET-Section found!");
		exit(EXIT_FAILURE);			
	}
	
	m_SSET_Length = SDL_SwapBE32( *((Uint32*) (m_SSET + 4))) - 8;
	
	m_SSET += 16;
	
	if(m_fileData + m_icnFileSize < m_SSET + m_SSET_Length) {
		LOG_ERROR("IcnFile", "Invalid ICN-File: SSET-Section is bigger than ICN-File!");
		exit(EXIT_FAILURE);
	}
	
	m_RPAL = m_SSET + m_SSET_Length;
	
	// check RPAL-Section
	if(		(m_RPAL[0] != 'R')
		||	(m_RPAL[1] != 'P')
		||	(m_RPAL[2] != 'A')
		||	(m_RPAL[3] != 'L')) {
		LOG_ERROR("IcnFile", "Invalid ICN-File: No RPAL-Section found!");
		exit(EXIT_FAILURE);			
	}
	
	m_RPAL_Length = SDL_SwapBE32( *((Uint32*) (m_RPAL + 4)));
	
	m_RPAL += 8;
	
	if(m_fileData + m_icnFileSize < m_RPAL + m_RPAL_Length) {
		LOG_ERROR("IcnFile", "Invalid ICN-File: RPAL-Section is bigger than ICN-File!");
		exit(EXIT_FAILURE);					
	}
	
	m_RTBL = m_RPAL + m_RPAL_Length;
	
	// check RTBL-Section
	if(		(m_RTBL[0] != 'R')
		||	(m_RTBL[1] != 'T')
		||	(m_RTBL[2] != 'B')
		||	(m_RTBL[3] != 'L')) {
		LOG_ERROR("IcnFile", "Invalid ICN-File: No RTBL-Section found!");
		exit(EXIT_FAILURE);			
	}
	

	
	m_RTBL_Length = SDL_SwapBE32( *((Uint32*) (m_RTBL + 4)));
	
	m_RTBL += 8;
	
	if(m_fileData + m_icnFileSize < m_RTBL + m_RTBL_Length) {
		LOG_ERROR("IcnFile", "Invalid ICN-File: RTBL-Section is bigger than ICN-File!");
		exit(EXIT_FAILURE);					
	}
	
	m_numFiles = m_SSET_Length / ((SIZE_X * SIZE_Y) / 2);
		
	if(m_RTBL_Length < m_numFiles) {
		LOG_ERROR("IcnFile", "Invalid ICN-File: RTBL-Section is too small!");
		exit(EXIT_FAILURE);			
	}
}

IcnFile::~IcnFile()
{	
}

SDL_Surface *IcnFile::getSurface(Uint32 indexOfFile) {
	SDL_Surface *pic;
	
	if(indexOfFile >= m_numFiles) {
		return NULL;
	}
	
	// check if palette is in range
	if(m_RTBL[indexOfFile] >= m_RPAL_Length / 16) {
		return NULL;
	}
	
	unsigned char* paletteStart = m_RPAL + (16 * m_RTBL[indexOfFile]);
		
	unsigned char *filestart = m_SSET + (indexOfFile * ((SIZE_X * SIZE_Y)/2));
	
	// create new picture surface
	if((pic = SDL_CreateRGBSurface(SDL_SWSURFACE,SIZE_X,SIZE_Y,8,0,0,0,0))== NULL) {
		return NULL;
	}
			
	SDL_SetColors(pic, m_palette->colors, 0, m_palette->ncolors);
	SDL_LockSurface(pic);	

	//Now we can copy to surface
	unsigned char *dest = (unsigned char*) (pic->pixels);
	unsigned char pixel;
	for(int y = 0; y < SIZE_Y;y++) {
		for(int x = 0; x < SIZE_X; x+=2) {
			pixel = filestart[ (y*SIZE_X + x) / 2];
			pixel = pixel >> 4;
			dest[x] = paletteStart[pixel];
			
			pixel = filestart[ (y*SIZE_X + x) / 2];
			pixel = pixel & 0x0F;
			dest[x+1] = paletteStart[pixel];
		}
		dest += pic->pitch;
	}
		
	SDL_UnlockSurface(pic);
	
	LOG_INFO("IcnFile", "File Nr.: %d (Size: %dx%d)", indexOfFile,SIZE_X,SIZE_Y);

	return pic;
}

SDL_Surface *IcnFile::getSurfaceArray(Uint32 mapFileIndex, int tilesX, int tilesY, int tilesN) {
	SDL_Surface *pic;
	
	if(mapFileIndex >= m_numTileSets) {
		return NULL;
	}
	
	if((tilesX == 0) && (tilesY == 0) && (tilesN == 0)) {
		// guest what is best
		int tmp = m_tileSet[mapFileIndex].numTiles;
		if(tmp == 24) {
			// special case (radar station and light factory)
			tilesX = 2;
			tilesY = 2;
			tilesN = 6;
		} else if((tmp % 9) == 0) {
			tilesX = 3;
			tilesY = 3;
			tilesN = tmp / 9;
		} else if((tmp % 6) == 0) {
			tilesX = 3;
			tilesY = 2;
			tilesN = tmp / 6;
		} else if((tmp % 4) == 0) {
			tilesX = 2;
			tilesY = 2;
			tilesN = tmp / 4;
		} else if((tmp>=40) && ((tmp % 5) == 0)) {
			tilesX = tmp/5;
			tilesY = 5;
			tilesN = 1;
		} else {
			tilesX = 1;
			tilesY = 1;
			tilesN = tmp;
		}
		
	} else if( ((tilesX == 0) || (tilesY == 0)) && (tilesN == 0)) {
		// not possible
		return NULL;		
	} else if((tilesX == 0) && (tilesY == 0) && (tilesN != 0)) {
		if(m_tileSet[mapFileIndex].numTiles % tilesN == 0) {
			// guest what is best
			int tmp = m_tileSet[mapFileIndex].numTiles / tilesN;
			if((tmp % 3) == 0) {
				tilesX = tmp/3;
				tilesY = 3;
			} else if((tmp % 2) == 0) {
				tilesX = tmp/2;
				tilesY = 2;
			} else {
				tilesX = tmp;
				tilesY = 1;
			}
		} else {
			// not possible
			return NULL;
		}
	} else {
		if((unsigned int)tilesX*tilesY*tilesN != m_tileSet[mapFileIndex].numTiles) {
			return NULL;
		}
	}
	
	// create new picture surface
	if((pic = SDL_CreateRGBSurface(SDL_HWSURFACE,SIZE_X*tilesX*tilesN,SIZE_Y*tilesY,8,0,0,0,0))== NULL) {
		return NULL;
	}
	
	SDL_SetColors(pic, m_palette->colors, 0, m_palette->ncolors);
	SDL_LockSurface(pic);
	
	int tileIdx = 0;
	for(int n = 0; n < tilesN; n++) {
		for(int y = 0; y < tilesY; y++) {
			for(int x = 0; x < tilesX; x++) {
				int indexOfFile = m_tileSet[mapFileIndex].tileIndex[tileIdx];
			
				// check if palette is in range
				if(m_RTBL[indexOfFile] >= m_RPAL_Length / 16) {
					SDL_UnlockSurface(pic);
					SDL_FreeSurface(pic);
					return NULL;
				}
	
				unsigned char *paletteStart = m_RPAL + (16 * m_RTBL[indexOfFile]);
				unsigned char *fileStart = m_SSET + (indexOfFile * ((SIZE_X * SIZE_Y)/2));

				//Now we can copy to surface
				unsigned char *dest = (unsigned char*) (pic->pixels) + (pic->pitch)*y*SIZE_Y + (x+n*tilesX) * SIZE_X;
				unsigned char pixel;
				for(int y = 0; y < SIZE_Y;y++) {
					for(int x = 0; x < SIZE_X; x+=2) {
						pixel = fileStart[ (y*SIZE_X + x) / 2];
						pixel = pixel >> 4;
						dest[x] = paletteStart[pixel];
			
						pixel = fileStart[ (y*SIZE_X + x) / 2];
						pixel = pixel & 0x0F;
						dest[x+1] = paletteStart[pixel];
					}
					dest += pic->pitch;
				}
				
				tileIdx++;
			}
		}
	}
		
	SDL_UnlockSurface(pic);

	return pic;
}

SDL_Surface *IcnFile::getSurfaceRow(Uint32 startIndex, Uint32 endIndex) {
	SDL_Surface *pic;
	
	if((startIndex >= m_numFiles)||(endIndex >= m_numFiles)||(startIndex > endIndex)) {
		return NULL;
	}
	
	Uint32 numTiles = endIndex - startIndex + 1;
	// create new picture surface
	if((pic = SDL_CreateRGBSurface(SDL_HWSURFACE,SIZE_X*numTiles,SIZE_Y,8,0,0,0,0))== NULL) {
		return NULL;
	}
	
	SDL_SetColors(pic, m_palette->colors, 0, m_palette->ncolors);
	SDL_LockSurface(pic);	

	for(unsigned int i = 0; i < numTiles; i++) {
		int indexOfFile = i+startIndex;		
			
		// check if palette is in range
		if(m_RTBL[indexOfFile] >= m_RPAL_Length / 16) {
			SDL_UnlockSurface(pic);
			SDL_FreeSurface(pic);
			return NULL;
		}
	
		unsigned char *paletteStart = m_RPAL + (16 * m_RTBL[indexOfFile]);
		unsigned char *fileStart = m_SSET + (indexOfFile * ((SIZE_X * SIZE_Y)/2));

		//Now we can copy to surface
		unsigned char *dest = (unsigned char*) (pic->pixels) + i*SIZE_X;
		unsigned char pixel;
		for(int y = 0; y < SIZE_Y;y++) {
			for(int x = 0; x < SIZE_X; x+=2) {
				pixel = fileStart[ (y*SIZE_X + x) / 2];
				pixel = pixel >> 4;
				dest[x] = paletteStart[pixel];
		
				pixel = fileStart[ (y*SIZE_X + x) / 2];
				pixel = pixel & 0x0F;
				dest[x+1] = paletteStart[pixel];
			}
			dest += pic->pitch;
		}
	}
		
	SDL_UnlockSurface(pic);
	return pic;
}

int IcnFile::getNumFiles()
{
	return m_numFiles;
}
