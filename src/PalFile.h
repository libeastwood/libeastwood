#ifndef EASTWOOD_PALETTE_H
#define EASTWOOD_PALETTE_H

#include <istream>

struct Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

typedef Color Palette[256];

class SDL_Palette;

class PalFile
{
    public:
	PalFile(std::istream &stream);
	virtual ~PalFile();

	SDL_Palette *getPalette();

    private:
	Palette _palette;
	SDL_Palette *_SDLPalette;

};

#endif // EASTWOOD_PALETTE_H
