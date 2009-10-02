#include <iostream>

#include "eastwood/StdDef.h"

#include "eastwood/Font.h"
#include "eastwood/Log.h"

namespace eastwood {

Font::Font(FNTCharacter *characters, FNTHeader *header) :
    _header(header), _characters(characters), _nchars(0)
{
}

Font::~Font()
{
    delete [] _characters;
    delete _header;
}

void Font::extents(std::string text, uint16_t& w, uint16_t& h)
{
    FNTCharacter *ch;
    w = 0;
    h = _header->height;

    for (unsigned int c=0; c!=text.length(); c++)
    {
	ch = &_characters[(int)text[c]];
	w += (2 * ch->width) + 1;
    };
}

void Font::render(std::string text, Surface &surface, int offx, int offy, uint8_t paloff)
{
    std::string test = text;
    FNTCharacter *ch;
    uint8_t *bitmap;

    uint8_t* pixels = surface;

    for (unsigned int c=0; c!=text.length(); c++) {
	ch = &_characters[(int)text[c]];
	bitmap = ch->bitmap;

	for (uint8_t y=0; y!=ch->height; y++) {
	    for (uint8_t x=0; x!=ch->width*2; x+=2) {
		uint8_t lobyte = bitmap[(x/2) + (y*ch->width)] >> 4;
		uint8_t hibyte = bitmap[(x/2) + (y*ch->width)] & 0x0F;

		if (hibyte!=0)
		    pixels[(offx + x) + ((ch->y_offset + y + offy) * surface.width())] = paloff + uint8_t(hibyte);

		if (lobyte!=0) //(2 < ch->width) lobyte!=0)
		    pixels[(offx + x + 1) + ((ch->y_offset + y + offy) * surface.width())] = paloff + uint8_t(lobyte);
	    };
	};
	offx += (2*ch->width) + 1;
    };
}

}
