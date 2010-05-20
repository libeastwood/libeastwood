#ifndef EASTWOOD_SDL_MIXER_PLAYER_H
#define EASTWOOD_SDL_MIXER_PLAYER_H

#include <SDL_mixer.h>

#include "eastwood/StdDef.h"
#include "eastwood/adl/emuopl.h"
#include "eastwood/AdlFile.h"

namespace eastwood { namespace SDL { namespace Mixer {

class Player: public CadlPlayer
{
    public:
	Player(int channels, int freq, uint16_t format, Copl *opl) :
	    CadlPlayer(opl), _channels(channels), _freq(freq), _format(format), _playing(false) {}

	Player() :
	    CadlPlayer(),
	    _channels(0), _freq(0), _format(0), _playing(false) {
		Mix_QuerySpec(&_freq, &_format, &_channels);	
		_opl = new CEmuopl(_freq, (_format & AUDIO_U16LSB), (_channels > 1));

		init();
	    }

	static void callback(void *userdata, uint8_t *audiobuf, int len) {
	    Player	*self = reinterpret_cast<Player *>(userdata);
	    static long	minicnt = 0;
	    long	i, towrite = len / self->getsampsize();
	    char	*pos = reinterpret_cast<char *>(audiobuf);

	    // Prepare audiobuf with emulator output
	    while(towrite > 0) {
		while(minicnt < 0) {
		    minicnt += self->_freq;
		    self->_playing = self->update();
		}
		i = std::min(towrite, static_cast<long>(minicnt / self->getrefresh() + 4) & ~3);
		self->_opl->update(reinterpret_cast<short*>(pos), i);
		pos += i * self->getsampsize(); towrite -= i;
		minicnt -= static_cast<long>(self->getrefresh() * i);
	    }
	}

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
