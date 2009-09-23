#ifndef EASTWOOD_CPSFILE_H
#define EASTWOOD_CPSFILE_H

#include "eastwood/Decode.h"

namespace eastwood {

enum compressionFormat {
    UNCOMPRESSED = 0x000,
    FORMAT_LBM = 0x003,
    FORMAT_80 = 0x004
};

class CpsFile : public Decode
{
    public:
	CpsFile(const std::istream &stream, Palette *palette = NULL);
	virtual ~CpsFile();

	Surface getSurface();

    private:
	compressionFormat _format;
	uint16_t _imageSize;


};

}
#endif // EASTWOOD_CPSFILE_H
