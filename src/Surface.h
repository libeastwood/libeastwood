#ifndef EASTWOOD_SURFACE_H
#define EASTWOOD_SURFACE_H

namespace eastwood {

struct Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

typedef Color Palette[256];

class Surface {
    public:
	Surface(uint16_t width, uint16_t height, uint8_t bpp, Palette *palette);
	Surface(const uint8_t *buffer, uint16_t width, uint16_t height, uint8_t bpp, Palette *palette);
	virtual ~Surface();

    protected:
	friend class CpsFile;
	friend class ShpFile;
	friend class WsaFile;

	uint8_t _bpp;
	uint16_t _width,
		 _height,
		 _pitch;
	uint8_t *_pixels;
	Palette *_palette;
};
}
#endif // EASTWOOD_SURFACE_H
