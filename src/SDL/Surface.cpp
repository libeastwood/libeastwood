#include "eastwood/StdDef.h"
#include "eastwood/SDL/Surface.h"
#include "eastwood/SDL/Palette.h"
#include "eastwood/Exception.h"

namespace eastwood { namespace SDL {

Surface::Surface(const eastwood::Surface& surface, uint32_t flags,
	uint32_t Rmask, uint32_t Gmask, uint32_t Bmask, uint32_t Amask) :
    eastwood::Surface(NULL, surface.width(), surface.height(), surface.bpp(), surface.palette()),
    _surface(SDL_CreateRGBSurface(flags, surface.width(), surface.height(), surface.bpp(), Rmask, Gmask, Bmask, Amask))
{
    memcpy(_surface->pixels, surface, surface.size());
    _pixels.reset(new Bytes((uint8_t*)_surface->pixels, BufMalloc));

    setPalette(_palette);
}

Surface::Surface(uint16_t width, uint16_t height, uint8_t bpp, Palette palette, uint32_t flags,
	uint32_t Rmask, uint32_t Gmask, uint32_t Bmask, uint32_t Amask) :
    eastwood::Surface(NULL, width, height, bpp, palette),
    _surface(SDL_CreateRGBSurface(flags, width, height, bpp, Rmask, Gmask, Bmask, Amask))
{
    _pixels.reset(new Bytes((uint8_t*)_surface->pixels, BufMalloc));

    setPalette(_palette);
}

Surface::Surface(const SDL_Surface *surface) :
    eastwood::Surface(NULL, surface->w, surface->h, surface->format->BitsPerPixel, SDL::Palette(*surface->format->palette)),
    _surface(SDL_ConvertSurface(const_cast<SDL_Surface*>(surface), surface->format, surface->flags))
{
    _pixels.reset(new Bytes((uint8_t*)_surface->pixels, BufMalloc));
}

Surface::~Surface()
{
    if(_surface) {
    	// This (which actually is pointing to the same are as _pixels) will be
	// freed in parent destructor
//	_surface->pixels = NULL;
	//SDL_FreeSurface(_surface);
    }
}

Surface& Surface::operator=(const eastwood::Surface &surface) 
{
    *(eastwood::Surface*)this = surface;

    _surface = SDL_CreateRGBSurface(SDL_SWSURFACE, _width, _height, _bpp, 0, 0, 0, 0);
    free(_surface->pixels);

    _surface->pixels = *this;

    setPalette(_palette);

    return *this;
}

Surface& Surface::operator=(const SDL_Surface *surface) 
{
    _surface = const_cast<SDL_Surface*>(surface);
    _bpp = surface->format->BitsPerPixel;
    _Bpp = surface->format->BytesPerPixel;
    _width = surface->w;
    _height = surface->h;
    _pitch = surface->pitch;
    _pixels.reset(new Bytes((uint8_t*)_surface->pixels, BufMalloc));
    _palette = SDL::Palette(*surface->format->palette);

    return *this;
}

bool Surface::setPalette(eastwood::Palette &palette, int firstColor, int flags)
{
    Palette sdlPal = palette;
    _palette = sdlPal;
    return SDL_SetPalette(_surface, flags, sdlPal, firstColor, _palette.size());
}

}}

