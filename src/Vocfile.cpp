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
#include <math.h>
#include <samplerate.h>
#include <string>
#include <SDL_mixer.h>

#include "mmath.h"
#include "Log.h"

#include "pakfile/Vocfile.h"
using namespace std;

#define	VOC_CODE_TERM		0
#define	VOC_CODE_DATA		1
#define	VOC_CODE_CONT		2
#define	VOC_CODE_SILENCE	3
#define	VOC_CODE_MARKER		4
#define	VOC_CODE_TEXT		5
#define	VOC_CODE_LOOPBEGIN	6
#define	VOC_CODE_LOOPEND	7
#define VOC_CODE_EXTENDED   8
#define VOC_CODE_DATA_16	9


#define NUM_SAMPLES_OF_SILENCE	160


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
Uint32 getSampleRateFromVOCRate(Uint8 vocSR) {
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
static Uint8 *LoadVOC_RW(SDL_RWops* rwop, Uint32 &size, Uint32 &rate) {
	Uint8 description[20];
	Uint16 offset;
	Uint16 version;
	Uint16 id;
	
	if(SDL_RWread(rwop,(char*) description,20,1) != 1) {
		LOG_ERROR("Vocfile", "loadVOCFromStream: Invalid header!");
		return NULL;
	}

	if (memcmp(description, "Creative Voice File", 19) != 0) {
		LOG_ERROR("Vocfile", "loadVOCFromStream: Invalid header!");
		return NULL;
	}
	
	if (description[19] != 0x1A) {
		LOG_ERROR("Vocfile", "loadVOCFromStream: Invalid header!");
		return NULL;
	}
	
	if(SDL_RWread(rwop,&offset,sizeof(offset),1) != 1) {
		LOG_ERROR("Vocfile", "loadVOCFromStream: Invalid header!");
		return NULL;
	}
	offset = SDL_SwapLE16(offset);
	
	if(SDL_RWread(rwop,&version,sizeof(version),1) != 1) {
		LOG_ERROR("Vocfile", "loadVOCFromStream: Invalid header!");
		return NULL;
	}
	version = SDL_SwapLE16(version);
	
	if(SDL_RWread(rwop,&id,sizeof(id),1) != 1) {
		LOG_ERROR("Vocfile", "loadVOCFromStream: Invalid header!");
		return NULL;
	}
	id = SDL_SwapLE16(id);
	
	if(offset != sizeof(description) + sizeof(offset) + sizeof(version) + sizeof(id)) {
		LOG_ERROR("Vocfile", "loadVOCFromStream: Invalid datablock offset in header!");
		return NULL;
	}
	
	// 0x100 is an invalid VOC version used by German version of DOTT (Disk) and
	// French version of Simon the Sorcerer 2 (CD)
	if(!(version == 0x010A || version == 0x0114 || version == 0x0100)) {
		LOG_ERROR("Vocfile", "loadVOCFromStream: Invalid version (0x%X) in header!",version);
		return NULL;
	}
	
	if(id != (~version + 0x1234)) {
		LOG_ERROR("Vocfile", "loadVOCFromStream: Invalid id in header!");
		return NULL;
	}
	
	int len;
	Uint8 *ret_sound = NULL;
	size = 0;
	
	Uint8 code;
	rate = 0;
	while (SDL_RWread(rwop,&code,sizeof(Uint8),1) == 1) {
		if(code == VOC_CODE_TERM) {
			return ret_sound;
		}
	
		Uint8 tmp[3];
		if(SDL_RWread(rwop,tmp,1,3) != 3) {
			LOG_ERROR("Vocfile", "loadVOCFromStream: Invalid block length!");
			return ret_sound;
		}
		len = tmp[0];
		len |= tmp[1] << 8;
		len |= tmp[2] << 16;

		switch (code) {
			case VOC_CODE_DATA: {
				Uint8 time_constant;
				if(SDL_RWread(rwop,&time_constant,sizeof(Uint8),1) != 1) {
					LOG_ERROR("Vocfile", "loadVOCFromStream: Cannot read time constant!");
					return ret_sound;
				}
					
				Uint8 packing;
				if(SDL_RWread(rwop,&packing,sizeof(Uint8),1) != 1) {
					LOG_ERROR("Vocfile", "loadVOCFromStream: Cannot read packing!");
					return ret_sound;
				}
				len -= 2;
				Uint32 tmp_rate = getSampleRateFromVOCRate(time_constant);
				if((rate != 0) && (rate != tmp_rate)) {
					LOG_ERROR("Vocfile", "This voc-file contains data blocks with different sampling rates: old rate: %d, new rate: %d",rate,tmp_rate);
				}
				rate = tmp_rate;
				
				//LOG_ERROR("Vocfile", "VOC Data Block: Rate: %d, Packing: %d, Length: %d\n", rate, packing, len);
				
				if (packing == 0) {
					if (size) {
						ret_sound = (Uint8 *)realloc(ret_sound, size + len);
					} else {
						ret_sound = (Uint8 *)malloc(len);
					}
					
					if(SDL_RWread(rwop,ret_sound + size,1,len) != len) {
						LOG_ERROR("Vocfile", "loadVOCFromStream: Cannot read data!");
						return ret_sound;
					}
					
					size += len;
				} else {
					LOG_ERROR("Vocfile", "VOC file packing %d unsupported!\n", packing);
				}
			} break;
			
			case VOC_CODE_SILENCE: {
				Uint16 SilenceLength;
				if(SDL_RWread(rwop,&SilenceLength,sizeof(Uint16),1) != 1) {
					LOG_ERROR("Vocfile", "loadVOCFromStream: Cannot read silence length!");
					return ret_sound;
				}
				SilenceLength = SDL_SwapLE16(SilenceLength);
				
				Uint8 time_constant;
				if(SDL_RWread(rwop,&time_constant,sizeof(Uint8),1) != 1) {
					LOG_ERROR("Vocfile", "loadVOCFromStream: Cannot read time constant!");
					return ret_sound;
				}
				
				Uint32 SilenceRate = getSampleRateFromVOCRate(time_constant);
				
				
				Uint32 length = 0;
				if(rate != 0) {
					length = (Uint32) ((((double) SilenceRate)/((double) rate)) * SilenceLength) + 1;
				} else {
					LOG_ERROR("Vocfile", "The silence in this voc-file is right at the beginning. Therefore it is not possible to adjust the silence sample rate to the sample rate of the other sound data in this file!");
					length = SilenceLength; 
				}
				
				if (size) {
					ret_sound = (Uint8 *)realloc(ret_sound, size + length);
				} else {
					ret_sound = (Uint8 *)malloc(length);
				}
				
				memset(ret_sound + size,0x80,length);
				
				size += length;
				
			} break;
			
			case VOC_CODE_CONT:
			case VOC_CODE_MARKER:
			case VOC_CODE_TEXT:
			case VOC_CODE_LOOPBEGIN:
			case VOC_CODE_LOOPEND:
			case VOC_CODE_EXTENDED:
			case VOC_CODE_DATA_16:
			default:
				LOG_ERROR("Vocfile", "Unhandled code in VOC file : %d", code);
				return ret_sound;
		}
	}
	return ret_sound;
return 0;
}

inline Uint8 Float2Uint8(float x) {
	int val = lround(x*127.0 + 128.0);
	if(val < 0) {
		val = 0;
	} else if (val > 255) {
		val = 255;
	}
	
	return (Uint8) val;
}

inline Sint8 Float2Sint8(float x) {
	int val = lround(x*127.0);
	if(val < -128) {
		val = -128;
	} else if (val > 127) {
		val = 127;
	}
	
	return (Sint8) val;
}

inline Uint16 Float2Uint16(float x) {
	int val = lround(x*32767.0 + 32768.0);
	if(val < 0) {
		val = 0;
	} else if (val > 65535) {
		val = 65535;
	}
	
	return (Uint16) val;
}

inline Sint16 Float2Sint16(float x) {
	int val = lround(x*32767.0);
	if(val < -32768) {
		val = -32768;
	} else if (val > 32767) {
		val = 32767;
	}
	
	return (Sint16) val;
}

Mix_Chunk* LoadVOC_RW(SDL_RWops* rwop, int freesrc) {
	if(rwop == NULL) {
		return NULL;
	}

	// Read voc file
	Uint32 RawData_Frequency;
	Uint32 RawData_Samples;
	Uint8* RawDataUint8 = LoadVOC_RW(rwop, RawData_Samples, RawData_Frequency);
	if(RawDataUint8 == NULL) {
		if(freesrc) {
			SDL_RWclose(rwop);
		}
		return NULL;
	}

	// Convert to floats
	float* RawDataFloat;
	if((RawDataFloat = (float*) malloc((RawData_Samples+2*NUM_SAMPLES_OF_SILENCE)*sizeof(float))) == NULL) {
		free(RawDataUint8);
		if(freesrc) {
			SDL_RWclose(rwop);
		}
		return NULL;
	}
	
	for(Uint32 i=0; i < NUM_SAMPLES_OF_SILENCE; i++) {
		RawDataFloat[i] = 0.0;
	}
	
	for(Uint32 i=NUM_SAMPLES_OF_SILENCE; i < RawData_Samples+NUM_SAMPLES_OF_SILENCE; i++) {
		RawDataFloat[i] = (((float) RawDataUint8[i-NUM_SAMPLES_OF_SILENCE])/128.0) - 1.0;
	}
	
	for(Uint32 i=RawData_Samples+NUM_SAMPLES_OF_SILENCE; i < RawData_Samples + 2*NUM_SAMPLES_OF_SILENCE; i++) {
		RawDataFloat[i] = 0.0;
	}
	
	free(RawDataUint8);
	
	RawData_Samples += 2*NUM_SAMPLES_OF_SILENCE;
	
	// To prevent strange invalid read in src_linear
	RawData_Samples--;
	
	// Get audio device specifications
	int TargetFrequency, channels;
	Uint16 TargetFormat;
	if(Mix_QuerySpec(&TargetFrequency, &TargetFormat, &channels) == 0) {
		free(RawDataUint8);
		free(RawDataFloat);
		if(freesrc) {
			SDL_RWclose(rwop);
		}
		return NULL;
	}
	
	// Convert to audio device frequency
	float ConversionRatio = ((float) TargetFrequency) / ((float) RawData_Frequency);
	Uint32 TargetDataFloat_Samples = (Uint32) ((float) RawData_Samples * ConversionRatio) + 1;
	float* TargetDataFloat;
	if((TargetDataFloat = (float*) malloc(TargetDataFloat_Samples*sizeof(float))) == NULL) {
		free(RawDataFloat);
		if(freesrc) {
			SDL_RWclose(rwop);
		}
		return NULL;
	}
	
	SRC_DATA src_data;
	src_data.data_in = RawDataFloat;
	src_data.input_frames = RawData_Samples;
	src_data.src_ratio = ConversionRatio;
	src_data.data_out = TargetDataFloat;
	src_data.output_frames = TargetDataFloat_Samples;

	if(src_simple(&src_data, SRC_LINEAR, 1) != 0) {
		free(RawDataFloat);
		free(TargetDataFloat);
		if(freesrc) {
			SDL_RWclose(rwop);
		}
		return NULL;
	}
	
	Uint32 TargetData_Samples = src_data.output_frames_gen;
	free(RawDataFloat);
	
	
	// Equalize if neccessary
	float distance = 0.0;
	for(Uint32 i=0; i < TargetData_Samples; i++) {
		if(fabs(TargetDataFloat[i]) > distance) {
			distance = fabs(TargetDataFloat[i]);
		}
	}
	
	if(distance > 1.0) {
		//Equalize
		for(Uint32 i=0; i < TargetData_Samples; i++) {
			TargetDataFloat[i] = TargetDataFloat[i] / distance;
		}
	}
	

	// Convert floats back to integers but leave out 3/4 of silence
	int ThreeQuaterSilenceLength = (int) ((NUM_SAMPLES_OF_SILENCE * ConversionRatio)*(3.0/4.0));
	TargetData_Samples -= 2*ThreeQuaterSilenceLength;
	
	Mix_Chunk* myChunk;
	if((myChunk = (Mix_Chunk*) calloc(sizeof(Mix_Chunk),1)) == NULL) {
		free(TargetDataFloat);
		if(freesrc) {
			SDL_RWclose(rwop);
		}
		return NULL;
	}
	
	myChunk->volume = 128;
	myChunk->allocated = 1;	
	
	switch(TargetFormat) {
		case AUDIO_U8:
		{
			Uint8* TargetData;
			int SizeOfTargetSample = sizeof(Uint8) * channels;
			if((TargetData = (Uint8*) malloc(TargetData_Samples * SizeOfTargetSample)) == NULL) {
				free(TargetDataFloat);
				free(myChunk);
				if(freesrc) {
					SDL_RWclose(rwop);
				}
				return NULL;
			}
			
			for(Uint32 i=0; i < TargetData_Samples*channels; i+=channels) {
				TargetData[i] = Float2Uint8(TargetDataFloat[(i/channels)+ThreeQuaterSilenceLength]);
				for(int j = 1; j < channels; j++) {
					TargetData[i+j] = TargetData[i];
				}
				
			}
			
			free(TargetDataFloat);
			
			myChunk->abuf = (Uint8*) TargetData;
			myChunk->alen = TargetData_Samples * SizeOfTargetSample;
			
		} break;
		
		case AUDIO_S8:
		{
			Sint8* TargetData;
			int SizeOfTargetSample = sizeof(Sint8) * channels;
			if((TargetData = (Sint8*) malloc(TargetData_Samples * SizeOfTargetSample)) == NULL) {
				free(TargetDataFloat);
				free(myChunk);
				if(freesrc) {
					SDL_RWclose(rwop);
				}
				return NULL;
			}
			
			for(Uint32 i=0; i < TargetData_Samples*channels; i+=channels) {
				TargetData[i] = Float2Sint8(TargetDataFloat[(i/channels)+ThreeQuaterSilenceLength]);
				for(int j = 1; j < channels; j++) {
					TargetData[i+j] = TargetData[i];
				}
				
			}
			
			free(TargetDataFloat);
			
			myChunk->abuf = (Uint8*) TargetData;
			myChunk->alen = TargetData_Samples * SizeOfTargetSample;
			
		} break;
				
		case AUDIO_U16LSB:
		{
			Uint16* TargetData;
			int SizeOfTargetSample = sizeof(Uint16) * channels;
			if((TargetData = (Uint16*) malloc(TargetData_Samples * SizeOfTargetSample)) == NULL) {
				free(TargetDataFloat);
				free(myChunk);
				if(freesrc) {
					SDL_RWclose(rwop);
				}
				return NULL;
			}
			
			for(Uint32 i=0; i < TargetData_Samples*channels; i+=channels) {
				TargetData[i] = SDL_SwapLE16(Float2Uint16(TargetDataFloat[(i/channels)+ThreeQuaterSilenceLength]));
				for(int j = 1; j < channels; j++) {
					TargetData[i+j] = TargetData[i];
				}
				
			}
			
			free(TargetDataFloat);
			
			myChunk->abuf = (Uint8*) TargetData;
			myChunk->alen = TargetData_Samples * SizeOfTargetSample;
			
		} break;
				
        case AUDIO_S16LSB:
		{
			Sint16* TargetData;
			int SizeOfTargetSample = sizeof(Sint16) * channels;
			if((TargetData = (Sint16*) malloc(TargetData_Samples * SizeOfTargetSample)) == NULL) {
				free(TargetDataFloat);
				free(myChunk);
				if(freesrc) {
					SDL_RWclose(rwop);
				}
				return NULL;
			}
			
			for(Uint32 i=0; i < TargetData_Samples*channels; i+=channels) {
				TargetData[i] = SDL_SwapLE16(Float2Sint16(TargetDataFloat[(i/channels)+ThreeQuaterSilenceLength]));
				for(int j = 1; j < channels; j++) {
					TargetData[i+j] = TargetData[i];
				}
				
			}
			
			free(TargetDataFloat);
			
			myChunk->abuf = (Uint8*) TargetData;
			myChunk->alen = TargetData_Samples * SizeOfTargetSample;
			
		} break;
		
        case AUDIO_U16MSB:
		{
			Uint16* TargetData;
			int SizeOfTargetSample = sizeof(Uint16) * channels;
			if((TargetData = (Uint16*) malloc(TargetData_Samples * SizeOfTargetSample)) == NULL) {
				free(TargetDataFloat);
				free(myChunk);
				if(freesrc) {
					SDL_RWclose(rwop);
				}
				return NULL;
			}
			
			for(Uint32 i=0; i < TargetData_Samples*channels; i+=channels) {
				TargetData[i] = SDL_SwapBE16(Float2Uint16(TargetDataFloat[(i/channels)+ThreeQuaterSilenceLength]));
				for(int j = 1; j < channels; j++) {
					TargetData[i+j] = TargetData[i];
				}
				
			}
			
			free(TargetDataFloat);
			
			myChunk->abuf = (Uint8*) TargetData;
			myChunk->alen = TargetData_Samples * SizeOfTargetSample;
			
		} break;
		
		case AUDIO_S16MSB:
		{
			Sint16* TargetData;
			int SizeOfTargetSample = sizeof(Sint16) * channels;
			if((TargetData = (Sint16*) malloc(TargetData_Samples * SizeOfTargetSample)) == NULL) {
				free(TargetDataFloat);
				free(myChunk);
				if(freesrc) {
					SDL_RWclose(rwop);
				}
				return NULL;
			}
			
			for(Uint32 i=0; i < TargetData_Samples*channels; i+=channels) {
				TargetData[i] = SDL_SwapBE16(Float2Sint16(TargetDataFloat[(i/channels)+ThreeQuaterSilenceLength]));
				for(int j = 1; j < channels; j++) {
					TargetData[i+j] = TargetData[i];
				}
				
			}
			
			free(TargetDataFloat);
			
			myChunk->abuf = (Uint8*) TargetData;
			myChunk->alen = TargetData_Samples * SizeOfTargetSample;

		} break;
		
		default:
		{
			free(TargetDataFloat);
			free(myChunk);
			if(freesrc) {
				SDL_RWclose(rwop);
			}
			return NULL;
		} break;
	}
	
	if(freesrc) {
		SDL_RWclose(rwop);
	}

return myChunk;
}
