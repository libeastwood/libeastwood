#include <vector>
#include <SDL.h>

#include "WsaFile.h"

#include "StdDef.h"

#include "Exception.h"
#include "Font.h"
#include "Log.h"

using namespace eastwood;


enum WSAFlags {
    WF_OFFSCREEN_DECODE = 0x10,
    WF_NO_LAST_FRAME = 0x20,
    WF_NO_FIRST_FRAME = 0x40,
    WF_FLIPPED = 0x80,
    WF_HAS_PALETTE = 0x100,
    WF_XOR = 0x200
};

WsaFile::WsaFile(std::istream &stream, SDL_Palette *palette,
	SDL_Surface *lastframe) :
    Decode(stream, 0, 0, palette), _frameOffsTable(std::vector<uint32_t>()),
    _decodedFrames(std::vector<uint8_t>()), _numFrames(0),
    _deltaBufferSize(0), _framesPer1024ms(0)


{
    LOG_INFO("WsaFile", "Loading wsa with size %d...", bufSize);

    _numFrames = readU16LE(_stream);
    LOG_INFO("WsaFile", "numframes = %d", _numFrames);

    _width = readU16LE(_stream);
    _height = readU16LE(_stream);
    LOG_INFO("WsaFile", "size %d x %d", _width, _height);

    _deltaBufferSize = readU32LE(_stream);

    uint32_t frameDataOffs = readU32LE(_stream);
    if (frameDataOffs == 0) {
	frameDataOffs = readU32LE(_stream);
	_numFrames--;
    }

    _frameOffsTable.resize(_numFrames+2);
    for (uint32_t i = 1; i < _frameOffsTable.size(); ++i) {
	_frameOffsTable[i] = readU32LE(_stream);
	if (_frameOffsTable[i])
	    _frameOffsTable[i] -= frameDataOffs;
    }

    _framesPer1024ms = _deltaBufferSize / 1024.0f;

    LOG_INFO("WsaFile", "_framesPer1024ms = %d", _framesPer1024ms);

    _decodedFrames.resize(_width*_height*_numFrames);

    if (lastframe != NULL)
	memcpy(&_decodedFrames.front(), lastframe->pixels, _width*_height);

    decodeFrames();
}

WsaFile::~WsaFile()
{
}

SDL_Surface *WsaFile::getSurface(uint32_t frameNumber)
{
    if(frameNumber >= _numFrames) {
	return NULL;
    }

    uint8_t *frame = &_decodedFrames.front() + (frameNumber * _width * _height);

    return createSurface(frame, SDL_HWSURFACE);
}

void WsaFile::decodeFrames()
{
    std::vector<uint8_t> dec80(_width*_height*2);

    for(uint16_t i=0;i<_numFrames;i++) 
    {
	std::vector<uint8_t> buf(_frameOffsTable[i+1] - _frameOffsTable[i]);
	_stream.read((char*)&buf.front(), buf.size());

	decode80(&buf.front(), &dec80.front(), 0);

	decode40(&dec80.front(), &_decodedFrames.front() + i * _width * _height);

	if (i < _numFrames - 1)
	    memcpy(&_decodedFrames.front() + (i+1) * _width * _height, &_decodedFrames.front() + i * _width * _height,_width * _height);
    }
}
