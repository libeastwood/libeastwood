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

class Surface {
    public:
	Surface() : _bpp(0), _width(0), _height(0), _pitch(0), _pixels(), _palette(0) {};
	Surface(uint16_t width, uint16_t height, uint8_t bpp, Palette palette);
	Surface(uint8_t *buffer, uint16_t width, uint16_t height, uint8_t bpp, Palette palette);
	virtual ~Surface();

	Surface(const Surface &surface);

	virtual operator uint8_t*() const {
	    return *_pixels.get();
	}

	uint16_t width() const throw() { return _width; }
	uint16_t height() const throw() { return _height; }
	uint16_t pitch() const throw() { return _pitch; }
	uint8_t bpp() const throw() { return _bpp; }
	Palette palette() const throw() { return _palette; }
	uint32_t size() const throw() { return _pitch * _height; }

	bool scalePrecondition(Scaler scale);
	Surface getScaled(Scaler scale);
	bool saveBMP(std::ostream &output);

    protected:
	uint8_t _bpp;
	uint16_t _width,
		 _height,
		 _pitch;
	BytesPtr _pixels;
	Palette _palette;
};
}
#endif // EASTWOOD_SURFACE_H
