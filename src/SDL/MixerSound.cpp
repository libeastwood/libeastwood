#include "StdDef.h"
#include "SDL/MixerSound.h"

namespace eastwood {

MixerSound::MixerSound(size_t size, uint8_t *buffer, uint32_t frequency, uint8_t channels, AudioFormat format) :
    Sound(size, buffer, frequency, channels, format)
{
}

MixerSound::MixerSound(size_t size, uint8_t *buffer) :
    Sound(size, buffer, 0, 0, FMT_INVALID)
{
    uint16_t format;
    Mix_QuerySpec((int*)&_frequency, &format, (int*)&_channels);
    switch(format) {
    default:
    case AUDIO_U8:	_format = FMT_U8;	break;
    case AUDIO_S8:	_format = FMT_S8;	break;
    case AUDIO_U16LSB:	_format = FMT_U16LE;	break;
    case AUDIO_S16LSB:	_format = FMT_S16LE;	break;
    case AUDIO_U16MSB:	_format = FMT_U16BE;	break;
    case AUDIO_S16MSB:	_format = FMT_S16BE;	break;
    }

}

Mix_Chunk *MixerSound::getMixChunk(Interpolator interpolator) {
    Mix_Chunk *mixChunk = new Mix_Chunk;
    mixChunk->abuf = _buffer;
    mixChunk->alen = _size;
    mixChunk->volume = 128;
    mixChunk->allocated = 1;	

    return mixChunk;
}

}
