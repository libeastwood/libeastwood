#ifndef EASTWOOD_SDL_PALETTE_H
#define EASTWOOD_SDL_PALETTE_H

#include <SDL_video.h>

#include "eastwood/StdDef.h"
#include "eastwood/Palette.h"

namespace eastwood { namespace SDL {

class Palette : public eastwood::Palette
{
    public:
	Palette(const eastwood::Palette& palette) :
	    eastwood::Palette(palette) {}

	Palette(const SDL_Palette *palette) :
	    eastwood::Palette(palette->ncolors) {
		for(uint32_t i = 0; i < size(); i++) {
		    _palette[i].r = palette->colors[i].r;
		    _palette[i].g = palette->colors[i].g;
		    _palette[i].b = palette->colors[i].b;
		    _palette[i].unused = palette->colors[i].unused;
		}	
	    }

	virtual ~Palette() {}

	virtual operator SDL_Color*() {
	    return reinterpret_cast<SDL_Color*>(&_palette.front());
	}
};

}}

#endif // EASTWOOD_SDL_PALETTE_H

