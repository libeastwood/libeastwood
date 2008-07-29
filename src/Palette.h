#ifndef DUNE_PAKFILE_PALETTE_H
#define DUNE_PAKFILE_PALETTE_H

#include "SDL.h"

#include <boost/shared_ptr.hpp>

class Palettefile;
typedef boost::shared_ptr<Palettefile> PalettefilePtr;

class Palettefile
{
    public:
        Palettefile(uint8_t * buf, uint16_t bufsize);
        ~Palettefile();

        SDL_Palette* getPalette() { return m_palette; }

    private:
        SDL_Palette* m_palette;
    
};

#endif // DUNE_PAKFILE_PALETTE_H
