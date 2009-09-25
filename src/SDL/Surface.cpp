#include "eastwood/StdDef.h"
#include "eastwood/SDL/Surface.h"
#include "eastwood/SDL/Palette.h"
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

    SDL::Palette palette = _palette;

    SDL_SetColors(this, palette.colors, 0, palette.ncolors);
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

    _palette = SDL::Palette(*surface.format->palette);
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

bool Surface::setPalette(eastwood::Palette palette, int firstColor, int flags)
{
    _palette = palette;
    return SDL_SetPalette(this, flags, (SDL_Color*)&palette[0], firstColor, palette.size());
}

}}

