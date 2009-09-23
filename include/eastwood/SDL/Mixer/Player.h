#ifndef EASTWOOD_SDL_MIXER_PLAYER_H
#define EASTWOOD_SDL_MIXER_PLAYER_H

#include <SDL_mixer.h>

#include "eastwood/AdlFile.h"

namespace eastwood { namespace SDL { namespace Mixer {

class Player: public CadlPlayer
{
    public:
	Player(int channels, int freq, uint16_t format, Copl *nopl);
	Player();

	static void callback(void *, uint8_t *, int);

    private:
      	inline uint8_t getsampsize() {
    	    return _channels * (_format == AUDIO_U8 ? 1 : 2);
	}

	int _channels;
	int _freq;
	uint16_t _format;
	bool _playing;
	
};

}}}
#endif // EASTWOOD_SDL_MIXER_PLAYER_H
