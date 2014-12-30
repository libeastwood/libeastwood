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
	Surface() : _bpp(0), _Bpp(0), _width(0), _height(0), _pitch(0), _pixels(), _palette(0) {};
	Surface(int32_t width, int32_t height, uint8_t bpp, Palette palette);
	Surface(uint8_t *buffer, int32_t width, int32_t height, uint8_t bpp, Palette palette);
	virtual ~Surface();

	Surface(const Surface &surface);

	virtual operator uint8_t*() const {
	    return *_pixels.get();
	}
	virtual operator void*() const {
            return *_pixels.get();
        }

	virtual operator bool() const {
	    return _bpp != 0;
	}

	auto width() const noexcept { return _width; }
	auto height() const noexcept { return _height; }
	auto pitch() const noexcept { return _pitch; }
	auto bpp() const noexcept { return _bpp; }
	auto Bpp() const noexcept { return _Bpp; }
	Palette palette() const noexcept { return _palette; }
	auto size() const noexcept { return _pitch * _height; }

	bool scalePrecondition(Scaler scale);
	Surface getScaled(Scaler scale);
	bool saveBMP(std::ostream &output);

    protected:
	uint8_t _bpp,
		_Bpp;
	int32_t _width,
		 _height;
	uint16_t _pitch;
	BytesPtr _pixels;
	Palette _palette;
};
}
#endif // EASTWOOD_SURFACE_H
