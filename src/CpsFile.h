#ifndef EASTWOOD_CPSFILE_H
#define EASTWOOD_CPSFILE_H

#include "Decode.h"

class SDL_Palette;
class SDL_Surface;

class CpsFile : public Decode
{
public:
	CpsFile(std::istream &stream, SDL_Palette *palette = NULL);
	~CpsFile();

	SDL_Surface *getSurface();

private:
	SDL_Palette *m_palette;
	std::istream &_stream;
};

#endif // EASTWOOD_CPSFILE_H
