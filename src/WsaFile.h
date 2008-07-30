#ifndef WSAFILE_H
#define WSAFILE_H

#include "Decode.h"
#include "Animation.h"
#include "SDL.h"
#include <string>

class WsaFile : public Decode
{
public:
	WsaFile(uint8_t * bufFiledata, int bufsize, 
                SDL_Surface* lastframe = NULL, float setFps = 0 );
	WsaFile();

	~WsaFile();

	SDL_Surface *getSurface(Uint32 FrameNumber, SDL_Palette *palette);

	Animation* getAnimation(unsigned int startindex, unsigned int endindex, SDL_Palette *palette, bool SetColorKey=true);

	inline int getNumFrames() { return (int) NumFrames; };
	inline Uint32 getFramesPer1024ms() { return FramesPer1024ms; };
	inline float getFPS() { return fps; }

private:
	void decodeFrames();
	std::string m_text;

	uint8_t* decodedFrames;

	uint8_t* Filedata;
	uint8_t m_textColor;

	Uint32* Index;
	int WsaFilesize;

	Uint16 NumFrames;
	Uint16 SizeX;
	Uint16 SizeY;
	Uint32 FramesPer1024ms;
	float fps;
};

#endif // WSAFILE_H
