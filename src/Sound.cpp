#include <cmath>
#include <samplerate.h>

#include "eastwood/StdDef.h"
#include "eastwood/Sound.h"

#include "eastwood/OStream.h"
#include "eastwood/Exception.h"

#ifdef _MSC_VER
inline long lround(double d) {
  return (long)(d>0 ? d+0.5 : ceil(d-0.5));
}
#endif


#define NUM_SAMPLES_OF_SILENCE 250

namespace eastwood {

struct waveHeader {
    char riffMagic[4];
    uint32_t riffSize;
    char waveMagic[4];
    char fmtMagic[4];
    uint32_t fmtSize;
    uint16_t formatTag;
    uint16_t channels;
    uint32_t frequency;
    uint32_t avgBytesPerSec;
    uint16_t blockAlign;
    uint16_t bits;
    char dataMagic[4];
    uint32_t dataSize;
};

Sound::Sound(uint32_t size, uint8_t *buffer, uint8_t channels, uint32_t frequency, AudioFormat format) :
    _size(size), _buffer(new Bytes(buffer)), _channels(channels), _frequency(frequency), _format(format)
{
}

Sound::Sound(const Sound &sound) :
    _size(sound._size), _buffer(new Bytes(new uint8_t[sound._size])),
    _channels(sound._channels), _frequency(sound._frequency), _format(sound._format)
{
    if(*this)
    	memcpy(*this, sound, size());
}

Sound::~Sound()
{
}

#define __HALF_MAX_SIGNED(type) (static_cast<type>(1) << (sizeof(type)*8-2))
#define __MAX_SIGNED(type) (__HALF_MAX_SIGNED(type) - 1 + __HALF_MAX_SIGNED(type))
#define __MIN_SIGNED(type) (-1 - __MAX_SIGNED(type))

#define __MIN(type) (static_cast<type>(-1) < 1?__MIN_SIGNED(type):static_cast<type>(0))
#define __MAX(type) (static_cast<type>(~__MIN(type)))

template <typename T>
static inline T float2integer(float x) {
    int32_t val = lround(x * static_cast<float>(__MAX_SIGNED(T)) + static_cast<float>(static_cast<T>(__MAX_SIGNED(T)+1)-__MIN(T)));
    if(val < __MIN(T))
	val = __MIN(T);
    else if(val > __MAX(T))
	val = __MAX(T);

    return static_cast<T>(val);
}

template <typename T>
void Sound::getSound(Sound &sound, uint32_t samples, float *dataFloat, int32_t silenceLength) {
    T* data;
    uint32_t sampleSize = sizeof(T) * sound._channels;
    sound._size = samples * sampleSize;
    sound._buffer.reset(new Bytes(reinterpret_cast<uint8_t*>(data = new T[(sound._size *sizeof(T))+1])));

    for(uint32_t i=0; i < samples*sound._channels; i+=sound._channels) {
	data[i] = float2integer<T>(dataFloat[(i/sound._channels)+silenceLength]);
	if(sound._format == FMT_U16LE || sound._format == FMT_S16LE)
	    data[i] = htole16(data[i]);
	else if(sound._format == FMT_U16BE || sound._format == FMT_S16BE)
	    data[i] = htobe16(data[i]);
	if(sizeof(T) == sizeof(uint8_t))
	    memset(reinterpret_cast<void*>(&data[i+1]), data[i], sound._channels);
	else
	    wmemset(reinterpret_cast<wchar_t*>(&data[i+1]), static_cast<wchar_t>(data[i]), sound._channels);
    }
}

Sound Sound::getResampled(uint8_t channels, uint32_t frequency, AudioFormat format, Interpolator interpolator) {
    uint32_t size;
    uint32_t targetSamples,
	     targetSamplesFloat;
    float conversionRatio,
	  distance,
	  *dataFloat,
	  *targetDataFloat;
    int32_t silenceLength;
    Sound sound = Sound(0, NULL, channels, frequency, format);
    SRC_DATA src_data;
    uint8_t *buffer = *_buffer.get();

    size = (_size+2*NUM_SAMPLES_OF_SILENCE)-1;
    // Convert to floats
    dataFloat = new float[size*sizeof(float)];

    memset(dataFloat, 0, (NUM_SAMPLES_OF_SILENCE*sizeof(float)*sizeof(float)));
    memset(&dataFloat[size-NUM_SAMPLES_OF_SILENCE], 0, (NUM_SAMPLES_OF_SILENCE*sizeof(float)*sizeof(float)));
    for(uint32_t i=NUM_SAMPLES_OF_SILENCE; i < size-NUM_SAMPLES_OF_SILENCE; i++)
	dataFloat[i] = (static_cast<float>(buffer[i-NUM_SAMPLES_OF_SILENCE])/128.0) - 1.0;


    // Convert to audio device frequency
    conversionRatio = static_cast<float>(frequency) / static_cast<float>(_frequency);
    targetSamplesFloat = static_cast<uint32_t>(static_cast<float>(size) * conversionRatio) + 1;
    targetDataFloat = new float[targetSamplesFloat*sizeof(float)];

    src_data.data_in = dataFloat;
    src_data.input_frames = size;
    src_data.src_ratio = conversionRatio;
    src_data.data_out = targetDataFloat;
    src_data.output_frames = targetSamplesFloat;

    if(src_simple(&src_data, interpolator, channels) != 0)
	goto end;

    targetSamples = src_data.output_frames_gen;


    // Equalize if neccessary
    distance = 0.0;
    for(uint32_t i=0; i < targetSamples; i++)
	if(fabs(targetDataFloat[i]) > distance)
	    distance = fabs(targetDataFloat[i]);

    //Equalize
    if(distance > 1.0)
	for(uint32_t i=0; i < targetSamples; i++)
	    targetDataFloat[i] = targetDataFloat[i] / distance;

    // Convert floats back to integers but leave out 3/4 of silence
    silenceLength = static_cast<int32_t>((NUM_SAMPLES_OF_SILENCE * conversionRatio)*(3.0/4.0));
    targetSamples -= 2*silenceLength;

    switch(format) {
    case FMT_U8:
	getSound<uint8_t>(sound, targetSamples, targetDataFloat, silenceLength);
	break;

    case FMT_S8:
	getSound<int8_t>(sound, targetSamples, targetDataFloat, silenceLength);
	break;

    case FMT_U16LE:
	getSound<uint16_t>(sound, targetSamples, targetDataFloat, silenceLength);
	break;

    case FMT_S16LE:
	getSound<int16_t>(sound, targetSamples, targetDataFloat, silenceLength);
	break;

    case FMT_U16BE:
	getSound<uint16_t>(sound, targetSamples, targetDataFloat, silenceLength);
	break;

    case FMT_S16BE:
	getSound<int16_t>(sound, targetSamples, targetDataFloat, silenceLength);
	break;
    default:
	throw Exception(LOG_ERROR, __FUNCTION__, "Invalid format");
    }

    end:
    delete [] dataFloat;
    delete [] targetDataFloat;

    return sound;
}

void Sound::saveWAV(std::ostream &output)
{
    OStream &os = reinterpret_cast<OStream&>(output);

    bool bigEndian = _format & (1<<12);
    bool isSigned = _format >> 15;

    waveHeader header = {
	{'R', 'I', 'F', bigEndian ? 'X' : 'F'},
	static_cast<uint32_t>(_size+sizeof(waveHeader)-sizeof(offsetof(waveHeader, riffSize))),
	{'W', 'A', 'V', 'E'},
	{'f', 'm', 't', ' '},
	16,
	0x0001U,
	_channels,
	_frequency,
	static_cast<uint32_t>(static_cast<double>(_channels * _frequency+1) / static_cast<double>(1) + 0.5),
	static_cast<uint16_t>(_channels * ((_format & ((1<<8)-1))>>3)),
	static_cast<uint16_t>(_format & ((1<<8)-1)),
	{'d', 'a', 't', 'a'},
	_size
    };

    uint8_t *buffer = *_buffer.get();
    
    os.write(reinterpret_cast<char*>(&header.riffMagic), sizeof(header.riffMagic));
    bigEndian ? os.putU32BE(header.riffSize) : os.putU32LE(header.riffSize);
    os.write(reinterpret_cast<char*>(&header.waveMagic), sizeof(header.waveMagic));
    os.write(reinterpret_cast<char*>(&header.fmtMagic), sizeof(header.fmtMagic));
    if(bigEndian) {
	os.putU32BE(header.fmtSize);
	os.putU16BE(header.formatTag);
	os.putU16BE(header.channels);
	os.putU32BE(header.frequency);
	os.putU32BE(header.avgBytesPerSec);
	os.putU16BE(header.blockAlign);
	os.putU16BE(header.bits);
    } else {
	os.putU32LE(header.fmtSize);
	os.putU16LE(header.formatTag);
	os.putU16LE(header.channels);
	os.putU32LE(header.frequency);
	os.putU32LE(header.avgBytesPerSec);
	os.putU16LE(header.blockAlign);
	os.putU16LE(header.bits);
    }
    os.write(reinterpret_cast<char*>(&header.dataMagic), sizeof(header.dataMagic));
    bigEndian ? os.putU32BE(header.dataSize) : os.putU32LE(header.dataSize);    
    // PCM requires data to be unsigned for 8 bit and signed for 16, so if not,
    // we need to convert it
    if((header.bits == 8 && isSigned) || (header.bits == 16 && !isSigned)) {
	for(uint32_t i = 0; i < _size; i++)
	    os.put(buffer[i] ^ (1<<7));
    } else
    	os.write(reinterpret_cast<char*>(buffer), _size);
}

}
