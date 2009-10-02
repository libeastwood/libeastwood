#include "eastwood/StdDef.h"
#include "eastwood/SDL/Surface.h"
#include "eastwood/SDL/Palette.h"
#include "eastwood/Exception.h"

namespace eastwood { namespace SDL {

static SDL_Surface *tmp = NULL;
Surface::Surface(const eastwood::Surface& surface, uint32_t flags,
	uint32_t Rmask, uint32_t Gmask, uint32_t Bmask, uint32_t Amask) :
    eastwood::Surface(NULL, surface.width(), surface.height(), surface.bpp(), surface.palette()),
    SDL_Surface(*(tmp = SDL_CreateRGBSurface(flags, surface.width(), surface.height(), surface.bpp(), Rmask, Gmask, Bmask, Amask))),
    _surface(tmp)
{
    tmp = NULL;
    memcpy(pixels, (uint8_t*)surface, surface.size());
    _pixels.reset(new Bytes((uint8_t*)pixels, BufMalloc));

    setPalette(_palette);
}

Surface::Surface(const SDL_Surface& surface) :
    eastwood::Surface(NULL, surface.w, surface.h, surface.format->BitsPerPixel, SDL::Palette(*surface.format->palette)),
    SDL_Surface(*(tmp = SDL_ConvertSurface(const_cast<SDL_Surface*>(&surface), surface.format, surface.flags))),
    _surface(tmp)
{
    tmp = NULL;
    _pixels.reset(new Bytes((uint8_t*)pixels, BufMalloc));
}

Surface::~Surface()
{
    if(_surface) {
    	// This (which actually is pointing to the same are as _pixels) will be
	// freed in parent destructor
	_surface->pixels = NULL;
	//SDL_FreeSurface(_surface);
    }
}

Surface& Surface::operator=(const eastwood::Surface &surface) 
{
    *(eastwood::Surface*)this = surface;

    _surface = SDL_CreateRGBSurface(SDL_SWSURFACE, _width, _height, _bpp, 0, 0, 0, 0);
    free(_surface->pixels);

    _surface->pixels = (uint8_t*)*this;

    *(SDL_Surface*)this = *_surface;
    setPalette(_palette);

    return *this;
}

Surface& Surface::operator=(const SDL_Surface *surface) 
{
    _surface = const_cast<SDL_Surface*>(surface);
    *this = *_surface;
    _bpp = format->BitsPerPixel;
    _width = w;
    _height = h;
    _pitch = pitch;
    _pixels.reset(new Bytes((uint8_t*)surface->pixels, BufMalloc));
    _palette = SDL::Palette(*format->palette);

    return *this;
}

bool Surface::setPalette(eastwood::Palette palette, int firstColor, int flags)
{
    _palette = palette;
    return SDL_SetPalette(this, flags, reinterpret_cast<SDL_Color*>(&palette[0]), firstColor, palette.size());
}

}}

