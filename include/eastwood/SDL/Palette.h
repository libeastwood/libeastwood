#ifndef EASTWOOD_SDL_PALETTE_H
#define EASTWOOD_SDL_PALETTE_H

#include <SDL_video.h>

#include "eastwood/Palette.h"

namespace eastwood { namespace SDL {

class Palette : public eastwood::Palette
{
    public:
	Palette(const eastwood::Palette& palette);
	Palette(const SDL_Palette& palette);
	virtual ~Palette();

	virtual operator SDL_Palette*() const {
	    return _sdlPalette;
	}

	virtual operator SDL_Color*() const {
	    return _sdlPalette->colors;
	}


    protected:
	SDL_Palette *_sdlPalette;
};

}}

#endif // EASTWOOD_SDL_PALETTE_H

