#ifndef EASTWOOD_SDL_VOCSDL_H
#define EASTWOOD_SDL_VOCSDL_H

#include <SDL_mixer.h>
#include "VocFile.h"

class VocSDL : VocFile
{
    public:
	VocSDL(std::istream &stream, int targetFrequency, int channels, AudioFormat format);
	VocSDL(std::istream &stream);	
	~VocSDL();

	Mix_Chunk *getMixChunk(Interpolator interpolator = I_LINEAR);

};

#endif // EASTWOOD_SDL_VOCSDL_H
