#ifndef EASTWOOD_SDL_SURFACE_H
#define EASTWOOD_SDL_SURFACE_H

#include <SDL/SDL_video.h>

#include "Decode.h"
#include "PalFile.h"

namespace eastwood { namespace SDL {

class Surface : public eastwood::Surface
{
    public:
	Surface(const eastwood::Surface& surface);
	virtual ~Surface();

    	::SDL_Surface *get(uint32_t flags);
	::SDL_Palette *getPalette();

};

}}

#endif // EASTWOOD_SDL_SURFACE_H
