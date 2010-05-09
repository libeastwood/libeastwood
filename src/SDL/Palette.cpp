#include "eastwood/StdDef.h"
#include "eastwood/SDL/Palette.h"
#include "eastwood/Exception.h"

namespace eastwood { namespace SDL {

Palette::Palette(const eastwood::Palette& palette) :
    eastwood::Palette(palette)
{
}

Palette::Palette(const SDL_Palette *palette) :
    eastwood::Palette(palette->ncolors)
{
    for(uint32_t i = 0; i < size(); i++) {
	_palette[i].r = palette->colors[i].r;
	_palette[i].g = palette->colors[i].g;
	_palette[i].b = palette->colors[i].b;
	_palette[i].unused = palette->colors[i].unused;
    }	
}


Palette::~Palette()
{
}

}}


