#ifndef EASTWOOD_PALETTE_H
#define EASTWOOD_PALETTE_H

#include "SDL.h"

class PalFile
{
    public:
        PalFile(uint8_t * buf, uint16_t bufsize);
        ~PalFile();

        SDL_Palette* getPalette() { return m_palette; }

    private:
        SDL_Palette* m_palette;
    
};

#endif // EASTWOOD_PALETTE_H
