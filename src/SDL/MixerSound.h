#ifndef EASTWOOD_SDL_MIXERSOUND_H
#define EASTWOOD_SDL_MIXERSOUND_H

#include <SDL_mixer.h>
#include "Sound.h"

namespace eastwood {

class MixerSound : Sound
{
    public:
	MixerSound(size_t size, uint8_t *buffer, uint32_t frequency, uint8_t channels, AudioFormat format);
	MixerSound(size_t size, uint8_t *buffer);

	MixerSound(std::istream &stream);	

	Mix_Chunk *getMixChunk(Interpolator interpolator = I_LINEAR);

};

}
#endif // EASTWOOD_SDL_VOCSDL_H
