#ifndef EASTWOOD_SOUND_H
#define EASTWOOD_SOUND_H

namespace eastwood {

enum AudioFormat { 
    FMT_U8,
    FMT_S8,
    FMT_U16LE,
    FMT_S16LE,
    FMT_U16BE,
    FMT_S16BE,
    FMT_INVALID = -1
};

enum Interpolator
{
    I_SINC_BEST_QUALITY		= 0,
    I_SINC_MEDIUM_QUALITY	= 1,
    I_SINC_FASTEST		= 2,
    I_ZERO_ORDER_HOLD		= 3,
    I_LINEAR			= 4
};

class Sound
{
    public:
	Sound() : _size(0), _buffer(NULL), _channels(0), _frequency(0), _format(FMT_INVALID) {};
	Sound(size_t size, uint8_t *buffer, uint8_t channels, uint32_t frequency, AudioFormat format);
	virtual ~Sound();

	Sound getResampled(Interpolator interpolator = I_LINEAR);

    protected:
	friend class VocFile;

    	size_t	_size;
    	uint8_t *_buffer;
	uint8_t _channels;
	uint32_t _frequency;
	AudioFormat _format;
};

}
#endif // EASTWOOD_SOUND_H

