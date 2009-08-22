/* This code is based on code from the ScummVM project
 * Copyright (C) 2001  Ludvig Strigeus
 * Copyright (C) 2001-2006 The ScummVM project
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */
#include <cmath>
#include <cassert>
#include <samplerate.h>
#include <fstream>

#include "StdDef.h"

#include "Exception.h"
#include "Log.h"
#include "VocFile.h"

using namespace eastwood;

enum VocCode {
    VOC_CODE_TERM = 0,
    VOC_CODE_DATA,
    VOC_CODE_CONT,
    VOC_CODE_SILENCE,
    VOC_CODE_MARKER,
    VOC_CODE_TEXT,
    VOC_CODE_LOOPBEGIN,
    VOC_CODE_LOOPEND,
    VOC_CODE_EXTENDED,
    VOC_CODE_DATA_16
};

#define NUM_SAMPLES_OF_SILENCE 250

struct VocFileHeader {
    uint8_t desc[20];
    uint16_t datablock_offset;
    uint16_t version;
    uint16_t id;
} __attribute__((packed));


VocFile::VocFile(std::istream &stream, int _frequency, int channels, AudioFormat format, int quality) :
    _stream(stream), _frequency(_frequency), _channels(channels), _format(format), _quality(quality),
    _vocFrequency(0), _vocSize(0), _vocBeginLoop(0), _vocEndLoop(0), _vocLoops(0), _vocBuffer(NULL),
    _buffer(NULL)
{
}

VocFile::~VocFile()
{
    free(_vocBuffer);
}

/**
 * Take a sample rate parameter as it occurs in a VOC sound header, and
 * return the corresponding sample frequency.
 *
 * This method has special cases for the standard rates of 11025 and 22050 kHz,
 * which due to limitations of the format, cannot be encoded exactly in a VOC
 * file. As a consequence, many game files have sound data sampled with those
 * rates, but the VOC marks them incorrectly as 11111 or 22222 kHz. This code
 * works around that and "unrounds" the sampling rates.
 */
int VocFile::getSampleRateFromVOCRate(int vocSR) {
    if (vocSR == 0xa5 || vocSR == 0xa6) {
	return 11025;
    } else if (vocSR == 0xd2 || vocSR == 0xd3) {
	return 22050;
    } else {
	int sr = 1000000L / (256L - vocSR);
	// inexact sampling rates occur e.g. in the kitchen in Monkey Island,
	// very easy to reach right from the start of the game.
	//warning("inexact sample rate used: %i (0x%x)", sr, vocSR);
	return sr;
    }
}

/**
	This method decodes a voc-file and returns a pointer the decoded data. This memory is
	allocated with malloc() and should be freed with free(). The size of the decoded data is
	returned through the parameter size and the sampling rate of this voc-file is returned
	through the parameter rate.
	The kind of voc-files that this function can decode is very restricted. Only voc-files
	with 8-bit unsigned sound samples, with no loops, no silence blocks, no extended blocks
	and no markers are supported.
	\param	rwop	An SDL_RWop that contains the voc-file
	\param	size	The size of the decoded data in bytes
	\param	rate	The sampling rate of the voc-file
	\return	A pointer to a memory block that contains the data. (Free it with free() when no longer needed)
*/
void VocFile::parseVocFormat() {
	VocFileHeader fileHeader;    
	
	_stream.read((char*)&fileHeader, 8);

	if (!memcmp(&fileHeader, "VTLK", 4)) {
	    _stream.read((char*)&fileHeader, sizeof(VocFileHeader));
	    if(!_stream.good()) goto invalid;
	} else if (!memcmp(&fileHeader, "Creative", 8)) {
	    _stream.read((char*)&fileHeader + 8, sizeof(VocFileHeader) - 8);
	    if(!_stream.good()) goto invalid;
	} else {
	invalid:
	    throw(Exception(LOG_ERROR, "VocFile", "Invalid header"));
	}

	if (memcmp(fileHeader.desc, "Creative Voice File", 19) != 0)
	    goto invalid;

	int32_t offset = htole16(fileHeader.datablock_offset);
	int16_t version = htole16(fileHeader.version);
	int16_t code = htole16(fileHeader.id);
	assert(offset == sizeof(VocFileHeader));
	// 0x100 is an invalid VOC version used by German version of DOTT (Disk) and
	// French version of Simon the Sorcerer 2 (CD)
	assert(version == 0x010A || version == 0x0114 || version == 0x0100);
	assert(code == ~version + 0x1234);

	uint32_t len;
	
	while (!_stream.eof()) {
	    code = _stream.get();
	    if(code == VOC_CODE_TERM) {
		//TODO: throw exception?
		return;
	    }

	    len = _stream.get();
	    len |= _stream.get() << 8;
	    len |= _stream.get() << 16;

	    switch ((VocCode)code) {
	    case VOC_CODE_DATA:
	    case VOC_CODE_DATA_16: {
		uint16_t packing;
		if(code == VOC_CODE_DATA) {
		    uint8_t time_constant = _stream.get();
		    packing = _stream.get();
		    len -= 2;
		    uint32_t tmp_rate = getSampleRateFromVOCRate(time_constant);
		    if((_vocFrequency != 0) && (_vocFrequency != _vocFrequency))
			LOG_ERROR("VocFile", "This voc-file contains data blocks with different sampling rates: old rate: %d, new rate: %d", _vocFrequency,tmp_rate);
		    _vocFrequency = tmp_rate;

		} else {
		    _stream.read((char*)_vocFrequency, sizeof(_vocFrequency));
		    _vocFrequency = htole32(_vocFrequency);
		    int bits = _stream.get();
		    int channels = _stream.get();
		    if (bits != 8 || channels != 1) {
			//warning("Unsupported VOC file format (%d bits per sample, %d channels)", bits, channels);
			break;
		    }
		    _stream.read((char*)packing, sizeof(packing));
		    packing = htole16(packing);
		    _stream.seekg(sizeof(uint32_t), std::ios::cur);
		    len -= 12;
		}
		//debug(9, "VOC Data Block: %d, %d, %d", rate, packing, len);
		if (packing == 0) {
		    if (_vocSize) {
			_vocBuffer = (uint8_t*)realloc(_vocBuffer, _vocSize + len);
		    } else {
			_vocBuffer = (uint8_t*)malloc(len);
		    }
		    _stream.read((char*)_vocBuffer + _vocSize, len);
		    _vocSize += len;
		    _vocBeginLoop = _vocSize;
		    _vocEndLoop = _vocSize;
		} else {
		    /*warning("VOC file packing %d unsupported", packing)*/;
		}
	    } break;
	    case VOC_CODE_SILENCE: {
		uint16_t silenceLength;
		_stream.read((char*)silenceLength, sizeof(silenceLength));
		silenceLength = htole16(silenceLength);
		uint8_t time_constant = _stream.get();
		uint32_t silenceRate = getSampleRateFromVOCRate(time_constant);

		uint32_t length = 0;
		if(_vocFrequency != 0) {
		    length = (uint32_t) ((((double) silenceRate)/((double) _vocFrequency)) * silenceLength) + 1;
		} else {
		    LOG_ERROR("VocFile", "The silence in this voc-file is right at the beginning.\n"
			   "Therefore it is not possible to adjust the silence sample rate to the sample rate of the other sound data in this file!");
		    length = silenceLength; 
		}

		if (_vocSize) {
		    _vocBuffer = (uint8_t *)realloc(_vocBuffer, _vocSize + length);
		} else {
		    _vocBuffer = (uint8_t *)malloc(length);
		}

		memset(_vocBuffer + _vocSize, 0x80, length);

		_vocSize += length;
	    } break;

	    case VOC_CODE_CONT:
	    case VOC_CODE_MARKER:
	    case VOC_CODE_TEXT:
	    case VOC_CODE_LOOPBEGIN:
		assert(len == sizeof(_vocLoops));
		_stream.read((char*)_vocLoops, len);
		_vocLoops = htole16(_vocLoops);
		break;

	    case VOC_CODE_LOOPEND:
		assert(len == 0);
		break;

	    case VOC_CODE_EXTENDED:
		assert(len == 4);
		_stream.seekg(_length, std::ios::cur);

	    default:
		LOG_ERROR("VocFile", "Unhandled code in VOC file : %d", code);
	    }
	}
}

#define __HALF_MAX_SIGNED(type) ((type)1 << (sizeof(type)*8-2))
#define __MAX_SIGNED(type) (__HALF_MAX_SIGNED(type) - 1 + __HALF_MAX_SIGNED(type))
#define __MIN_SIGNED(type) (-1 - __MAX_SIGNED(type))

#define __MIN(type) ((type)-1 < 1?__MIN_SIGNED(type):(type)0)
#define __MAX(type) ((type)~__MIN(type))

template <typename T>
static inline T float2integer(float x) {
    int val = lround(x * (float)__MAX_SIGNED(T) + (float)((T)(__MAX_SIGNED(T)+1)-__MIN(T)));
    if(val < __MIN(T))
	val = __MIN(T);
    else if(val > __MAX(T))
	val = __MAX(T);

    return (T)val;
}

template <typename T>
static uint8_t *setSoundBuffer(size_t &length, AudioFormat format,
	int channels, uint32_t samples, float *dataFloat,
	int silenceLength) {
    T* data;
    int sampleSize = sizeof(T) * channels;
    length = samples * sampleSize;
    data = new T[length];

    for(uint32_t i=0; i < samples*channels; i+=channels) {
	data[i] = float2integer<T>(dataFloat[(i/channels)+silenceLength]);
	if(format == FMT_U16LE || format == FMT_S16LE)
	    data[i] = htole16(data[i]);
	else if(format == FMT_U16BE || format == FMT_S16BE)
	    data[i] = htobe16(data[i]);
	if(sizeof(T) == sizeof(uint8_t))
	    memset((void*)&data[i+1], data[i], channels);
	else
	    wmemset((wchar_t*)&data[i+1], (wchar_t)data[i], channels);
    }

    return (uint8_t*)data;
}

uint8_t *VocFile::loadVOCFromStream() {
    parseVocFormat();

    // Convert to floats
    float *dataFloat = new float[(_vocSize+2*NUM_SAMPLES_OF_SILENCE)*sizeof(float)];

    for(uint32_t i=0; i < NUM_SAMPLES_OF_SILENCE; i++)
	dataFloat[i] = 0.0;

    for(uint32_t i=NUM_SAMPLES_OF_SILENCE; i < _vocSize+NUM_SAMPLES_OF_SILENCE; i++)
	dataFloat[i] = (((float) _vocBuffer[i-NUM_SAMPLES_OF_SILENCE])/128.0) - 1.0;

    for(uint32_t i=_vocSize+NUM_SAMPLES_OF_SILENCE; i < _vocSize+2*NUM_SAMPLES_OF_SILENCE; i++)
	dataFloat[i] = 0.0;

    _vocSize += 2*NUM_SAMPLES_OF_SILENCE;

    // To prevent strange invalid read in src_linear
    _vocSize--;

    // Convert to audio device frequency
    float conversionRatio = ((float) _frequency) / ((float) _vocFrequency);
    uint32_t targetSamplesFloat = (uint32_t) ((float) _vocSize * conversionRatio) + 1;
    float *targetDataFloat = new float[targetSamplesFloat*sizeof(float)];

    SRC_DATA src_data;
    src_data.data_in = dataFloat;
    src_data.input_frames = _vocSize;
    src_data.src_ratio = conversionRatio;
    src_data.data_out = targetDataFloat;
    src_data.output_frames = targetSamplesFloat;

    if(_quality < SRC_SINC_BEST_QUALITY || _quality > SRC_LINEAR)
	_quality = SRC_LINEAR;
    if(src_simple(&src_data, _quality, _channels) != 0) {
	delete [] dataFloat;
	delete [] targetDataFloat;
	return NULL;
    }

    uint32_t targetSamples = src_data.output_frames_gen;
    delete [] dataFloat;


    // Equalize if neccessary
    float distance = 0.0;
    for(uint32_t i=0; i < targetSamples; i++)
	if(fabs(targetDataFloat[i]) > distance)
	    distance = fabs(targetDataFloat[i]);

    //Equalize
    if(distance > 1.0)
	for(uint32_t i=0; i < targetSamples; i++)
	    targetDataFloat[i] = targetDataFloat[i] / distance;

    // Convert floats back to integers but leave out 3/4 of silence
    int silenceLength = (int) ((NUM_SAMPLES_OF_SILENCE * conversionRatio)*(3.0/4.0));
    targetSamples -= 2*silenceLength;


    switch(_format) {
    case FMT_U8:
	_buffer = setSoundBuffer<uint8_t>(_length, _format, _channels, targetSamples, targetDataFloat, silenceLength);
	break;

    case FMT_S8:
	_buffer = setSoundBuffer<int8_t>(_length, _format, _channels, targetSamples, targetDataFloat, silenceLength);
	break;

    case FMT_U16LE:
	_buffer = setSoundBuffer<uint16_t>(_length, _format, _channels, targetSamples, targetDataFloat, silenceLength);
	break;

    case FMT_S16LE:
	_buffer = setSoundBuffer<int16_t>(_length, _format, _channels, targetSamples, targetDataFloat, silenceLength);
	break;

    case FMT_U16BE:
	_buffer = setSoundBuffer<uint16_t>(_length, _format, _channels, targetSamples, targetDataFloat, silenceLength);
	break;

    case FMT_S16BE:
	_buffer = setSoundBuffer<int16_t>(_length, _format, _channels, targetSamples, targetDataFloat, silenceLength);
	break;
    }

    delete [] targetDataFloat;

    return _buffer;
}
