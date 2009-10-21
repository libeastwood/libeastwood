#ifndef EASTWOOD_SDL_PALETTE_H
#define EASTWOOD_SDL_PALETTE_H

#include <SDL_video.h>

#include "eastwood/Palette.h"

namespace eastwood { namespace SDL {

class Palette : public eastwood::Palette
{
    public:
	Palette(const eastwood::Palette& palette);
	Palette(const SDL_Palette *palette);
	virtual ~Palette();

	virtual operator SDL_Color*() {
	    return reinterpret_cast<SDL_Color*>(&_palette.front());
	}
};

}}

#endif // EASTWOOD_SDL_PALETTE_H

