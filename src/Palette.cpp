#include "eastwood/StdDef.h"
#include "eastwood/Palette.h"

namespace eastwood {

Palette::Palette(uint16_t colors) : _palette(colors)
{
}

bool Palette::savePAL(std::ostream &output, bool fullpal)
{
    if(fullpal) {
        for (uint16_t i = 0; i < _palette.size(); i++){
            output.put(_palette[i].r);
            output.put(_palette[i].g);
            output.put(_palette[i].b);
        }
    } else {
        for (uint16_t i = 0; i < _palette.size(); i++){
            output.put(_palette[i].r>>2);
            output.put(_palette[i].g>>2);
            output.put(_palette[i].b>>2);
        }
    }
    return true;
}

Palette::~Palette()
{
}

}
