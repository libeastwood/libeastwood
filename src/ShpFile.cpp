#include <SDL.h>

#include "StdDef.h"

#include "Log.h"

#include "Exception.h"
#include "ShpFile.h"

using namespace eastwood;

ShpFile::ShpFile(unsigned char *bufFiledata, int bufSize, SDL_Palette *palette) : Decode()
{
	Filedata = bufFiledata;
	ShpFilesize = bufSize;
	Index = NULL;
	readIndex();
	m_palette = palette;
}

ShpFile::~ShpFile()
{
	if(Index != NULL) {
		free(Index);
	}
}

static inline uint32_t TILE_GETINDEX(const uint32_t x) {
    return (x & (TILE_NORMAL-1));
}


static inline TileType TILE_GETTYPE(const uint32_t x) {
    return (TileType)(x & (TILE_NORMAL-1)<<16);
}

SDL_Surface *ShpFile::getSurface(uint32_t IndexOfFile)
{
	SDL_Surface *pic = NULL;
	unsigned char *DecodeDestination = NULL;
	unsigned char *ImageOut = NULL;
	
	if(IndexOfFile >= NumFiles) {
		return NULL;
	}
	
	unsigned char *Fileheader = Filedata + Index[IndexOfFile].StartOffset;
	
	unsigned char type = Fileheader[0];
	
	unsigned char sizeY = Fileheader[2];
	unsigned char sizeX = Fileheader[3];
	
	/* size and also checksum */
	uint16_t size = htole16(*((uint16_t*) (Fileheader + 8)));
	
	
	LOG_INFO("ShpFile", "File Nr.: %d (Size: %dx%d)",IndexOfFile,sizeX,sizeY);
	/*
	printf("Type: %d\n",type);
	printf("SizeX: %d SizeY: %d\n",sizeX,sizeY);
	printf("Size: %d Filesize: %d\n",size,Index[IndexOfFile].EndOffset-Index[IndexOfFile].StartOffset);
	fflush(stdout);
	*/
	
	if((ImageOut = (unsigned char*) calloc(1,sizeX*sizeY)) == NULL) {
				return NULL;
	}
	
	switch(type) {
	
		case 0:
		{
			if( (DecodeDestination = (unsigned char*) calloc(1,size)) == NULL) {
				free(ImageOut);
				return NULL;
			}
			
			if(decode80(Fileheader + 10,DecodeDestination,size) == -1) {
				LOG_WARNING("ShpFile","Checksum-Error in Shp-File");
			}
						
			shp_correct_lf(DecodeDestination,ImageOut, size);
			
			free(DecodeDestination);
		} break;
		
		case 1:
		{
			if( (DecodeDestination = (unsigned char*) calloc(1,size)) == NULL) {
				free(ImageOut);
				return NULL;
			}
			
			if(decode80(Fileheader + 10 + 16,DecodeDestination,size) == -1) {
				LOG_WARNING("ShpFile", "Checksum-Error in Shp-File");
			}
			
			shp_correct_lf(DecodeDestination, ImageOut, size);
			
			apply_pal_offsets(Fileheader + 10,ImageOut,sizeX*sizeY);
			
			free(DecodeDestination);			
		} break;
		
		case 2:
		{
			shp_correct_lf(Fileheader+10, ImageOut,size);
		} break;
		
		case 3:
		{
		
			shp_correct_lf(Fileheader + 10 + 16, ImageOut,size);
		
			apply_pal_offsets(Fileheader + 10,ImageOut,sizeX*sizeY);
		} break;
		
		default:
		{
			char error[256];
			sprintf(error, "Type %d in SHP-Files not supported!", type);
			throw(Exception(LOG_ERROR, "ShpFile", error));
		}
	}
	
	// create new picture surface
	if((pic = SDL_CreateRGBSurface(SDL_SWSURFACE,sizeX,sizeY,8,0,0,0,0))== NULL) {
		return NULL;
	}
	SDL_SetColors(pic, m_palette->colors, 0, m_palette->ncolors);
	SDL_LockSurface(pic);	

	//Now we can copy line by line
	for(int y = 0; y < sizeY;y++) {
		memcpy(	((char*) (pic->pixels)) + y * pic->pitch , ImageOut + y * sizeX, sizeX);
	}
		
	SDL_UnlockSurface(pic);

    SDL_SetColorKey(pic, SDL_SRCCOLORKEY, 0);
	
	if(ImageOut != NULL) {
		free(ImageOut);
	}
	return pic;
}

SDL_Surface *ShpFile::getSurfaceArray(unsigned int tilesX, unsigned int tilesY, ...) {
	uint32_t *tiles;
	if((tilesX == 0) || (tilesY == 0)) {
		return NULL;
	}
	
	if((tiles = (uint32_t*) malloc(tilesX*tilesY*sizeof(uint32_t))) == NULL)
    	    throw(std::bad_alloc());

	va_list arg_ptr;
	va_start(arg_ptr, tilesY);
	
	for(uint32_t i = 0; i < tilesX*tilesY; i++) {
		tiles[i] = va_arg( arg_ptr, int );
		if(TILE_GETINDEX(tiles[i]) >= NumFiles) {
    		    free(tiles);
    		    char error[256];
    		    sprintf(error, "getSurfaceArray(): There exist only %d files in this *.shp.",NumFiles);
    		    throw(Exception(LOG_ERROR, "ShpFile", error));
		}
	}
	
	va_end(arg_ptr);
	return getSurfaceArray(tilesX, tilesY, tiles);
}

SDL_Surface *ShpFile::getSurfaceArray(unsigned int tilesX, unsigned int tilesY, uint32_t *tiles) {
	SDL_Surface *pic = NULL;
	unsigned char *DecodeDestination = NULL;
	unsigned char *ImageOut = NULL;
	uint32_t i,j;
	
	unsigned char sizeY = (Filedata + Index[TILE_GETINDEX(tiles[0])].StartOffset)[2];
	unsigned char sizeX = (Filedata + Index[TILE_GETINDEX(tiles[0])].StartOffset)[3];
	
	for(i = 1; i < tilesX*tilesY; i++) {
		if(((Filedata + Index[TILE_GETINDEX(tiles[i])].StartOffset)[2] != sizeY)
		 || ((Filedata + Index[TILE_GETINDEX(tiles[i])].StartOffset)[3] != sizeX)) {
			free(tiles);
			throw(Exception(LOG_ERROR, "ShpFile", "getSurfaceArray(): Not all pictures have the same size!"));
		 }
	}
	
	// create new picture surface
	if((pic = SDL_CreateRGBSurface(SDL_HWSURFACE,sizeX*tilesX,sizeY*tilesY,8,0,0,0,0)) == NULL) {
		throw(Exception(LOG_ERROR, "ShpFile","getSurfaceArray(): Cannot create Surface."));
	}
	
	SDL_SetColors(pic, m_palette->colors, 0, m_palette->ncolors);
	SDL_LockSurface(pic);	
	
	for(j = 0; j < tilesY; j++)	{
		for(i = 0; i < tilesX; i++) {
	
			unsigned char *Fileheader = Filedata + Index[TILE_GETINDEX(tiles[j*tilesX+i])].StartOffset;	
			unsigned char type = Fileheader[0];
		
			/* size and also checksum */
			uint16_t size = htole16(*((uint16_t*) (Fileheader + 8)));
		
			if((ImageOut = (unsigned char*) calloc(1,sizeX*sizeY)) == NULL) {
				free(tiles);
				throw(std::bad_alloc());
			}
		
			switch(type) {
		
				case 0:
				{
					if( (DecodeDestination = (unsigned char*) calloc(1,size)) == NULL) {
						free(ImageOut);
						free(tiles);
						throw(std::bad_alloc());
					}
				
					if(decode80(Fileheader + 10,DecodeDestination,size) == -1) {
						throw(Exception(LOG_WARNING, "ShpFile","Checksum-Error in Shp-File"));
					}
							
					shp_correct_lf(DecodeDestination,ImageOut, size);
				
					free(DecodeDestination);
				} break;
			
				case 1:
				{
					if( (DecodeDestination = (unsigned char*) calloc(1,size)) == NULL) {
						free(ImageOut);
						free(tiles);
						throw(std::bad_alloc());
					}
				
					if(decode80(Fileheader + 10 + 16,DecodeDestination,size) == -1) {
						LOG_WARNING("ShpFile","Checksum-Error in Shp-File");
					}
				
					shp_correct_lf(DecodeDestination, ImageOut, size);
				
					apply_pal_offsets(Fileheader + 10,ImageOut,sizeX*sizeY);
				
					free(DecodeDestination);			
				} break;
			
				case 2:
				{
					shp_correct_lf(Fileheader+10, ImageOut,size);
				} break;
			
				case 3:
				{		
					shp_correct_lf(Fileheader + 10 + 16, ImageOut,size);		
					apply_pal_offsets(Fileheader + 10,ImageOut,sizeX*sizeY);
				} break;
			
				default:
				char error[256];
	    			sprintf(error, "Type %d in SHP-Files not supported!",type);
	    			throw(Exception(LOG_ERROR, "ShpFile", error));
	    			break;		    
			}
	
			//Now we can copy line by line
			switch(TILE_GETTYPE(tiles[i])) {
				case TILE_NORMAL:
				{
					for(int y = 0; y < sizeY; y++) {
						memcpy(	((char*) (pic->pixels)) + i*sizeX + (y+j*sizeY) * pic->pitch , ImageOut + y * sizeX, sizeX);
					}
				} break;
				
				case TILE_FLIPH:
				{
					for(int y = 0; y < sizeY; y++) {
						memcpy(	((char*) (pic->pixels)) + i*sizeX + (y+j*sizeY) * pic->pitch , ImageOut + (sizeY-1-y) * sizeX, sizeX);
					}				
				} break;
				
				case TILE_FLIPV:
				{
					for(int y = 0; y < sizeY; y++) {
						for(int x = 0; x < sizeX; x++) {
							*(((char*) (pic->pixels)) + i*sizeX + (y+j*sizeY) * pic->pitch + x) = *(ImageOut + y * sizeX + (sizeX-1-x));
						}
					}
				} break;
				
				case TILE_ROTATE:
				{
					for(int y = 0; y < sizeY; y++) {
						for(int x = 0; x < sizeX; x++) {
							*(((char*) (pic->pixels)) + i*sizeX + (y+j*sizeY) * pic->pitch + x) = *(ImageOut + (sizeY-1-y) * sizeX + (sizeX-1-x));
						}
					}
				} break;
				
				default:
	    			    throw(Exception(LOG_ERROR, "ShpFile", "Invalid type for this parameter. Must be one of TILE_NORMAL, TILE_FLIPH, TILE_FLIPV or TILE_ROTATE!"));
   				    break;
			}
		
			if(ImageOut != NULL) {
				free(ImageOut);
			}
		}
	}
	
	SDL_UnlockSurface(pic);
	SDL_SetColorKey(pic, SDL_SRCCOLORKEY | SDL_RLEACCEL, 0);
	return pic;
}

void ShpFile::readIndex()
{
	// First get number of files in shp-file
	NumFiles = htole16( ((uint16_t*) Filedata)[0]);
	
	if(NumFiles == 0) {
	    throw(Exception(LOG_ERROR, "ShpFile", "There is no file in this shp-File!"));
	}
	
	if(NumFiles == 1) {
		/* files with only one image might be different */
		
		// create array with one entry
		if((Index = (ShpFileEntry*) malloc(sizeof(ShpFileEntry) * 1)) == NULL) {
			throw(Exception(LOG_ERROR, "ShpFile", "readIndex"));
		}
		
		if (((uint16_t*) Filedata)[2] != 0) {
			/* File has special header with only 2 byte offset */
			
			Index[0].StartOffset = ((uint32_t) htole16(((uint16_t*) Filedata)[1]));
			Index[0].EndOffset = ((uint32_t) htole16(((uint16_t*) Filedata)[2])) - 1;


		} else {
			/* File has normal 4 byte offsets */
			Index[0].StartOffset = ((uint32_t) htole32(*((uint32_t*) (Filedata+2)))) + 2;
			Index[0].EndOffset = ((uint32_t) htole16(((uint16_t*) Filedata)[3])) - 1 + 2;
		}

	} else {
		/* File contains more than one image */
	
		if( ShpFilesize < (uint32_t) ((NumFiles * 4) + 2 + 2)) {
		    char error[256];
		    sprintf(error, "Shp-File-Header is not complete! Header should be %d bytes big, but Shp-File is only %d bytes long.",(NumFiles * 4) + 2 + 2,ShpFilesize);
		    throw(Exception(LOG_ERROR, "ShpFile", error));
		}
		
		// create array
		if((Index = (ShpFileEntry*) malloc(sizeof(ShpFileEntry) * NumFiles)) == NULL) {
			throw(Exception(LOG_ERROR, "ShpFile", "readIndex"));
		}
		
		// now fill Index with start and end-offsets
		for(int i = 0; i < NumFiles; i++) {
			Index[i].StartOffset = htole32( ((uint32_t*)(Filedata+2))[i]) + 2;
			
			if(i > 0) {
				char error[256];
				Index[i-1].EndOffset = Index[i].StartOffset - 1;
				sprintf(error, "The File with Index %d, goes until byte %d, but this SHP-File is only %d bytes big.",
						    i,Index[i-1].EndOffset,ShpFilesize);
				if(Index[i-1].EndOffset > ShpFilesize)
				        throw(Exception(LOG_ERROR, "ShpFile", error));
			}
		}
		
		// Add the EndOffset for the last file
		Index[NumFiles-1].EndOffset = ((uint32_t) htole16( *((uint16_t*) (Filedata + 2 + (NumFiles * 4))))) - 1 + 2;
	}
}
