#include "StdDef.h"
#include "Surface.h"

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

}
