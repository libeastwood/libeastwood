#ifndef EASTWOOD_SDL_MIXERPLAYER_H
#define EASTWOOD_SDL_MIXERPLAYER_H

namespace eastwood { namespace SDL {

class MixerPlayer: public CadlPlayer
{
    public:
	MixerPlayer(Copl *nopl);
	MixerPlayer();

	static void callback(void *, Uint8 *, int);

    private:
	int _channels;
	int _freq;
	uint16_t _format;
	
      	unsigned char getsampsize() {
    	    return _channels * (_format == AUDIO_U8 ? 1 : 2);
	}

};

}}
#endif // EASTWOOD_SDL_MIXERPLAYER_H
