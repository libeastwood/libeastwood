#ifndef EASTWOOD_PCXFILE_H
#define	EASTWOOD_PCXFILE_H

#include "Palette.h"
#include "Decode.h"
#include "ArcStreamBuf.h"
#include "IStream.h"

namespace eastwood {

enum formatVersion {
    PBRUSH_DOS= 0x00,
    V28_PAL = 0x02,
    V28_NOPAL = 0x03,
    PBRUSH_WIN = 0x04,
    V30_STD = 0x05
};

class PcxFile : public Decode
{
    public:
	PcxFile(std::istream &stream);
	~PcxFile();
        
        Surface getSurface();

    private:
        void readHeader();
	formatVersion _format;
};

}
#endif	/* EASTWOOD_PCXFILE_H */

