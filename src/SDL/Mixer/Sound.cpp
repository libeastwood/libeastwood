#include "eastwood/StdDef.h"
#include "eastwood/SDL/Mixer/Sound.h"

namespace eastwood { namespace SDL { namespace Mixer {

static std::tr1::shared_ptr<Mix_Chunk> createChunk(bool allocated, uint8_t *buffer, uint32_t length, uint8_t volume = 128)
{
    Mix_Chunk *chunk = (Mix_Chunk*)malloc(sizeof(Mix_Chunk));
    chunk->allocated = allocated;
    chunk->abuf = buffer;
    chunk->alen = length;
    chunk->volume = volume;
    return std::tr1::shared_ptr<Mix_Chunk>(chunk, Mix_FreeChunk);
}

Sound::Sound() :
    eastwood::Sound(), _chunk(createChunk(false, NULL, 0))
{
}

Sound::Sound(const eastwood::Sound &sound) :
    eastwood::Sound(sound), _chunk(createChunk(true, *_buffer.get(), _size))
{
}

Sound::Sound(uint32_t size, uint8_t *buffer, uint32_t frequency, uint8_t channels, AudioFormat format) :
    eastwood::Sound(size, buffer, frequency, channels, format),
    _chunk(createChunk(true, buffer, size))
{
}

Sound::Sound(const Mix_Chunk *sound) :
    eastwood::Sound(sound->alen, NULL, 0, 0, FMT_INVALID),
    _chunk(createChunk(true, NULL, sound->alen))
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

    _buffer.reset(new Bytes((uint8_t*)malloc(_size), BufMalloc));
    memcpy(*this, sound->abuf, _size);
    _chunk->abuf = *this;

}

Sound::~Sound()
{
    _chunk->abuf = NULL;
}

Sound& Sound::operator=(const eastwood::Sound &sound) 
{
    *(eastwood::Sound*)this = sound;
    _chunk->abuf = NULL;
    _chunk = createChunk(true, *this, size());

    return *this;
}

Sound& Sound::operator=(const Mix_Chunk *sound) 
{
    _chunk->abuf = NULL;
    _chunk.reset(const_cast<Mix_Chunk*>(sound), Mix_FreeChunk);
    _buffer.reset(new Bytes((uint8_t*)sound->abuf, BufMalloc));

    return *this;
}

Sound Sound::getResampled(Interpolator interpolator)
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

    return eastwood::Sound::getResampled(channels, frequency, aformat, interpolator);
}

}}}
