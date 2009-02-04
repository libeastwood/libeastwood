#ifndef EASTWOOD_CPSFILE_H
#define EASTWOOD_CPSFILE_H

#include "Decode.h"


class SDL_Palette;
class SDL_Surface
class CpsFile : public Decode
{
public:
	CpsFile(unsigned char *bufFiledata, int bufSize, SDL_Palette *palette = NULL);
	~CpsFile();

	SDL_Surface *getSurface();

private:
	const unsigned char *m_filedata;
	Uint32 m_cpsFilesize;
	SDL_Palette *m_palette;
};

#endif // EASTWOOD_CPSFILE_H
