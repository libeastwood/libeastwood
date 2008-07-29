#include "Log.h"

#include "pakfile/Palette.h"

Palettefile::Palettefile(unsigned char* buf, uint16_t bufsize)
{
    LOG_INFO("Palette", "Loading palette...");
    
    m_palette = new SDL_Palette;
    m_palette->ncolors = bufsize / 3;

    m_palette->colors = new SDL_Color[m_palette->ncolors];
    SDL_Color* c = m_palette->colors;
    uint8_t* dunepal = buf;
    
    for (uint16_t i=0; i!=m_palette->ncolors; i++, c++, dunepal+=3)
    {
        c->r = (dunepal[0] << 2) + 3;
        c->g = (dunepal[1] << 2) + 3;
        c->b = (dunepal[2] << 2) + 3;
    };

    delete [] buf;
}

Palettefile::~Palettefile()
{
    delete m_palette;
}
