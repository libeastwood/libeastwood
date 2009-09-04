#include "StdDef.h"
#include "Surface.h"
#include "Exception.h"

#include "scaler/scalebit.h"

namespace eastwood {

Surface::Surface(uint16_t width, uint16_t height, uint8_t bpp, Palette *palette) :
    _bpp(bpp), _width(width), _height(height), _pitch(width*(bpp/8)), _pixels(NULL), _palette(palette)
{
    _pixels = new uint8_t[_pitch * _height];
}

Surface::Surface(const uint8_t *buffer, uint16_t width, uint16_t height, uint8_t bpp, Palette *palette) :
    _bpp(bpp), _width(width), _height(height), _pitch(width*(bpp/8)), _pixels(NULL), _palette(palette)
{
    _pixels = new uint8_t[_pitch * _height];
    for(int y = 0; y < _height; y++)
	memcpy(((uint8_t*)(_pixels)) + y * _pitch , buffer + y * _width, _width);

}

Surface::~Surface() {
    //FIXME:
#if 0
    if(_pixels)
    	delete [] _pixels;
    if(_palette)
	delete [] _palette;
#endif
}

bool Surface::scalePrecondition(Scaler scaler)
{
    return scale_precondition(scaler, _bpp/8, _width, _height);
}

Surface Surface::getScaled(Scaler scaler)
{
    Surface scaled;
    switch(scaler) {
	case Scale2X:
	case Scale2X3:
	case Scale2X4:
	case Scale3X:
	case Scale4X:
	    scaled = Surface(_width * (scaler & 0xff), _height * (scaler>>8), _bpp, _palette);
    	break;
	default:
	    throw(Exception(LOG_ERROR, "Surface", "getScaled(): Unsupported scaler"));
    }
    scale(scaler, scaled._pixels, scaled._pitch, _pixels - _pitch, _pitch, _bpp/8, _width, _height);
    return scaled;
}

}
