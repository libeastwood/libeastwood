#include <vector>

#include "eastwood/StdDef.h"

#include "eastwood/WsaFile.h"

#include "eastwood/Exception.h"
#include "eastwood/Font.h"
#include "eastwood/Log.h"

namespace eastwood {

WsaFile::WsaFile(std::istream &stream, Palette palette,
	Surface firstFrame) :
    Decode(stream, 0, 0, palette), _frameOffsTable(0),
    _decodedFrames(0), _numFrames(0),
    _deltaBufferSize(0), _framesPer1024ms(0)


{
    LOG_INFO("WsaFile", "Loading wsa with size %d...", bufSize);

    _numFrames = _stream.getU16LE();
    LOG_INFO("WsaFile", "numframes = %d", _numFrames);

    _width = _stream.getU16LE();
    _height = _stream.getU16LE();
    LOG_INFO("WsaFile", "size %d x %d", _width, _height);

    _deltaBufferSize = _stream.getU32LE();

    uint32_t frameDataOffs = _stream.getU32LE();
    if (frameDataOffs == 0) {
	frameDataOffs = _stream.getU32LE();
	_numFrames--;
    }

    _frameOffsTable.resize(_numFrames+2);
    for (uint32_t i = 1; i < _frameOffsTable.size(); ++i) {
	_frameOffsTable[i] = _stream.getU32LE();
	if (_frameOffsTable[i])
	    _frameOffsTable[i] -= frameDataOffs;
    }

    _framesPer1024ms = _deltaBufferSize / 1024.0f;

    LOG_INFO("WsaFile", "_framesPer1024ms = %d", _framesPer1024ms);

    _decodedFrames.resize(_width*_height*_numFrames);

    if (firstFrame.bpp())
	memcpy(&_decodedFrames.front(), (uint8_t*)firstFrame, _width*_height);

    decodeFrames();
}

WsaFile::~WsaFile()
{
}

Surface WsaFile::getSurface(uint16_t frameNumber)
{
/*    if(frameNumber >= _numFrames) {
	return NULL;
    }*/

    uint8_t *frame = &_decodedFrames.front() + (frameNumber * _width * _height);

    return Surface(frame, _width, _height, 8, _palette);
}

void WsaFile::decodeFrames()
{
    std::vector<uint8_t> dec80(_width*_height*2);

    for(uint16_t i = 0; i<_numFrames;i++) {
	decode80(&dec80.front(), 0);
	decode40(&dec80.front(), &_decodedFrames.front() + i * _width * _height);

	if (i < _numFrames - 1)
	    memcpy(&_decodedFrames.front() + (i+1) * _width * _height, &_decodedFrames.front() + i * _width * _height,_width * _height);
    }
}

}
