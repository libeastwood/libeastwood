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
	Surface(const eastwood::Surface& surface,
		uint32_t Rmask = 0, uint32_t Gmask = 0, uint32_t Bmask = 0, uint32_t Amask = 0) :
	    eastwood::Surface(surface.width(), surface.height(), surface.bpp(), surface.palette()),
	    _surface(SDL_CreateRGBSurfaceFrom(*this, surface.width(), surface.height(),
			surface.bpp(), surface.pitch(), Rmask, Gmask, Bmask, Amask)) {
		memcpy(_surface->pixels, surface, surface.size());
		setPalette(_palette);
	    }

	Surface(uint16_t width, uint16_t height, uint8_t bpp, Palette palette,
		uint32_t Rmask = 0, uint32_t Gmask = 0, uint32_t Bmask = 0, uint32_t Amask = 0) :
	    eastwood::Surface(width, height, bpp, palette),
	    _surface(SDL_CreateRGBSurfaceFrom(*this, width, height, bpp,
			_pitch, Rmask, Gmask, Bmask, Amask)) {
		setPalette(_palette);
	    }

	Surface(const SDL_Surface *surface) :
	    eastwood::Surface(surface->w, surface->h, surface->format->BitsPerPixel, SDL::Palette(surface->format->palette)),
	    _surface(SDL_ConvertSurface(const_cast<SDL_Surface*>(surface), surface->format, surface->flags)) {
		_pixels.reset(new Bytes(reinterpret_cast<uint8_t*>(_surface->pixels), (surface->flags & SDL_PREALLOC) ? BufNewArray : BufMalloc));
	    }

	virtual ~Surface() {
	    SDL_FreeSurface(_surface);
	}

	virtual operator SDL_Surface*() const {
	    return _surface;
	}

	Surface &operator=(const eastwood::Surface &surface) {
	    *(static_cast<eastwood::Surface*>(this)) = surface;

	    _surface = SDL_CreateRGBSurfaceFrom(*this, _width, _height, _bpp, _pitch, 0, 0, 0, 0);

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
	    _pixels.reset(new Bytes(reinterpret_cast<uint8_t*>(_surface->pixels), (surface->flags & SDL_PREALLOC) ? BufNewArray : BufMalloc));
	    if(surface->format->palette != NULL)
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
