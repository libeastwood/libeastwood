#include <fstream>
#include "StdDef.h"
#include "SDL/VocSDL.h"

VocSDL::VocSDL(std::istream &stream, int frequency, int channels, AudioFormat format) :
    VocFile(stream, frequency, channels, format)
{
}

VocSDL::VocSDL(std::istream &stream) : VocFile(stream, 0, 0, FMT_U8)
{
    uint16_t format;
    Mix_QuerySpec(&_frequency, &format, &_channels);
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

VocSDL::~VocSDL()
{
}

Mix_Chunk *VocSDL::getMixChunk(Interpolator interpolator) {
    SoundBuffer soundBuffer = getVOCFromStream(interpolator);
    Mix_Chunk *mixChunk = new Mix_Chunk;
    mixChunk->abuf = soundBuffer.buffer;
    mixChunk->alen = soundBuffer.length;
    mixChunk->volume = 128;
    mixChunk->allocated = 1;	

    return mixChunk;
}
