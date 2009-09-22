#include "StdDef.h"
#include "SDL/MixerSound.h"

namespace eastwood { namespace SDL {

static Mix_Chunk* createChunk(bool allocated, uint8_t *buffer, uint32_t length, uint8_t volume = 128)
{
    Mix_Chunk *chunk = (Mix_Chunk*)malloc(sizeof(Mix_Chunk));
    chunk->allocated = allocated;
    chunk->abuf = buffer;
    chunk->alen = length;
    chunk->volume = volume;
    return chunk;
}

static Mix_Chunk *tmp = NULL;

MixerSound::MixerSound() :
    Sound(), Mix_Chunk(*(tmp = createChunk(false, NULL, 0))), _sound(tmp)
{
    tmp = NULL;
}

MixerSound::MixerSound(const eastwood::Sound &sound) :
    eastwood::Sound(sound), Mix_Chunk(*(tmp = createChunk(false, NULL, 0))),
    _sound(tmp)
{
    tmp = NULL;
    allocated = true;
    abuf = *_buffer.get();
    alen = _size;
}


MixerSound::MixerSound(uint32_t size, uint8_t *buffer, uint32_t frequency, uint8_t channels, AudioFormat format) :
    Sound(size, buffer, frequency, channels, format),
    Mix_Chunk(*(tmp = createChunk(true, buffer, size))), _sound(tmp)
{
    tmp = NULL;
}

MixerSound::~MixerSound()
{
    _sound->abuf = NULL;
    Mix_FreeChunk(_sound);
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

    return MixerSound(Sound::getResampled(channels, frequency, aformat, interpolator));
}

}}
