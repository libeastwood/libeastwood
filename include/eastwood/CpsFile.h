#ifndef EASTWOOD_CPSFILE_H
#define EASTWOOD_CPSFILE_H

#include "eastwood/Decode.h"
#include "eastwood/Palette.h"

namespace eastwood {

enum compressionFormat {
    UNCOMPRESSED = 0x000,
    FORMAT_LBM = 0x003,
    FORMAT_80 = 0x004
};

class CpsFile : public Decode
{
    public:
	CpsFile(std::istream &stream, Palette palette = Palette(0));
	~CpsFile();

	Surface getSurface();
    Palette getPalette() { return _palette; }

    private:
	void readHeader();
	compressionFormat _format;

};

}
#endif // EASTWOOD_CPSFILE_H
