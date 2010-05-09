#ifndef EASTWOOD_SDL_SURFACE_H
#define EASTWOOD_SDL_SURFACE_H

#include <SDL_video.h>

#include "eastwood/StdDef.h"
#include "eastwood/Decode.h"
#include "eastwood/SDL/Palette.h"

namespace eastwood { namespace SDL {

class Surface : public eastwood::Surface
{
    public:
	Surface() : _surface(NULL) {};
	Surface(const eastwood::Surface& surface, uint32_t flags = SDL_HWSURFACE,
		uint32_t Rmask = 0, uint32_t Gmask = 0, uint32_t Bmask = 0, uint32_t Amask = 0) :
	    eastwood::Surface(NULL, surface.width(), surface.height(), surface.bpp(), surface.palette()),
	    _surface(SDL_CreateRGBSurface(flags, surface.width(), surface.height(), surface.bpp(), Rmask, Gmask, Bmask, Amask)) {
		memcpy(_surface->pixels, surface, surface.size());
		_pixels.reset(new Bytes((uint8_t*)_surface->pixels, BufMalloc));

		setPalette(_palette);
	    }

	Surface(uint16_t width, uint16_t height, uint8_t bpp, Palette palette, uint32_t flags = SDL_HWSURFACE,
		uint32_t Rmask = 0, uint32_t Gmask = 0, uint32_t Bmask = 0, uint32_t Amask = 0) :
	    eastwood::Surface(NULL, width, height, bpp, palette),
	    _surface(SDL_CreateRGBSurface(flags, width, height, bpp, Rmask, Gmask, Bmask, Amask)) {
		_pixels.reset(new Bytes((uint8_t*)_surface->pixels, BufMalloc));

		setPalette(_palette);
	    }

	Surface(const SDL_Surface *surface) :
	    eastwood::Surface(NULL, surface->w, surface->h, surface->format->BitsPerPixel, SDL::Palette(surface->format->palette)),
	    _surface(SDL_ConvertSurface(const_cast<SDL_Surface*>(surface), surface->format, surface->flags)) {
		_pixels.reset(new Bytes((uint8_t*)_surface->pixels, BufMalloc));
	    }

	virtual ~Surface() {
	    if(_surface) {
		// This (which actually is pointing to the same are as _pixels) will be
		// freed in parent destructor
		//	_surface->pixels = NULL;
		//SDL_FreeSurface(_surface);
	    }
	}

	virtual operator SDL_Surface*() const {
	    return _surface;
	}

	Surface &operator=(const eastwood::Surface &surface) {
	    *(eastwood::Surface*)this = surface;

	    _surface = SDL_CreateRGBSurface(SDL_SWSURFACE, _width, _height, _bpp, 0, 0, 0, 0);
	    free(_surface->pixels);

	    _surface->pixels = *this;

	    setPalette(_palette);

	    return *this;
	}

	Surface &operator=(const SDL_Surface *surface) {
	    _surface = const_cast<SDL_Surface*>(surface);
	    _bpp = surface->format->BitsPerPixel;
	    _Bpp = surface->format->BytesPerPixel;
	    _width = surface->w;
	    _height = surface->h;
	    _pitch = surface->pitch;
	    _pixels.reset(new Bytes((uint8_t*)_surface->pixels, BufMalloc));
	    _palette = SDL::Palette(surface->format->palette);

	    return *this;
	}


    protected:
	bool setPalette(eastwood::Palette &palette, int firstColor = 0, int flags = (SDL_LOGPAL|SDL_PHYSPAL)) {
	    Palette sdlPal = palette;
	    _palette = sdlPal;
	    return SDL_SetPalette(_surface, flags, sdlPal, firstColor, _palette.size());
	}


	SDL_Surface *_surface;
};

}}

#endif // EASTWOOD_SDL_SURFACE_H
