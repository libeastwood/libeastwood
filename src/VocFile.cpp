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
uint32_t getSampleRateFromVOCRate(uint8_t vocSR) {
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
static uint8_t *loadVOCFromStream(std::istream &stream, uint32_t &size, uint32_t &rate, uint16_t &loops, uint32_t &begin_loop, uint32_t &end_loop) {
	VocFileHeader fileHeader;    
	
	stream.read((char*)&fileHeader, 8);

	if (!memcmp(&fileHeader, "VTLK", 4)) {
	    stream.read((char*)&fileHeader, sizeof(VocFileHeader));
	    if(!stream.good()) goto invalid;
	} else if (!memcmp(&fileHeader, "Creative", 8)) {
	    stream.read((char*)&fileHeader + 8, sizeof(VocFileHeader) - 8);
	    if(!stream.good()) goto invalid;
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
	uint8_t *ret_sound = NULL;
	size = 0;
	rate = 0;
	begin_loop = 0;
	end_loop = 0;
	
	while (!stream.eof()) {
	    code = stream.get();
	    if(code == VOC_CODE_TERM) {
		return ret_sound;
	    }

	    len = stream.get();
	    len |= stream.get() << 8;
	    len |= stream.get() << 16;

	    switch ((VocCode)code) {
	    case VOC_CODE_DATA:
	    case VOC_CODE_DATA_16: {
		uint16_t packing;
		if(code == VOC_CODE_DATA) {
		    uint8_t time_constant = stream.get();
		    packing = stream.get();
		    len -= 2;
		    uint32_t tmp_rate = getSampleRateFromVOCRate(time_constant);
		    if((rate != 0) && (rate != tmp_rate))
			LOG_ERROR("VocFile", "This voc-file contains data blocks with different sampling rates: old rate: %d, new rate: %d",rate,tmp_rate);
		    rate = tmp_rate;

		} else {
		    stream.read((char*)rate, sizeof(rate));
		    rate = htole32(rate);
		    int bits = stream.get();
		    int channels = stream.get();
		    if (bits != 8 || channels != 1) {
			//warning("Unsupported VOC file format (%d bits per sample, %d channels)", bits, channels);
			break;
		    }
		    stream.read((char*)packing, sizeof(packing));
		    packing = htole16(packing);
		    stream.seekg(sizeof(uint32_t), std::ios::cur);
		    len -= 12;
		}
		//debug(9, "VOC Data Block: %d, %d, %d", rate, packing, len);
		if (packing == 0) {
		    if (size) {
			ret_sound = (uint8_t*)realloc(ret_sound, size + len);
		    } else {
			ret_sound = (uint8_t*)malloc(len);
		    }
		    stream.read((char*)ret_sound + size, len);
		    size += len;
		    begin_loop = size;
		    end_loop = size;
		} else {
		    /*warning("VOC file packing %d unsupported", packing)*/;
		}
	    } break;
	    case VOC_CODE_SILENCE: {
		uint16_t silenceLength;
		stream.read((char*)silenceLength, sizeof(silenceLength));
		silenceLength = htole16(silenceLength);
		uint8_t time_constant = stream.get();
		uint32_t silenceRate = getSampleRateFromVOCRate(time_constant);

		uint32_t length = 0;
		if(rate != 0) {
		    length = (uint32_t) ((((double) silenceRate)/((double) rate)) * silenceLength) + 1;
		} else {
		    LOG_ERROR("VocFile", "The silence in this voc-file is right at the beginning.\n"
			   "Therefore it is not possible to adjust the silence sample rate to the sample rate of the other sound data in this file!");
		    length = silenceLength; 
		}

		if (size) {
		    ret_sound = (uint8_t *)realloc(ret_sound, size + length);
		} else {
		    ret_sound = (uint8_t *)malloc(length);
		}

		memset(ret_sound + size,0x80,length);

		size += length;
	    } break;

	    case VOC_CODE_CONT:
	    case VOC_CODE_MARKER:
	    case VOC_CODE_TEXT:
	    case VOC_CODE_LOOPBEGIN:
		assert(len == sizeof(loops));
		stream.read((char*)loops, len);
		loops = htole16(loops);
		break;

	    case VOC_CODE_LOOPEND:
		assert(len == 0);
		break;

	    case VOC_CODE_EXTENDED:
		assert(len == 4);
		stream.seekg(len, std::ios::cur);

	    default:
		LOG_ERROR("VocFile", "Unhandled code in VOC file : %d", code);
		return ret_sound;
	    }
	}
	return ret_sound;
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
static T *setSoundBuffer(size_t &len, AudioFormat format,
	int channels, uint32_t samples, float *dataFloat,
	int silenceLength) {
    T* data;
    int sampleSize = sizeof(T) * channels;
    len = samples * sampleSize;
    data = new T[len];

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

    return data;
}

uint8_t *loadVOCFromStream(std::istream &stream, size_t &len, int targetFrequency, int channels, AudioFormat targetFormat, int quality) {
    // Read voc file
    uint32_t frequency,
	     samples,
	     begin_loop,
	     end_loop;
    uint16_t loops;
    uint8_t *data = loadVOCFromStream(stream, samples, frequency, loops, begin_loop, end_loop);

    // Convert to floats
    float *dataFloat = new float[(samples+2*NUM_SAMPLES_OF_SILENCE)*sizeof(float)];

    for(uint32_t i=0; i < NUM_SAMPLES_OF_SILENCE; i++)
	dataFloat[i] = 0.0;

    for(uint32_t i=NUM_SAMPLES_OF_SILENCE; i < samples+NUM_SAMPLES_OF_SILENCE; i++)
	dataFloat[i] = (((float) data[i-NUM_SAMPLES_OF_SILENCE])/128.0) - 1.0;

    for(uint32_t i=samples+NUM_SAMPLES_OF_SILENCE; i < samples+2*NUM_SAMPLES_OF_SILENCE; i++)
	dataFloat[i] = 0.0;

    free(data);

    samples += 2*NUM_SAMPLES_OF_SILENCE;

    // To prevent strange invalid read in src_linear
    samples--;

    // Convert to audio device frequency
    float conversionRatio = ((float) targetFrequency) / ((float) frequency);
    uint32_t targetSamplesFloat = (uint32_t) ((float) samples * conversionRatio) + 1;
    float* targetDataFloat = new float[targetSamplesFloat*sizeof(float)];

    SRC_DATA src_data;
    src_data.data_in = dataFloat;
    src_data.input_frames = samples;
    src_data.src_ratio = conversionRatio;
    src_data.data_out = targetDataFloat;
    src_data.output_frames = targetSamplesFloat;

    if(quality < SRC_SINC_BEST_QUALITY || quality > SRC_LINEAR)
	quality = SRC_SINC_BEST_QUALITY;
    if(src_simple(&src_data, quality, channels) != 0) {
	delete [] dataFloat;
	delete [] targetDataFloat;
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


    switch(targetFormat) {
    case FMT_U8:
	data = (uint8_t*) setSoundBuffer<uint8_t>(len, targetFormat, channels, targetSamples, targetDataFloat, silenceLength);
	break;

    case FMT_S8:
	data = (uint8_t*) setSoundBuffer<int8_t>(len, targetFormat, channels, targetSamples, targetDataFloat, silenceLength);
	break;

    case FMT_U16LE:
	data = (uint8_t*) setSoundBuffer<uint16_t>(len, targetFormat, channels, targetSamples, targetDataFloat, silenceLength);
	break;

    case FMT_S16LE:
	data = (uint8_t*) setSoundBuffer<int16_t>(len, targetFormat, channels, targetSamples, targetDataFloat, silenceLength);
	break;

    case FMT_U16BE:
	data = (uint8_t*) setSoundBuffer<uint16_t>(len, targetFormat, channels, targetSamples, targetDataFloat, silenceLength);
	break;

    case FMT_S16BE:
	data = (uint8_t*) setSoundBuffer<int16_t>(len, targetFormat, channels, targetSamples, targetDataFloat, silenceLength);
	break;
    }

    delete [] targetDataFloat;

    return data;
}
