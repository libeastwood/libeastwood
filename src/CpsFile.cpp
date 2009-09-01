#include <iostream>
#include <string>
#include <vector>
#include <SDL.h>

#include "StdDef.h"

#include "CpsFile.h"
#include "Exception.h"
#include "Log.h"
#include "PalFile.h"

using namespace eastwood;

CpsFile::CpsFile(std::istream &stream, SDL_Palette *palette) :
    Decode(stream, 320, 200, palette), _format(UNCOMPRESSED), _imageSize(0)
{
    if((uint16_t)(readU16LE(_stream)+_stream.gcount()) != getStreamSize(_stream))
	throw(Exception(LOG_ERROR, "CpsFile", "Invalid file size"));

    _format = (compressionFormat)readU16LE(_stream);
    switch(_format) {
	case UNCOMPRESSED:
	case FORMAT_80:
	    break;
	case FORMAT_LBM:
	    throw(Exception(LOG_ERROR, "CpsFile", "LBM format support not implemented"));
	default:
	    char error[256];
	    snprintf(error, sizeof(error), "Format not supported: %x", _format);
	    throw(Exception(LOG_ERROR, "CpsFile", error));
    }

    if((_imageSize = readU16LE(_stream)) != _width*_height)
	throw(Exception(LOG_ERROR, "CpsFile", "Invalid image size"));

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
    std::vector<uint8_t> ImageOut(_imageSize);

    if(decode80(&ImageOut.front(),0) == -2)
	throw(Exception(LOG_ERROR, "CpsFile", "Cannot decode Cps-File"));

    return createSurface(&ImageOut.front(), SDL_SWSURFACE);
}
