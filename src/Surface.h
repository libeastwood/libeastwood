#ifndef EASTWOOD_SURFACE_H
#define EASTWOOD_SURFACE_H

#include <ostream>

namespace eastwood {

struct Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

typedef Color Palette[256];

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
	Surface() : _bpp(0), _width(0), _height(0), _pitch(0), _pixels(NULL), _palette(NULL) {};
	Surface(uint16_t width, uint16_t height, uint8_t bpp, Palette *palette);
	Surface(const uint8_t *buffer, uint16_t width, uint16_t height, uint8_t bpp, Palette *palette);
	virtual ~Surface();

	bool scalePrecondition(Scaler scale);
	Surface* getScaled(Scaler scale);
	bool SaveBMP(std::ostream &output);
	inline Point size() {
	    Point point = { _width, _height };
	    return point;
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
	uint8_t *_pixels;
	Palette *_palette;
};
}
#endif // EASTWOOD_SURFACE_H
