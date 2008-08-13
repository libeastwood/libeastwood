#include <SDL_endian.h>
#include <stdlib.h>
#include <string.h>

#include "Log.h"

#include "ShpFile.h"

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

SDL_Surface *ShpFile::getSurface(Uint32 IndexOfFile)
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
	Uint16 size = SDL_SwapLE16(*((Uint16*) (Fileheader + 8)));
	
	
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
			LOG_ERROR("ShpFile","Type %d in SHP-Files not supported!",type);
			exit(EXIT_FAILURE);
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
	Uint32 *tiles;
	if((tilesX == 0) || (tilesY == 0)) {
		return NULL;
	}
	
	if((tiles = (Uint32*) malloc(tilesX*tilesY*sizeof(Uint32))) == NULL) {
		LOG_ERROR("ShpFile","ShpFile::getSurfaceArray(): Cannot allocate memory!");
		exit(EXIT_FAILURE);
	}

	va_list arg_ptr;
	va_start(arg_ptr, tilesY);
	
	for(Uint32 i = 0; i < tilesX*tilesY; i++) {
		tiles[i] = va_arg( arg_ptr, int );
		if(TILE_GETINDEX(tiles[i]) >= NumFiles) {
		free(tiles);
			LOG_ERROR("ShpFile","ShpFile::getSurfaceArray(): There exist only %d files in this *.shp.",NumFiles);
			return NULL;
		}
	}
	
	va_end(arg_ptr);
	return getSurfaceArray(tilesX, tilesY, tiles);
}

SDL_Surface *ShpFile::getSurfaceArray(unsigned int tilesX, unsigned int tilesY, Uint32 *tiles) {
	SDL_Surface *pic = NULL;
	unsigned char *DecodeDestination = NULL;
	unsigned char *ImageOut = NULL;
	Uint32 i,j;
	
	unsigned char sizeY = (Filedata + Index[TILE_GETINDEX(tiles[0])].StartOffset)[2];
	unsigned char sizeX = (Filedata + Index[TILE_GETINDEX(tiles[0])].StartOffset)[3];
	
	for(i = 1; i < tilesX*tilesY; i++) {
		if(((Filedata + Index[TILE_GETINDEX(tiles[i])].StartOffset)[2] != sizeY)
		 || ((Filedata + Index[TILE_GETINDEX(tiles[i])].StartOffset)[3] != sizeX)) {
			free(tiles);
			LOG_ERROR("ShpFile","ShpFile::getSurfaceArray(): Not all pictures have the same size!");
			exit(EXIT_FAILURE);
		 }
	}
	
	// create new picture surface
	if((pic = SDL_CreateRGBSurface(SDL_HWSURFACE,sizeX*tilesX,sizeY*tilesY,8,0,0,0,0)) == NULL) {
		LOG_ERROR("ShpFile","ShpFile::getSurfaceArray(): Cannot create Surface.");
		exit(EXIT_FAILURE);
	}
	
	SDL_SetColors(pic, m_palette->colors, 0, m_palette->ncolors);
	SDL_LockSurface(pic);	
	
	for(j = 0; j < tilesY; j++)	{
		for(i = 0; i < tilesX; i++) {
	
			unsigned char *Fileheader = Filedata + Index[TILE_GETINDEX(tiles[j*tilesX+i])].StartOffset;	
			unsigned char type = Fileheader[0];
		
			/* size and also checksum */
			Uint16 size = SDL_SwapLE16(*((Uint16*) (Fileheader + 8)));
		
			if((ImageOut = (unsigned char*) calloc(1,sizeX*sizeY)) == NULL) {
				free(tiles);
				LOG_ERROR("ShpFile","ShpFile::getSurfaceArray(): Cannot allocate memory!");
				exit(EXIT_FAILURE);
			}
		
			switch(type) {
		
				case 0:
				{
					if( (DecodeDestination = (unsigned char*) calloc(1,size)) == NULL) {
						free(ImageOut);
						free(tiles);
						LOG_ERROR("ShpFile","ShpFile::getSurfaceArray(): Cannot allocate memory!");
						exit(EXIT_FAILURE);
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
						free(tiles);
						LOG_ERROR("ShpFile","ShpFile::getSurfaceArray(): Cannot allocate memory!");
						exit(EXIT_FAILURE);
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
				{
					LOG_ERROR("ShpFile","ShpFile: Type %d in SHP-Files not supported!",type);
					exit(EXIT_FAILURE);
				}
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
				{
					LOG_ERROR("ShpFile","ShpFile: Invalid type for this parameter. Must be one of TILE_NORMAL, TILE_FLIPH, TILE_FLIPV or TILE_ROTATE!");
					exit(EXIT_FAILURE);				
				} break;
			}
		
			if(ImageOut != NULL) {
				free(ImageOut);
			}
		}
	}
	
	free(tiles);
	
	SDL_UnlockSurface(pic);
	SDL_SetColorKey(pic, SDL_SRCCOLORKEY | SDL_RLEACCEL, 0);
	return pic;
}

void ShpFile::readIndex()
{
	// First get number of files in shp-file
	NumFiles = SDL_SwapLE16( ((Uint16*) Filedata)[0]);
	
	if(NumFiles == 0) {
		LOG_ERROR("ShpFile", "There is no file in this shp-File!");
		exit(EXIT_FAILURE);
	}
	
	if(NumFiles == 1) {
		/* files with only one image might be different */
		
		// create array with one entry
		if((Index = (ShpFileEntry*) malloc(sizeof(ShpFileEntry) * 1)) == NULL) {
			LOG_ERROR("ShpFile", "readIndex");
			exit(EXIT_FAILURE);
		}
		
		if (((Uint16*) Filedata)[2] != 0) {
			/* File has special header with only 2 byte offset */
			
			Index[0].StartOffset = ((Uint32) SDL_SwapLE16(((Uint16*) Filedata)[1]));
			Index[0].EndOffset = ((Uint32) SDL_SwapLE16(((Uint16*) Filedata)[2])) - 1;


		} else {
			/* File has normal 4 byte offsets */
			Index[0].StartOffset = ((Uint32) SDL_SwapLE32(*((Uint32*) (Filedata+2)))) + 2;
			Index[0].EndOffset = ((Uint32) SDL_SwapLE16(((Uint16*) Filedata)[3])) - 1 + 2;
		}

	} else {
		/* File contains more than one image */
	
		if( ShpFilesize < (Uint32) ((NumFiles * 4) + 2 + 2)) {
			LOG_ERROR("ShpFile", "Shp-File-Header is not complete! Header should be %d bytes big, but Shp-File is only %d bytes long.",(NumFiles * 4) + 2 + 2,ShpFilesize);
			exit(EXIT_FAILURE);				
		}
		
		// create array
		if((Index = (ShpFileEntry*) malloc(sizeof(ShpFileEntry) * NumFiles)) == NULL) {
			LOG_ERROR("ShpFile", "readIndex");
			exit(EXIT_FAILURE);
		}
		
		// now fill Index with start and end-offsets
		for(int i = 0; i < NumFiles; i++) {
			Index[i].StartOffset = SDL_SwapLE32( ((Uint32*)(Filedata+2))[i]) + 2;
			
			if(i > 0) {
				Index[i-1].EndOffset = Index[i].StartOffset - 1;
				
				if(Index[i-1].EndOffset > ShpFilesize) {
					LOG_ERROR("ShpFile", "The File with Index %d, goes until byte %d, but this SHP-File is only %d bytes big.",i,Index[i-1].EndOffset,ShpFilesize);
					exit(EXIT_FAILURE);						
				}
			}
		}
		
		// Add the EndOffset for the last file
		Index[NumFiles-1].EndOffset = ((Uint32) SDL_SwapLE16( *((Uint16*) (Filedata + 2 + (NumFiles * 4))))) - 1 + 2;
	}
}
