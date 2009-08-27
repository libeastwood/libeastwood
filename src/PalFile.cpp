#include <istream>
#include <SDL_video.h>

#include "StdDef.h"
#include "Log.h"

#include "PalFile.h"

PalFile::PalFile(std::istream &stream) : _SDLPalette(NULL)
{
    for (uint16_t i = 0; i < sizeof(_palette)/sizeof(_palette[0]); i++){
	_palette[i].r = stream.get()<<2;
	_palette[i].g = stream.get()<<2;
	_palette[i].b = stream.get()<<2;
    }
}

PalFile::~PalFile()
{
}

SDL_Palette *PalFile::getPalette()
{
    SDL_Palette *palette = new SDL_Palette;

    palette->ncolors = sizeof(_palette)/sizeof(_palette[0]);

    palette->colors = new SDL_Color[palette->ncolors];

    for (int i = 0; i < palette->ncolors; i++){
	palette->colors[i].r = _palette[i].r;
	palette->colors[i].g = _palette[i].g;
	palette->colors[i].b = _palette[i].b;
	palette->colors[i].unused = 0;
    }

    return palette;
}
