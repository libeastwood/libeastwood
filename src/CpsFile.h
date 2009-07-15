#ifndef EASTWOOD_CPSFILE_H
#define EASTWOOD_CPSFILE_H

#include "Decode.h"

class SDL_Palette;
class SDL_Surface;

class CpsFile : public Decode
{
public:
	CpsFile(const uint8_t *bufFiledata, int bufSize, SDL_Palette *palette = NULL);
	~CpsFile();

	SDL_Surface *getSurface();

private:
	const uint8_t *m_filedata;
	SDL_Palette *m_palette;
};

#endif // EASTWOOD_CPSFILE_H
