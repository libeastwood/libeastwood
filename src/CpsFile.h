#ifndef CPSFILE_H
#define CPSFILE_H

#include "Decode.h"
#include "SDL.h"

class CpsFile : public Decode
{
public:
	CpsFile(unsigned char * bufFiledata, int bufsize, SDL_Palette* palette = NULL);
	~CpsFile();

	SDL_Surface *getSurface();

private:
	unsigned char *Filedata;
	Uint32 CpsFilesize;
	SDL_Palette *m_palette;
};

#endif // CPSFILE_H
