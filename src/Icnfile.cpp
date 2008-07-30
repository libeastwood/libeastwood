#include <SDL_endian.h>
#include <stdlib.h>
#include <string.h>


#include "Icnfile.h"
#include "Log.h"

#define	SIZE_X	16
#define SIZE_Y	16

  

Icnfile::Icnfile(unsigned char * bufFiledata, int bufsize, 
                 unsigned char * bufMapdata, int mapsize) {
	Filedata = bufFiledata;
	IcnFilesize = bufsize;

    int MapFilesize = mapsize;
    unsigned char * MapFiledata = bufMapdata;
    int i;

	// now we can start creating the Tilesetindex
	if(MapFilesize < 2) {
		LOG_ERROR("Icnfile","Mapfile: This *.map-File is too short!");
		exit(EXIT_FAILURE);		
	}
	
	NumTilesets = SDL_SwapLE16( *((Uint16 *) MapFiledata));
	
	if(MapFilesize < NumTilesets * 2) {
		LOG_ERROR("Icnfile", "Mapfile: This *.map-File is too short!");
		exit(EXIT_FAILURE);		
	}	
	
	if((Tileset = (MapfileEntry*) malloc(sizeof(MapfileEntry)*NumTilesets)) == NULL) {
		LOG_ERROR("Icnfile","Mapfile: Allocating memory failed!");
		exit(EXIT_FAILURE);
	}
	
	// calculate size for all entries
	Uint16 index = SDL_SwapLE16( ((Uint16*) MapFiledata)[0]);
	for(i = 1; i < NumTilesets; i++) {
		Uint16 tmp = SDL_SwapLE16( ((Uint16*) MapFiledata)[i]);
		Tileset[i-1].NumTiles = tmp - index;
		index = tmp;
	}
	Tileset[NumTilesets-1].NumTiles = (MapFilesize/2) - index;
	
	for(i = 0; i < NumTilesets; i++) {
	
		if((Tileset[i].TileIndex = (Uint16*) malloc(sizeof(Uint16)*Tileset[i].NumTiles)) == NULL) {
			LOG_ERROR("Icnfile","Mapfile: Allocating memory failed!");
			exit(EXIT_FAILURE);
		}
		
		index = SDL_SwapLE16( ((Uint16*) MapFiledata)[i]);
		
		if((unsigned int)MapFilesize < (index+Tileset[i].NumTiles)*2 ) {
			LOG_ERROR("Icnfile","Mapfile: This *.map-File is too short!");
			exit(EXIT_FAILURE);			
		}
		
		// now we can read in
		for(unsigned int j = 0; j < Tileset[i].NumTiles; j++) {
			Tileset[i].TileIndex[j] = SDL_SwapLE16( ((Uint16*) MapFiledata)[index+j]);
		}
	}
	free(MapFiledata);
	// reading MAP-File is now finished
	

	// check if we can access first section;
	if(IcnFilesize < 0x20) {
		LOG_ERROR("Icnfile", "Invalid ICN-File: No SSET-Section found!");
		exit(EXIT_FAILURE);
	}
	
	
	SSET = Filedata+0x18;
	
	// check SSET-Section
	if(		(SSET[0] != 'S')
		||	(SSET[1] != 'S')
		||	(SSET[2] != 'E')
		||	(SSET[3] != 'T')) {
		LOG_ERROR("Icnfile", "Invalid ICN-File: No SSET-Section found!");
		exit(EXIT_FAILURE);			
	}
	
	SSET_Length = SDL_SwapBE32( *((Uint32*) (SSET + 4))) - 8;
	
	SSET += 16;
	
	if(Filedata + IcnFilesize < SSET + SSET_Length) {
		LOG_ERROR("Icnfile", "Invalid ICN-File: SSET-Section is bigger than ICN-File!");
		exit(EXIT_FAILURE);					
	}
	
	RPAL = SSET + SSET_Length;
	
	// check RPAL-Section
	if(		(RPAL[0] != 'R')
		||	(RPAL[1] != 'P')
		||	(RPAL[2] != 'A')
		||	(RPAL[3] != 'L')) {
		LOG_ERROR("Icnfile", "Invalid ICN-File: No RPAL-Section found!");
		exit(EXIT_FAILURE);			
	}
	
	RPAL_Length = SDL_SwapBE32( *((Uint32*) (RPAL + 4)));
	
	RPAL += 8;
	
	if(Filedata + IcnFilesize < RPAL + RPAL_Length) {
		LOG_ERROR("Icnfile", "Invalid ICN-File: RPAL-Section is bigger than ICN-File!");
		exit(EXIT_FAILURE);					
	}
	
	RTBL = RPAL + RPAL_Length;
	
	// check RTBL-Section
	if(		(RTBL[0] != 'R')
		||	(RTBL[1] != 'T')
		||	(RTBL[2] != 'B')
		||	(RTBL[3] != 'L')) {
		LOG_ERROR("Icnfile", "Invalid ICN-File: No RTBL-Section found!");
		exit(EXIT_FAILURE);			
	}
	

	
	RTBL_Length = SDL_SwapBE32( *((Uint32*) (RTBL + 4)));
	
	RTBL += 8;
	
	if(Filedata + IcnFilesize < RTBL + RTBL_Length) {
		LOG_ERROR("Icnfile", "Invalid ICN-File: RTBL-Section is bigger than ICN-File!");
		exit(EXIT_FAILURE);					
	}
	
	NumFiles = SSET_Length / ((SIZE_X * SIZE_Y) / 2);
		
	if(RTBL_Length < NumFiles) {
		LOG_ERROR("Icnfile", "Invalid ICN-File: RTBL-Section is too small!");
		exit(EXIT_FAILURE);			
	}
}

Icnfile::~Icnfile()
{	
	;
}

SDL_Surface *Icnfile::getSurface(Uint32 IndexOfFile, SDL_Palette *palette) {
	SDL_Surface * pic;
	
	if(IndexOfFile >= NumFiles) {
		return NULL;
	}
	
	// check if palette is in range
	if(RTBL[IndexOfFile] >= RPAL_Length / 16) {
		return NULL;
	}
	
	unsigned char* palettestart = RPAL + (16 * RTBL[IndexOfFile]);
		
	unsigned char * filestart = SSET + (IndexOfFile * ((SIZE_X * SIZE_Y)/2));
	
	// create new picture surface
	if((pic = SDL_CreateRGBSurface(SDL_SWSURFACE,SIZE_X,SIZE_Y,8,0,0,0,0))== NULL) {
		return NULL;
	}
			
	SDL_SetColors(pic, palette->colors, 0, palette->ncolors);
	SDL_LockSurface(pic);	

	//Now we can copy to surface
	unsigned char *dest = (unsigned char*) (pic->pixels);
	unsigned char pixel;
	for(int y = 0; y < SIZE_Y;y++) {
		for(int x = 0; x < SIZE_X; x+=2) {
			pixel = filestart[ (y*SIZE_X + x) / 2];
			pixel = pixel >> 4;
			dest[x] = palettestart[pixel];
			
			pixel = filestart[ (y*SIZE_X + x) / 2];
			pixel = pixel & 0x0F;
			dest[x+1] = palettestart[pixel];
		}
		dest += pic->pitch;
	}
		
	SDL_UnlockSurface(pic);
	
	LOG_INFO("Icnfile", "File Nr.: %d (Size: %dx%d)",IndexOfFile,SIZE_X,SIZE_Y);

	return pic;
}

SDL_Surface *Icnfile::getSurfaceArray(Uint32 MapfileIndex, SDL_Palette *palette, int tilesX, int tilesY, int tilesN) {
	SDL_Surface * pic;
	
	if(MapfileIndex >= NumTilesets) {
		return NULL;
	}
	
	if((tilesX == 0) && (tilesY == 0) && (tilesN == 0)) {
		// guest what is best
		int tmp = Tileset[MapfileIndex].NumTiles;
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
		if(Tileset[MapfileIndex].NumTiles % tilesN == 0) {
			// guest what is best
			int tmp = Tileset[MapfileIndex].NumTiles / tilesN;
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
		if((unsigned int)tilesX*tilesY*tilesN != Tileset[MapfileIndex].NumTiles) {
			return NULL;
		}
	}
	
	// create new picture surface
	if((pic = SDL_CreateRGBSurface(SDL_HWSURFACE,SIZE_X*tilesX*tilesN,SIZE_Y*tilesY,8,0,0,0,0))== NULL) {
		return NULL;
	}
	
	SDL_SetColors(pic, palette->colors, 0, palette->ncolors);
	SDL_LockSurface(pic);
	
	int Tileidx=0;
	for(int n = 0; n < tilesN; n++) {
		for(int y = 0; y < tilesY; y++) {
			for(int x = 0; x < tilesX; x++) {
				int IndexOfFile = Tileset[MapfileIndex].TileIndex[Tileidx];		
			
				// check if palette is in range
				if(RTBL[IndexOfFile] >= RPAL_Length / 16) {
					SDL_UnlockSurface(pic);
					SDL_FreeSurface(pic);
					return NULL;
				}
	
				unsigned char* palettestart = RPAL + (16 * RTBL[IndexOfFile]);
				unsigned char * filestart = SSET + (IndexOfFile * ((SIZE_X * SIZE_Y)/2));

				//Now we can copy to surface
				unsigned char *dest = (unsigned char*) (pic->pixels) + (pic->pitch)*y*SIZE_Y + (x+n*tilesX) * SIZE_X;
				unsigned char pixel;
				for(int y = 0; y < SIZE_Y;y++) {
					for(int x = 0; x < SIZE_X; x+=2) {
						pixel = filestart[ (y*SIZE_X + x) / 2];
						pixel = pixel >> 4;
						dest[x] = palettestart[pixel];
			
						pixel = filestart[ (y*SIZE_X + x) / 2];
						pixel = pixel & 0x0F;
						dest[x+1] = palettestart[pixel];
					}
					dest += pic->pitch;
				}
				
				Tileidx++;
			}
		}
	}
		
	SDL_UnlockSurface(pic);

	return pic;
}

SDL_Surface *Icnfile::getSurfaceRow(Uint32 StartIndex, Uint32 EndIndex, SDL_Palette *palette) {
	SDL_Surface * pic;
	
	if((StartIndex >= NumFiles)||(EndIndex >= NumFiles)||(StartIndex > EndIndex)) {
		return NULL;
	}
	
	Uint32 NumTiles = EndIndex - StartIndex + 1;
	// create new picture surface
	if((pic = SDL_CreateRGBSurface(SDL_HWSURFACE,SIZE_X*NumTiles,SIZE_Y,8,0,0,0,0))== NULL) {
		return NULL;
	}
	
	SDL_SetColors(pic, palette->colors, 0, palette->ncolors);
	SDL_LockSurface(pic);	

	for(unsigned int i = 0; i < NumTiles; i++) {
		int IndexOfFile = i+StartIndex;		
			
		// check if palette is in range
		if(RTBL[IndexOfFile] >= RPAL_Length / 16) {
			SDL_UnlockSurface(pic);
			SDL_FreeSurface(pic);
			return NULL;
		}
	
		unsigned char* palettestart = RPAL + (16 * RTBL[IndexOfFile]);
		unsigned char * filestart = SSET + (IndexOfFile * ((SIZE_X * SIZE_Y)/2));

		//Now we can copy to surface
		unsigned char *dest = (unsigned char*) (pic->pixels) + i*SIZE_X;
		unsigned char pixel;
		for(int y = 0; y < SIZE_Y;y++) {
			for(int x = 0; x < SIZE_X; x+=2) {
				pixel = filestart[ (y*SIZE_X + x) / 2];
				pixel = pixel >> 4;
				dest[x] = palettestart[pixel];
		
				pixel = filestart[ (y*SIZE_X + x) / 2];
				pixel = pixel & 0x0F;
				dest[x+1] = palettestart[pixel];
			}
			dest += pic->pitch;
		}
	}
		
	SDL_UnlockSurface(pic);
	return pic;
}

int Icnfile::getNumFiles()
{
	return NumFiles;
}
