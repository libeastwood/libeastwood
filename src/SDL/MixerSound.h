#ifndef EASTWOOD_SDL_MIXERSOUND_H
#define EASTWOOD_SDL_MIXERSOUND_H

#include <SDL_mixer.h>
#include "Sound.h"

namespace eastwood {

class MixerSound : public Sound
{
    public:
#if 1
	MixerSound() : Sound(){};
	MixerSound(size_t size, uint8_t *buffer, uint32_t frequency, uint8_t channels, AudioFormat format);
	MixerSound(size_t size, uint8_t *buffer);
	MixerSound(const eastwood::Sound &sound);
	virtual ~MixerSound();
#endif
	

	MixerSound getResampled(Interpolator interpolator = I_LINEAR);

	Mix_Chunk *get();

};

}
#endif // EASTWOOD_SDL_VOCSDL_H
