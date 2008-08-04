#include "Log.h"

#include "PalFile.h"

PalFile::PalFile(unsigned char *buf, int bufSize)
{
    LOG_INFO("PalFile", "Loading palette...");
    
    m_palette = new SDL_Palette;
    m_palette->ncolors = bufSize / 3;

    m_palette->colors = new SDL_Color[m_palette->ncolors];
    SDL_Color *c = m_palette->colors;
    unsigned char *dunePal = buf;
    
    for (int i=0; i!=m_palette->ncolors; i++, c++, dunePal+=3)
    {
        c->r = (dunePal[0] << 2) + 3;
        c->g = (dunePal[1] << 2) + 3;
        c->b = (dunePal[2] << 2) + 3;
    };

    delete [] buf;
}

PalFile::~PalFile()
{
    delete m_palette;
}
