#include "eastwood/StdDef.h"
#include "eastwood/SDL/Surface.h"
#include "eastwood/Exception.h"

namespace eastwood { namespace SDL {

static SDL_Surface *tmp = NULL;
Surface::Surface(const eastwood::Surface& surface, uint32_t flags,
	uint32_t Rmask, uint32_t Gmask, uint32_t Bmask, uint32_t Amask) :
    eastwood::Surface(surface),
    SDL_Surface(*(tmp = SDL_CreateRGBSurface(flags, surface.size().x, surface.size().y, surface.bpp(), Rmask, Gmask, Bmask, Amask))),
    _surface(tmp)
{
    free(pixels);
    tmp = NULL;
    pixels = _pixels;

#define ncolors sizeof(*_palette)/sizeof(*_palette[0])
    SDL_Color colors[ncolors];

    for (uint16_t i = 0; i < ncolors; i++){
	colors[i].r = _palette[i]->r;
	colors[i].g = _palette[i]->g;
	colors[i].b = _palette[i]->b;
	colors[i].unused = 0;
    }


    SDL_SetColors(this, colors, 0, ncolors);
}

Surface::Surface(const SDL_Surface& surface) :
    _surface(NULL)
{
    _bpp = format->BitsPerPixel;
    _width = w;
    _height = h;
    _pitch = pitch;
    _pixelsPtr.reset(new Bytes(reinterpret_cast<uint8_t*>(pixels)));
    _pixels = *_pixelsPtr.get();

    for (uint16_t i = 0; i < sizeof(*_palette)/sizeof(*_palette[0]); i++){
	_palette[i]->r = surface.format->palette->colors[i].r;
	_palette[i]->g = surface.format->palette->colors[i].g;
	_palette[i]->b = surface.format->palette->colors[i].b;
    }
}


Surface::~Surface()
{
    if(_surface) {
    	// This (which actually is pointing to the same are as _pixels) will be
	// freed in parent destructor
	_surface->pixels = NULL;
	SDL_FreeSurface(_surface);
    }
}

}}

