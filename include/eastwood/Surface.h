#ifndef EASTWOOD_SURFACE_H
#define EASTWOOD_SURFACE_H

#include <ostream>
#include <memory>

#include "eastwood/Buffer.h"
#include "eastwood/Palette.h"

namespace eastwood {

enum Scaler {
    Scale2X = 2 + (2<<8),
    Scale2X3 = 2 + (3<<8),
    Scale2X4 = 2 + (4<<8),
    Scale3X = 3 + (3<<8),
    Scale4X = 4 + (4<<8)
};

struct Point {
    uint16_t x;
    uint16_t y;
};

class Surface {
    public:
	Surface() : _bpp(0), _width(0), _height(0), _pitch(0), _pixelsPtr(), _pixels(NULL), _palette(0) {};
	Surface(uint16_t width, uint16_t height, uint8_t bpp, Palette palette);
	Surface(uint8_t *buffer, uint16_t width, uint16_t height, uint8_t bpp, Palette palette);
	virtual ~Surface();

	Surface(const Surface &surface) :
	    _bpp(surface._bpp), _width(surface._width), _height(surface._height), _pitch(surface._pitch),
	    _pixelsPtr(surface._pixelsPtr), _pixels(surface._pixels), _palette(surface._palette)  {}

	Surface &operator=(const Surface &surface) {
	    _bpp = surface._bpp;
	    _width = surface._width;
	    _height = surface._height;
	    _pitch = surface._pitch;
	    _pixelsPtr = surface._pixelsPtr;
	    _pixels = surface._pixels;
	    _palette = surface._palette;
	    return *this;
	}

	bool scalePrecondition(Scaler scale);
	Surface getScaled(Scaler scale);
	bool saveBMP(std::ostream &output);

	inline Point size() const {
	    Point point = { _width, _height };
	    return point;
	}

	inline uint8_t bpp() const {
	    return _bpp;
	}

    protected:
	friend class CpsFile;
	friend class IcnFile;
	friend class ShpFile;
	friend class WsaFile;
	friend class Font;

	uint8_t _bpp;
	uint16_t _width,
		 _height,
		 _pitch;
	BytesPtr _pixelsPtr;
	uint8_t *_pixels;
	Palette _palette;
};
}
#endif // EASTWOOD_SURFACE_H
