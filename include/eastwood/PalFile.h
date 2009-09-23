#ifndef EASTWOOD_PALETTE_H
#define EASTWOOD_PALETTE_H

#include <istream>

#include "Surface.h"

namespace eastwood {

class PalFile
{
    public:
	PalFile(std::istream &stream);
	virtual ~PalFile();

	Palette *getPalette();

    private:
	Palette _palette;

};

}
#endif // EASTWOOD_PALETTE_H
