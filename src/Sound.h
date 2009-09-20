#ifndef EASTWOOD_SOUND_H
#define EASTWOOD_SOUND_H

#include <ostream>

namespace eastwood {

enum AudioFormat { 
    FMT_U8	= 0x0008,
    FMT_S8	= 0x8008,
    FMT_U16LE	= 0x0010,
    FMT_S16LE	= 0x8010,
    FMT_U16BE	= 0x1010,
    FMT_S16BE	= 0x9010,
    FMT_INVALID	= (uint16_t)-1
};

enum Interpolator
{
    I_SINC_BEST_QUALITY		= 0,
    I_SINC_MEDIUM_QUALITY	= 1,
    I_SINC_FASTEST		= 2,
    I_ZERO_ORDER_HOLD		= 3,
    I_LINEAR			= 4,
    I_INVALID			= -1
};

class Sound
{
    public:
	Sound() : _size(0), _buffer(NULL), _channels(0), _frequency(0), _format(FMT_INVALID) {};
	Sound(size_t size, uint8_t *buffer, uint8_t channels, uint32_t frequency, AudioFormat format);
	virtual ~Sound();

	Sound* getResampled(uint8_t channels, uint32_t frequency, AudioFormat format, Interpolator interpolator = I_LINEAR);
	void saveWAV(std::ostream &output);

    protected:
	template <typename T>
	void getSound(Sound &sound, uint32_t samples, float *dataFloat, int32_t silenceLength);


    	size_t	_size;
    	uint8_t *_buffer;
	uint8_t _channels;
	uint32_t _frequency;
	AudioFormat _format;
};

}
#endif // EASTWOOD_SOUND_H

