#ifndef EASTWOOD_CPSFILE_H
#define EASTWOOD_CPSFILE_H

#include "Decode.h"

#include <SDL.h>

class CpsFile : public Decode
{
public:
	CpsFile(const unsigned char *bufFiledata, int bufSize, SDL_Palette *palette = NULL);
	~CpsFile();

	SDL_Surface *getSurface();

private:
	const unsigned char *m_filedata;
	SDL_Palette *m_palette;
};

#endif // EASTWOOD_CPSFILE_H
