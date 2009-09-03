#include <istream>
#include <SDL_video.h>

#include "StdDef.h"
#include "Log.h"

#include "PalFile.h"

namespace eastwood {

PalFile::PalFile(std::istream &stream)
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

Palette *PalFile::getPalette()
{
    Palette *palette = new Palette[sizeof(_palette)/sizeof(_palette[0])];
    for (uint16_t i = 0; i < sizeof(_palette)/sizeof(_palette[0]); i++)
	*palette[i] = _palette[i];

    return palette;
}

}
