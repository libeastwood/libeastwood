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

	inline uint32_t getNumFrames() { return (uint32_t) NumFrames; };
	inline uint32_t getFramesPer1024ms() { return FramesPer1024ms; };
	inline float getFPS() { return fps; }

private:
	void decodeFrames();
	std::string m_text;

	unsigned char *decodedFrames;

	unsigned char *Filedata;
	unsigned char *m_textColor;

	SDL_Palette *m_palette;

	uint32_t *Index;
	int WsaFilesize;

	uint16_t NumFrames;
	uint16_t SizeX;
	uint16_t SizeY;
	uint32_t FramesPer1024ms;
	float fps;
};

#endif // EASTWOOD_WSAFILE_H
