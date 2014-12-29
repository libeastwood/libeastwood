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
#include "eastwood/StdDef.h"

#include "eastwood/Exception.h"
#include "eastwood/Log.h"
#include "eastwood/VocFile.h"

namespace eastwood {

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
    VOC_CODE_DATA_16,
    VOC_CODE_INVALID
};

PACK
struct VocFileHeader {
    uint8_t desc[20];
    uint16_t datablock_offset;
    uint16_t version;
    uint16_t id;
} GNUC_ATTRIBUTE((packed));
PACK

VocFile::VocFile(std::istream &stream) :
    _stream(reinterpret_cast<IStream&>(stream))
{
    readHeader();
}

VocFile::~VocFile()
{
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
uint32_t VocFile::getSampleRateFromVOCRate(uint32_t vocSR) {
    if (vocSR == 0xa5 || vocSR == 0xa6) {
	return 11025;
    } else if (vocSR == 0xd2 || vocSR == 0xd3) {
	return 22050;
    } else {
	uint32_t sr = 1000000L / (256L - vocSR);
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
void VocFile::readHeader() {
    VocFileHeader fileHeader;    

    _stream.read(reinterpret_cast<char*>(&fileHeader), 8);

    if (!memcmp(&fileHeader, "VTLK", 4)) {
	_stream.read(reinterpret_cast<char*>(&fileHeader), offsetof(VocFileHeader,datablock_offset));
	if(!_stream.good()) goto invalid;
    } else if (!memcmp(&fileHeader, "Creative", 8)) {
	_stream.read(reinterpret_cast<char*>(&fileHeader) + 8, offsetof(VocFileHeader,datablock_offset) - 8);
	if(!_stream.good()) goto invalid;
    } else {
invalid:
	throw(Exception(LOG_ERROR, "Invalid header"));
	return;
    }

    if (memcmp(fileHeader.desc, "Creative Voice File", 19) != 0)
	goto invalid;

    _stream.readU16LE(&fileHeader.datablock_offset, (sizeof(VocFileHeader)-offsetof(VocFileHeader,datablock_offset))/sizeof(uint16_t));
    
    if(fileHeader.datablock_offset != sizeof(VocFileHeader))
    	throw(Exception(LOG_ERROR, "Incorrect data block offset"));
    // 0x100 is an invalid VOC version used by German version of DOTT (Disk) and
    // French version of Simon the Sorcerer 2 (CD)
    if(fileHeader.version != 0x010A && fileHeader.version != 0x0114 && fileHeader.version != 0x0100)
	throw(Exception(LOG_ERROR, "Unknown version"));
    if(fileHeader.id != ~fileHeader.version + 0x1234)
	throw(Exception(LOG_ERROR, "Incorrect header id"));
}



Sound VocFile::getSound()
{
    uint8_t *buffer = nullptr,
	    channels = 0;
    uint16_t vocLoops = 0;
    uint32_t len = 0,
	     size = 0,
	     frequency = 0,
	     vocBeginLoop = 0,
	     vocEndLoop = 0;
    AudioFormat format = FMT_INVALID;
    VocCode code = VOC_CODE_INVALID;


    _stream.seekg(sizeof(VocFileHeader), std::ios::beg);


    while (!_stream.eof()) {
	code = static_cast<VocCode>(_stream.get());
	if(code == VOC_CODE_TERM) {
	    //TODO: throw exception?
	    break;
	}

	len = _stream.getU16LE();
	len |= static_cast<uint32_t>(_stream.get() << 16);

	switch (code) {
	    case VOC_CODE_DATA:
	    case VOC_CODE_DATA_16: {

		uint16_t packing;
		if(code == VOC_CODE_DATA) {
		    uint8_t time_constant = _stream.get();
		    packing = _stream.get();
		    len -= 2;
		    uint32_t tmp_rate = getSampleRateFromVOCRate(time_constant);
		    if((frequency != 0) && (frequency != frequency))
			LOG_ERROR("This voc-file contains data blocks with different sampling rates: old rate: %d, new rate: %d", frequency, tmp_rate);
		    frequency = tmp_rate;
		    format = FMT_U8;
		    channels = 1;
		} else {
		    _stream.read(reinterpret_cast<char*>(frequency), sizeof(frequency));
		    frequency = _stream.getU32LE();
		    uint8_t bits = _stream.get();
		    channels = _stream.get();
		    if (bits != 8 || channels != 1) {
			//warning("Unsupported VOC file format (%d bits per sample, %d channels)", bits, channels);
			break;
		    }
		    if(bits == 8)
			format = FMT_U8;
		    else if(bits == 16)
			format = FMT_U16LE;
		    packing = _stream.getU16LE();
		    _stream.seekg(sizeof(uint32_t), std::ios::cur);
		    len -= 12;
		}
		//debug(9, "VOC Data Block: %d, %d, %d", rate, packing, len);
		if (packing == 0) {
    		    if (size) {
    			uint8_t *newBuffer = new uint8_t[size + len];
    			buffer = reinterpret_cast<uint8_t*>(memcpy(newBuffer, buffer, size));
    		    } else
    			buffer = new uint8_t[len];

		    _stream.read(reinterpret_cast<char*>(buffer) + size, len);
		    size += len;
		    vocBeginLoop = size;
		    vocEndLoop = size;
		} else {
		    /*warning("VOC file packing %d unsupported", packing)*/;
		}
	    } break;

	    case VOC_CODE_SILENCE: {
		uint16_t silenceLength = _stream.getU16LE();
		uint8_t time_constant = _stream.get();
		uint32_t silenceRate = getSampleRateFromVOCRate(time_constant);

		uint32_t length = 0;
		if(frequency != 0) {
		    length = static_cast<uint32_t>(static_cast<double>(silenceRate)/(static_cast<double>(frequency) * silenceLength)) + 1;
		} else {
		    LOG_ERROR("The silence in this voc-file is right at the beginning.\n"
			    "Therefore it is not possible to adjust the silence sample rate to the sample rate of the other sound data in this file!");
		    length = silenceLength; 
		}

		if (size) {
		    uint8_t *newBuffer = new uint8_t[size + length];
		    buffer = reinterpret_cast<uint8_t*>(memcpy(newBuffer, buffer, size));
		} else
		    buffer = new uint8_t[length];

		memset(buffer + size, 0x80, length);

		size += length;
	    } break;

	    case VOC_CODE_CONT:
	    case VOC_CODE_MARKER:
	    case VOC_CODE_TEXT:
	    case VOC_CODE_LOOPBEGIN:
		if(len != sizeof(vocLoops))
		    throw(Exception(LOG_ERROR, "Size of vocLoops doesn't match expected size"));
		vocLoops = _stream.getU16LE();
	    break;

	    case VOC_CODE_LOOPEND:
		if(len != 0)
		    throw(Exception(LOG_WARNING, "Code size of loop end is non-zero"));
	    break;

	    case VOC_CODE_EXTENDED:
		if(len != 4)
		    throw(Exception(LOG_ERROR, "Unexpected extended code size"));
		_stream.seekg(len, std::ios::cur);
	    default:
		LOG_ERROR("Unhandled code in VOC file : %d", code);
	}
    }

    return Sound(size, buffer, channels, frequency, format);
}

}
