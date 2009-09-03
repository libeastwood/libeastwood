#include "StdDef.h"
#include "SDL/Surface.h"
#include "Exception.h"

#include <algorithm>

namespace eastwood { namespace SDL {

Surface::Surface(const eastwood::Surface& surface) : eastwood::Surface(surface)
{
}

Surface::~Surface()
{
}

::SDL_Surface *Surface::get(uint32_t flags)
{
    ::SDL_Surface *surface;
    if((surface = SDL_CreateRGBSurface(flags, _width, _height, _bpp, 0, 0, 0, 0))== NULL)
	throw(Exception(LOG_ERROR, "SDL::Surface", "Unable to create SDL_Surface"));

    ::SDL_LockSurface(surface);

    memcpy(surface->pixels, _pixels, _pitch * _height);

    ::SDL_UnlockSurface(surface);

    ::SDL_Palette *palette = getPalette();

    ::SDL_SetColors(surface, palette->colors, 0, palette->ncolors);
    return surface;
}


::SDL_Palette *Surface::getPalette()
{
    ::SDL_Palette *palette = new ::SDL_Palette;

    palette->ncolors = sizeof(*_palette)/sizeof(*_palette[0]);

    palette->colors = new ::SDL_Color[palette->ncolors];

    for (int i = 0; i < palette->ncolors; i++){
	palette->colors[i].r = _palette[i]->r;
	palette->colors[i].g = _palette[i]->g;
	palette->colors[i].b = _palette[i]->b;
	palette->colors[i].unused = 0;
    }

    return palette;
}

}}

