#ifndef EASTWOOD_CPSFILE_H
#define EASTWOOD_CPSFILE_H

#include "Decode.h"

class SDL_Palette;
class SDL_Surface;

enum compressionFormat {
    UNCOMPRESSED = 0x000,
    FORMAT_LBM = 0x003,
    FORMAT_80 = 0x004
};

class CpsFile : public Decode
{
    public:
	CpsFile(std::istream &stream, SDL_Palette *palette = NULL);
	virtual ~CpsFile();

	SDL_Surface *getSurface();

    private:
	compressionFormat _format;
	uint16_t _imageSize;


};

#endif // EASTWOOD_CPSFILE_H
