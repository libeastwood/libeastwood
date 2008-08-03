#include <iostream>
#include <string>

#include <SDL_endian.h>
#include "CpsFile.h"
#include "Log.h"

#define	SIZE_X	320
#define SIZE_Y	200

CpsFile::CpsFile(unsigned char * bufFiledata, int bufsize, SDL_Palette* palette) : Decode()
{
	Filedata = bufFiledata;
	CpsFilesize = bufsize;
	if(*(unsigned char *)(bufFiledata + 9) == 3){
		LOG_INFO("CpsFile", "CPS has embedded palette, loading...");
		m_palette = new SDL_Palette;
		m_palette->ncolors = bufsize / 3;
		m_palette->colors = new SDL_Color[m_palette->ncolors];

		bufFiledata += 10;
		for (int i = 0; i < m_palette->ncolors; i++){
			m_palette->colors[i].r = *bufFiledata++ <<2;
			m_palette->colors[i].g = *bufFiledata++ <<2;
			m_palette->colors[i].b = *bufFiledata++ <<2;
			m_palette->colors[i].unused = 0;
		}
	}else{
//		if (palette == NULL)
//			m_palette = DataCache::Instance()->getPalette(IBM_PAL);
//		else
			m_palette = palette;
	}
}

CpsFile::~CpsFile()
{	
	;
}

SDL_Surface *CpsFile::getSurface()
{
	unsigned char * ImageOut;
	SDL_Surface *pic = NULL;

	// check for valid file
	if( SDL_SwapLE16(*(unsigned short *)(Filedata + 2)) != 0x0004) {
		return NULL;
	}
	
	if( SDL_SwapLE16(*(unsigned short *)(Filedata + 4)) != 0xFA00) {
		return NULL;
	}
	
	Uint16 PaletteSize = SDL_SwapLE16(*((unsigned short *)(Filedata + 8)));
	
	if( (ImageOut = (unsigned char*) calloc(1,SIZE_X*SIZE_Y)) == NULL) {
		return NULL;
	}
	
	if(decode80(Filedata + 10 + PaletteSize,ImageOut,0) == -2) {
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
		memcpy(	((char*) (pic->pixels)) + y * pic->pitch , ImageOut + y * SIZE_X, SIZE_X);
	}
		
	SDL_UnlockSurface(pic);

	free(ImageOut);
	return pic;
}
