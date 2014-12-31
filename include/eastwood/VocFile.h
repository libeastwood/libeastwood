#ifndef EASTWOOD_VOCFILE_H
#define EASTWOOD_VOCFILE_H

#include "eastwood/IStream.h"
#include "eastwood/Sound.h"

namespace eastwood {

class VocFile 
{
    public:
	VocFile(std::istream &stream);
	~VocFile();

	/**
	  Try to load a VOC from the stream. Returns a pointer to Mix_Chunk.
	  It is the callers responsibility to deallocate that data again later on
	  with Mix_FreeChunk()!
	  \param	stream	The stream to load load VOC from. The sample is loaded from this VOC-File.
	  \param	quality	Interpolator type, 0 gives best quality, 4 is fastest. (see libsamplerate API)
	  \return	a pointer to the sample as a Mix_Chunk. nullptr is returned on errors.
	  */
	Sound getSound();

    private:
	inline void readHeader();

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
	uint32_t getSampleRateFromVOCRate(uint32_t vocSR);

	IStream &_stream;
};

}

#endif // EASTWOOD_VOCFILE_H
