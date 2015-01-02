#include "eastwood/PcxFile.h"
#include "eastwood/StdDef.h"
#include "eastwood/Exception.h"
#include "eastwood/PalFile.h"


namespace eastwood {

const uint8_t ZSOFT_SIG = 0x0A;    //zsoft pcx format signature
const int OFFSET = 128;         //size of pcx header

PcxFile::PcxFile(std::istream &stream):
Decode(stream, 0, 0), _format(V30_STD)
{
    uint16_t x_min,
             y_min, 
             x_max, 
             y_max;
    uint16_t linebytes;
    uint8_t bpp;
    
    if(_stream.get() != ZSOFT_SIG)
        throw(Exception(LOG_ERROR, "PcxFile", "Not a zSoft PCX"));
    
    _format = static_cast<formatVersion>(_stream.get());
    if(_format != V30_STD)
        throw(Exception(LOG_ERROR, "PcxFile", "Formats other than 5 not handled"));
    
    if(_stream.get() != 1)
        throw(Exception(LOG_ERROR, "PcxFile", "Encoding byte not correct"));
    
    bpp = _stream.get();
    if(bpp < 8)
        throw(Exception(LOG_ERROR, "PcxFile", "PCX of bit depth < 8 not handled"));
    //_Bpp = _bpp / 8;
    
    x_min = _stream.getU16LE();
    y_min = _stream.getU16LE();
    x_max = _stream.getU16LE();
    y_max = _stream.getU16LE();
    
    //ignore resolution and 16 color palette
    _stream.seekg(53, std::ios_base::cur);
    
    //again make sure we have a 256 colour image
    if(_stream.get() != 1)
        throw(Exception(LOG_ERROR, "PcxFile", "Bitplanes > 1 not supported"));
    
    linebytes = _stream.getU16LE();
    _height = (y_max - y_min) + 1;
    _width = linebytes;
    
    _stream.seekg(_stream.sizeg() - 768, std::ios_base::beg);
    PalFile pal(_stream, true);
    _palette = pal.getPalette();
}

Surface PcxFile::getSurface()
{
    Surface pic(_width, _height, 8, _palette);
    
    _stream.seekg(OFFSET, std::ios_base::beg);
    
    decodeRLE(pic);

    return pic;
}

PcxFile::~PcxFile() {
    
}

} //eastwood
