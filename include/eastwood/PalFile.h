#ifndef EASTWOOD_PALFILE_H
#define EASTWOOD_PALFILE_H

#include <istream>

#include "eastwood/Palette.h"

namespace eastwood {

class PalFile
{
public:
    PalFile(std::istream &stream, bool fullpal = false);
    ~PalFile();

    Palette getPalette();

private:
    Palette _palette;

};

}
#endif // EASTWOOD_PALFILE_H
