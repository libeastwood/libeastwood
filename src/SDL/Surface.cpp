#include "StdDef.h"
#include "SDL/Surface.h"
#include "Exception.h"

#include <algorithm>

namespace eastwood { namespace SDL {

SDL_Palette* convertPalette(eastwood::Palette *palette)
{    
    SDL_Palette *newPalette = new SDL_Palette;

    newPalette->ncolors = sizeof(*palette)/sizeof(*palette[0]);

    newPalette->colors = new SDL_Color[newPalette->ncolors];

    for (int i = 0; i < newPalette->ncolors; i++){
	newPalette->colors[i].r = palette[i]->r;
	newPalette->colors[i].g = palette[i]->g;
	newPalette->colors[i].b = palette[i]->b;
	newPalette->colors[i].unused = 0;
    }

    return newPalette;
}

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
    return convertPalette(_palette);
}

}}

