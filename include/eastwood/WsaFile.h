#ifndef EASTWOOD_WSAFILE_H
#define EASTWOOD_WSAFILE_H
#include <vector>

#include "eastwood/Decode.h"

namespace eastwood {

class WsaFile : public Decode
{
public:
	WsaFile(std::istream &stream, Palette palette,
                Surface firstFrame = Surface());

	virtual ~WsaFile();

	Surface getSurface(uint32_t frameNumber);

	uint32_t getNumFrames() const throw() { return _numFrames; };
	uint32_t getFramesPer1024ms() const throw() { return _framesPer1024ms; };

private:
	void decodeFrames();
	std::vector<uint32_t> _frameOffsTable;
	std::vector<uint8_t> _decodedFrames;

	uint16_t _numFrames;
	uint32_t _deltaBufferSize,
		 _framesPer1024ms;
};

}
#endif // EASTWOOD_WSAFILE_H
