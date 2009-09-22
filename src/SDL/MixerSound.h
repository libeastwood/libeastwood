#ifndef EASTWOOD_SDL_MIXERSOUND_H
#define EASTWOOD_SDL_MIXERSOUND_H

#include <SDL_mixer.h>
#include "Sound.h"

namespace eastwood { namespace SDL {

class MixerSound : public Sound, public Mix_Chunk
{
    public:
	MixerSound();
	MixerSound(uint32_t size, uint8_t *buffer, uint32_t frequency, uint8_t channels, AudioFormat format);
	MixerSound(uint32_t size, uint8_t *buffer);
	MixerSound(const eastwood::Sound &sound);
	virtual ~MixerSound();

	MixerSound getResampled(Interpolator interpolator = I_LINEAR);

    private:
	Mix_Chunk *_sound;

};

}}
#endif // EASTWOOD_SDL_VOCSDL_H
