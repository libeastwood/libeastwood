#ifndef EASTWOOD_WSAFILE_H
#define EASTWOOD_WSAFILE_H

#include "Decode.h"

#include <string>

class SDL_Palette;
class SDL_Surface;
class WsaFile : public Decode
{
public:
	WsaFile(unsigned char *bufFileData, int bufSize, SDL_Palette *palette,
                SDL_Surface *lastframe = NULL, float setFps = 0 );
	WsaFile();

	~WsaFile();

	SDL_Surface *getSurface(uint32_t FrameNumber);

	inline uint32_t getNumFrames() { return (uint32_t) _numFrames; };
	inline uint32_t getFramesPer1024ms() { return _framesPer1024ms; };
	inline float getFPS() { return _fps; }

private:
	void decodeFrames();
	std::string _text;

	unsigned char *_decodedFrames;

	unsigned char *_fileData;
	unsigned char *_textColor;

	SDL_Palette *_palette;

	uint32_t *_index;
	int _wsaFileSize;

	uint16_t _numFrames;
	uint16_t _sizeX;
	uint16_t _sizeY;
	uint32_t _framesPer1024ms;
	float _fps;
};

#endif // EASTWOOD_WSAFILE_H
