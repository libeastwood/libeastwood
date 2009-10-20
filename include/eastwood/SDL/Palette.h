#ifndef EASTWOOD_SDL_PALETTE_H
#define EASTWOOD_SDL_PALETTE_H

#include <SDL_video.h>

#include "eastwood/Palette.h"

namespace eastwood { namespace SDL {

class Palette : public eastwood::Palette, public SDL_Palette
{
    public:
	Palette(const eastwood::Palette& palette);
	Palette(const SDL_Palette& palette);
	virtual ~Palette();
};

}}

#endif // EASTWOOD_SDL_PALETTE_H

