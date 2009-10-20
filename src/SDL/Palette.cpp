#include "eastwood/StdDef.h"
#include "eastwood/SDL/Palette.h"
#include "eastwood/Exception.h"

namespace eastwood { namespace SDL {

Palette::Palette(const eastwood::Palette& palette) :
    eastwood::Palette(palette), _sdlPalette(static_cast<SDL_Palette*>(malloc(sizeof(SDL_Palette))))
{
    _sdlPalette->ncolors = size();
    _sdlPalette->colors = reinterpret_cast<SDL_Color*>(&_palette[0]);
}

Palette::Palette(const SDL_Palette& palette) :
    eastwood::Palette(palette.ncolors), _sdlPalette(static_cast<SDL_Palette*>(malloc(sizeof(SDL_Palette))))
{
    for(uint32_t i = 0; i < size(); i++)
	_palette[i] = *(reinterpret_cast<eastwood::Color*>(&palette.colors[i]));
    _sdlPalette->ncolors = size();
    _sdlPalette->colors = reinterpret_cast<SDL_Color*>(&_palette[0]);
}


Palette::~Palette()
{
}

}}


