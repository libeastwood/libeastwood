#include <iostream>
#include <string>
#include <SDL.h>

#include "StdDef.h"

#include "CpsFile.h"
#include "Exception.h"
#include "Log.h"
#include "PalFile.h"

using namespace eastwood;

CpsFile::CpsFile(std::istream &stream, SDL_Palette *palette) : Decode(stream, 320, 200, palette)
{
    _stream.ignore(2);
    if(readU16LE(_stream) != 0x0004 ||
	    readU16LE(_stream) != 0xFA00)
	throw(Exception(LOG_ERROR, "CpsFile", "Invalid header"));

    _stream.ignore(2);

    if(readU16LE(_stream)== sizeof(Palette)){
	LOG_INFO("CpsFile", "CPS has embedded palette, loading...");
	PalFile pal(_stream);
	_palette = pal.getPalette();
    }
}

CpsFile::~CpsFile()
{	
}

SDL_Surface *CpsFile::getSurface()
{
    uint8_t *buffer, *ImageOut;
    SDL_Surface *pic = NULL;
    uint32_t size;
    std::streampos pos;

    pos = _stream.tellg();
    _stream.seekg(0, std::ios::end);	
    size = static_cast<std::streamoff>(_stream.tellg()) - static_cast<std::streamoff>(pos);
    _stream.seekg(pos);
    buffer = new uint8_t[size];
    _stream.read((char*)buffer, size);
    _stream.seekg(pos);

    ImageOut = new uint8_t[_width*_height];

    if(decode80(buffer,ImageOut,0) == -2)
	throw(Exception(LOG_ERROR, "CpsFile", "Cannot decode Cps-File"));

    delete [] buffer;

    pic = createSurface(ImageOut, SDL_SWSURFACE);

    delete [] ImageOut;
    return pic;
}
