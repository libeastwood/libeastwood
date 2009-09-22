#include <iostream>
#include <string>
#include <vector>
#include <SDL.h>

#include "StdDef.h"

#include "CpsFile.h"
#include "Exception.h"
#include "Log.h"
#include "PalFile.h"

namespace eastwood {

CpsFile::CpsFile(const std::istream &stream, Palette *palette) :
    Decode(stream, 320, 200, palette), _format(UNCOMPRESSED), _imageSize(0)
{
    if((uint16_t)(_stream.getU16LE()+_stream.gcount()) != _stream.size())
	throw(Exception(LOG_ERROR, "CpsFile", "Invalid file size"));

    _format = (compressionFormat)_stream.getU16LE();
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

    if((_imageSize = _stream.getU16LE()) != _width*_height)
	throw(Exception(LOG_ERROR, "CpsFile", "Invalid image size"));

    _stream.ignore(2);

    if(_stream.getU16LE()== sizeof(Palette)){
	LOG_INFO("CpsFile", "CPS has embedded palette, loading...");
	PalFile pal(_stream);
	_palette = pal.getPalette();
    }
}

CpsFile::~CpsFile()
{	
}

Surface eastwood::CpsFile::getSurface()
{
    uint8_t *imageOut = new uint8_t[_imageSize];

    switch(_format) {
	case UNCOMPRESSED:
	    _stream.read(reinterpret_cast<char*>(imageOut), _imageSize);
	    break;
	case FORMAT_LBM:
	    //TODO: implement?
	    break;
	case FORMAT_80:
    	if(decode80(imageOut,0) == -2)
    	    throw(Exception(LOG_ERROR, "CpsFile", "Cannot decode Cps-File"));
	break;
    }
    Surface pic(imageOut, _width, _height, 8, _palette);    

    return pic;
}

}
