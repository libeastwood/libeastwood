#include <fstream>
#include "StdDef.h"
#include "SDL/VocSDL.h"

VocSDL::VocSDL(std::istream &stream, int frequency, int channels, AudioFormat format, int quality) :
    VocFile(stream, frequency, channels, format, quality), _mixChunk(new Mix_Chunk)
{
}

VocSDL::VocSDL(std::istream &stream, int quality) : VocFile(stream, 0, 0, FMT_U8, quality), _mixChunk(new Mix_Chunk)
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
    delete _mixChunk;
}

Mix_Chunk *VocSDL::get() {
    _mixChunk->abuf = loadVOCFromStream();
    _mixChunk->alen = getLength();
    _mixChunk->volume = 128;
    _mixChunk->allocated = 1;	

    return _mixChunk;
}
