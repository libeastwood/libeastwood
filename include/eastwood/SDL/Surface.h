#ifndef EASTWOOD_SDL_SURFACE_H
#define EASTWOOD_SDL_SURFACE_H

#include <SDL_video.h>

#include "eastwood/Decode.h"
#include "eastwood/SDL/Palette.h"

namespace eastwood { namespace SDL {

class Surface : public eastwood::Surface
{
    public:
	Surface() : _surface(NULL) {};
	Surface(const eastwood::Surface& surface, uint32_t flags = SDL_HWSURFACE,
		uint32_t Rmask = 0, uint32_t Gmask = 0, uint32_t Bmask = 0, uint32_t Amask = 0);
	Surface(uint16_t width, uint16_t height, uint8_t bpp, Palette palette, uint32_t flags = SDL_HWSURFACE,
		uint32_t Rmask = 0, uint32_t Gmask = 0, uint32_t Bmask = 0, uint32_t Amask = 0);
        
	Surface(const SDL_Surface *surface);
	virtual ~Surface();

	virtual operator SDL_Surface*() const {
	    return _surface;
	}

	Surface &operator=(const eastwood::Surface &surface);
	Surface &operator=(const SDL_Surface *surface);


    protected:
	bool setPalette(eastwood::Palette &palette, int firstColor = 0, int flags = (SDL_LOGPAL|SDL_PHYSPAL));

	SDL_Surface *_surface;
};

}}

#endif // EASTWOOD_SDL_SURFACE_H
