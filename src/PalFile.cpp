#include <istream>

#include "eastwood/StdDef.h"
#include "eastwood/Log.h"

#include "eastwood/PalFile.h"

namespace eastwood {

PalFile::PalFile(std::istream &stream) : _palette(256)
{
    for (uint16_t i = 0; i < _palette.size(); i++){
	_palette[i].r = stream.get()<<2;
	_palette[i].g = stream.get()<<2;
	_palette[i].b = stream.get()<<2;
	_palette[i].unused = 0;
    }
}

PalFile::~PalFile()
{
}

Palette PalFile::getPalette()
{
    return _palette;
}

}
