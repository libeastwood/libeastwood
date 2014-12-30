#include <iostream>
#include <string>
#include <vector>

#include "eastwood/StdDef.h"

#include "eastwood/CpsFile.h"
#include "eastwood/Exception.h"
#include "eastwood/Log.h"
#include "eastwood/PalFile.h"

namespace eastwood {

CpsFile::CpsFile(std::istream &stream, Palette palette) :
    Decode(stream, 320, 200, palette), _format(UNCOMPRESSED)
{
    if(static_cast<uint16_t>(_stream.getU16LE()+_stream.gcount()) != _stream.sizeg())
	throw(Exception(LOG_ERROR, __FUNCTION__, "Invalid file size"));

    _format = static_cast<compressionFormat>(_stream.getU16LE());
    switch(_format) {
	case UNCOMPRESSED:
	case FORMAT_80:
	    break;
	case FORMAT_LBM:
	    throw(Exception(LOG_ERROR, __FUNCTION__, "LBM format support not implemented"));
	default:
	    throw(Exception(LOG_ERROR, __FUNCTION__, "Format not supported: %x", _format));
    }

    if(_stream.getU16LE() + _stream.getU16LE() != _width*_height)
	throw(Exception(LOG_ERROR, __FUNCTION__, "Invalid image size"));

    if(_stream.getU16LE() == 768){
	LOG_INFO("%s", "CPS has embedded palette");
	if(palette)
	    _stream.ignore(768);
	else {
    	    PalFile pal(_stream);
    	    _palette = pal.getPalette();
	}
    }
    else if(!_palette)
	throw(Exception(LOG_ERROR, __FUNCTION__, "No palette provided as argument or embedded in CPS"));
}

CpsFile::~CpsFile()
{	
}

Surface eastwood::CpsFile::getSurface()
{
    Surface pic(_width, _height, 8, _palette);

    switch(_format) {
	case UNCOMPRESSED:
	    _stream.read(reinterpret_cast<char*>(static_cast<uint8_t*>(pic)), pic.size());
	    break;
	case FORMAT_LBM:
	    //TODO: implement?
	    throw(Exception(LOG_ERROR, __FUNCTION__, "LBM format not yet supported"));
	    break;
	case FORMAT_80:
    	if(decode80(pic,0) == -2)
    	    throw(Exception(LOG_ERROR, __FUNCTION__, "Cannot decode Cps-File"));
	break;
	default:
	    throw(Exception(LOG_ERROR, __FUNCTION__, "Unknown format"));
    }

    return pic;
}

}
