#include "Log.h"

#include "PalFile.h"

PalFile::PalFile(unsigned char *bufFileData, int bufSize)
{
    LOG_INFO("PalFile", "Loading palette...");
    
    m_palette = new SDL_Palette;
    m_palette->ncolors = bufSize / 3;

    m_palette->colors = new SDL_Color[m_palette->ncolors];

    for (int i = 0; i < m_palette->ncolors; i++){
	    m_palette->colors[i].r = *bufFileData++ <<2;
	    m_palette->colors[i].g = *bufFileData++ <<2;
	    m_palette->colors[i].b = *bufFileData++ <<2;
    }

    delete [] bufFileData;
}

PalFile::~PalFile()
{
    delete m_palette;
}
