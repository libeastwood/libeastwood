#ifndef EASTWOOD_SDL_MIXER_SOUND_H
#define EASTWOOD_SDL_MIXER_SOUND_H

#include <SDL_mixer.h>

#include "eastwood/Sound.h"

namespace eastwood { namespace SDL { namespace Mixer {

class Sound : public eastwood::Sound
{
    public:
	Sound();
	Sound(uint32_t size, uint8_t *buffer, uint32_t frequency, uint8_t channels, AudioFormat format);
	Sound(const eastwood::Sound &sound);
	Sound(const Mix_Chunk *sound);
	virtual ~Sound();

	virtual operator Mix_Chunk*() const throw() {
	    return _chunk.get();
	}

	Sound &operator=(const eastwood::Sound &sound);
	Sound &operator=(const Mix_Chunk *sound);

	Sound getResampled(Interpolator interpolator = I_LINEAR);

    private:
	std::tr1::shared_ptr<Mix_Chunk> _chunk;
};

}}}
#endif // EASTWOOD_SDL_MIXER_SOUND_H
