#include "StdDef.h"
#include "SDL/MixerSound.h"

namespace eastwood { namespace SDL {

MixerSound::MixerSound(const eastwood::Sound &sound) : eastwood::Sound(sound)
{
}

MixerSound::MixerSound(size_t size, uint8_t *buffer, uint32_t frequency, uint8_t channels, AudioFormat format) :
    Sound(size, buffer, frequency, channels, format)
{
}

MixerSound::~MixerSound()
{
}

MixerSound MixerSound::getResampled(Interpolator interpolator)
{
    uint8_t channels;
    uint16_t format;
    uint32_t frequency;
    AudioFormat aformat = FMT_INVALID;
    Mix_QuerySpec((int*)&frequency, &format, (int*)&channels);
    switch(format) {
    default:
    case AUDIO_U8:	aformat = FMT_U8;	break;
    case AUDIO_S8:	aformat = FMT_S8;	break;
    case AUDIO_U16LSB:	aformat = FMT_U16LE;	break;
    case AUDIO_S16LSB:	aformat = FMT_S16LE;	break;
    case AUDIO_U16MSB:	aformat = FMT_U16BE;	break;
    case AUDIO_S16MSB:	aformat = FMT_S16BE;	break;
    }

    return Sound::getResampled(channels, frequency, aformat, interpolator);
}

Mix_Chunk *MixerSound::get() {
    Mix_Chunk *mixChunk = new Mix_Chunk;
    mixChunk->abuf = _buffer;
    mixChunk->alen = _size;
    mixChunk->volume = 128;
    mixChunk->allocated = 1;	

    return mixChunk;
}

}}
