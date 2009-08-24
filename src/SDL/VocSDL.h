#ifndef EASTWOOD_SDL_VOCSDL_H
#define EASTWOOD_SDL_VOCSDL_H

#include <SDL_mixer.h>
#include "VocFile.h"

class VocSDL : VocFile
{
    public:
	VocSDL(std::istream &stream, int targetFrequency, int channels, AudioFormat format, int quality = 4);
	VocSDL(std::istream &stream, int quality = 4);	
	~VocSDL();

	Mix_Chunk *get();

    private:
	Mix_Chunk *_mixChunk;

};

#endif // EASTWOOD_SDL_VOCSDL_H
