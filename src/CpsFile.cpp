#include <iostream>
#include <string>
#include <SDL.h>

#include "StdDef.h"

#include "CpsFile.h"
#include "Log.h"

#define	SIZE_X	320
#define SIZE_Y	200

CpsFile::CpsFile(const uint8_t *bufFileData, int bufSize, SDL_Palette *palette) : Decode()
{
	m_filedata = bufFileData;
	if(*(uint8_t *)(bufFileData + 9) == 3){
		LOG_INFO("CpsFile", "CPS has embedded palette, loading...");
		m_palette = new SDL_Palette;
		m_palette->ncolors = bufSize / 3;
		m_palette->colors = new SDL_Color[m_palette->ncolors];

		bufFileData += 10;
		for (int i = 0; i < m_palette->ncolors; i++){
			m_palette->colors[i].r = *bufFileData++ <<2;
			m_palette->colors[i].g = *bufFileData++ <<2;
			m_palette->colors[i].b = *bufFileData++ <<2;
			m_palette->colors[i].unused = 0;
		}
	}else{
			m_palette = palette;
	}
}

CpsFile::~CpsFile()
{	
}

SDL_Surface *CpsFile::getSurface()
{
	uint8_t *ImageOut;
	SDL_Surface *pic = NULL;

	// check for valid file
	if( htole16(*(uint16_t *)(m_filedata + 2)) != 0x0004) {
		return NULL;
	}
	
	if( htole16(*(uint16_t *)(m_filedata + 4)) != 0xFA00) {
		return NULL;
	}
	
	uint16_t PaletteSize = htole16(*((uint16_t *)(m_filedata + 8)));
	
	if( (ImageOut = (uint8_t*) calloc(1,SIZE_X*SIZE_Y)) == NULL) {
		return NULL;
	}
	
	if(decode80(m_filedata + 10 + PaletteSize,ImageOut,0) == -2) {
		LOG_ERROR("CpsFile", "Cannot decode Cps-File");
	}
	
	// create new picture surface
	if((pic = SDL_CreateRGBSurface(SDL_SWSURFACE,SIZE_X,SIZE_Y,8,0,0,0,0))== NULL) {
		return NULL;
	}

	SDL_SetColors(pic, m_palette->colors, 0, m_palette->ncolors);
	
	SDL_LockSurface(pic);	

	//Now we can copy line by line
	for(int y = 0; y < SIZE_Y;y++) {
		memcpy(	((uint8_t*) (pic->pixels)) + y * pic->pitch , ImageOut + y * SIZE_X, SIZE_X);
	}
		
	SDL_UnlockSurface(pic);

	free(ImageOut);
	return pic;
}
