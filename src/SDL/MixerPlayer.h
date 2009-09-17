#ifndef EASTWOOD_SDL_MIXERPLAYER_H
#define EASTWOOD_SDL_MIXERPLAYER_H

#include <SDL_mixer.h>

#include "AdlFile.h"

namespace eastwood { namespace SDL {

class MixerPlayer: public CadlPlayer
{
    public:
	MixerPlayer(int channels, int freq, uint16_t format, Copl *nopl);
	MixerPlayer();

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

}}
#endif // EASTWOOD_SDL_MIXERPLAYER_H
