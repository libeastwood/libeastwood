#include <iostream>
#include <string>
#include <SDL.h>

#include "StdDef.h"

#include "CpsFile.h"
#include "Exception.h"
#include "Log.h"

#define	SIZE_X	320
#define SIZE_Y	200

using namespace eastwood;

CpsFile::CpsFile(std::istream &stream, SDL_Palette *palette) : Decode(), _stream(stream)
{
    _stream.seekg(2, std::ios::beg);
    if(htole16(_stream.get() | _stream.get() << 8) != 0x0004 ||
	    htole16(_stream.get() | _stream.get() << 8) != 0xFA00)
	throw(Exception(LOG_ERROR, "CpsFile", "Invalid header"));

    _stream.seekg(2, std::ios::cur);
    uint16_t paletteSize = htole16(_stream.get() | _stream.get() << 8);

    if(paletteSize == 768){
	LOG_INFO("CpsFile", "CPS has embedded palette, loading...");
	m_palette = new SDL_Palette;
	m_palette->ncolors = paletteSize / 3;
	m_palette->colors = new SDL_Color[m_palette->ncolors];

	for (int i = 0; i < m_palette->ncolors; i++){
	    m_palette->colors[i].r = _stream.get() <<2;
	    m_palette->colors[i].g = _stream.get() <<2;
	    m_palette->colors[i].b = _stream.get() <<2;
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
    uint8_t *buffer, *ImageOut;
    SDL_Surface *pic = NULL;
    uint32_t pos,
	     size;

    pos = (uint32_t)_stream.tellg();
    _stream.seekg(0, std::ios::end);	
    size = (uint32_t)_stream.tellg() - pos;
    _stream.seekg(pos, std::ios::beg);
    buffer = new uint8_t[size];
    _stream.read((char*)buffer, size);
    _stream.seekg(pos, std::ios::beg);


    if( (ImageOut = (uint8_t*) calloc(1,SIZE_X*SIZE_Y)) == NULL) {
	return NULL;
    }

    if(decode80(buffer,ImageOut,0) == -2) {
	LOG_ERROR("CpsFile", "Cannot decode Cps-File");
    }
    delete [] buffer;

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
