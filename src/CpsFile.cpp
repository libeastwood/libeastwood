#include <iostream>
#include <string>
#include <vector>
#include <SDL.h>

#include "eastwood/StdDef.h"

#include "eastwood/CpsFile.h"
#include "eastwood/Exception.h"
#include "eastwood/Log.h"
#include "eastwood/PalFile.h"

namespace eastwood {

CpsFile::CpsFile(std::istream &stream, Palette palette) :
    Decode(stream, 320, 200, palette), _format(UNCOMPRESSED)
{
    if((uint16_t)(_stream.getU16LE()+_stream.gcount()) != _stream.sizeg())
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

    if(_stream.getU16LE() + _stream.getU16LE() != _width*_height)
	throw(Exception(LOG_ERROR, "CpsFile", "Invalid image size"));

    if(_stream.getU16LE() == 768){
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
    uint16_t imageSize = _width * _height;
    uint8_t *imageOut = new uint8_t[imageSize];

    switch(_format) {
	case UNCOMPRESSED:
	    _stream.read(reinterpret_cast<char*>(imageOut), imageSize);
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
