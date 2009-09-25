#include "eastwood/StdDef.h"
#include "eastwood/SDL/Palette.h"
#include "eastwood/Exception.h"

namespace eastwood { namespace SDL {

Palette::Palette(const eastwood::Palette& palette) :
    eastwood::Palette(palette)
{
    ncolors = size();
    colors = reinterpret_cast<SDL_Color*>(&_palette[0]);
}

Palette::Palette(const SDL_Palette& palette) :
    eastwood::Palette(palette.ncolors)
{
    for(uint32_t i = 0; i < size(); i++)
	_palette[i] = *(reinterpret_cast<eastwood::Color*>(&palette.colors[i]));
}


Palette::~Palette()
{
}

}}


