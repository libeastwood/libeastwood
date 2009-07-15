#include <iostream>
#include <SDL.h>

#include "StdDef.h"

#include "Font.h"
#include "Log.h"

Font::Font(FNTCharacter *characters, FNTHeader *header)
{
	m_characters = characters;
	m_header = header;
}

Font::~Font()
{
	delete [] m_characters;
	delete m_header;
}

void Font::extents(std::string text, uint16_t& w, uint16_t& h)
{
	FNTCharacter *ch;
	w = 0;
    	h = m_header->height;

    	for (unsigned int c=0; c!=text.length(); c++)
    	{
		ch = &m_characters[(int)text[c]];
		w += (2 * ch->width) + 1;
    };
}

void Font::render(std::string text, SDL_Surface *image, int offx, int offy, uint8_t paloff)
{
	std::string test = text;
	FNTCharacter *ch;
	uint8_t *bitmap;

	SDL_Surface *surface = image;
	uint8_t* pixels = (uint8_t*)surface->pixels;

    	for (unsigned int c=0; c!=text.length(); c++)
	{
		ch = &m_characters[(int)text[c]];
		bitmap = ch->bitmap;

		for (uint8_t y=0; y!=ch->height; y++)
		{
			for (uint8_t x=0; x!=ch->width*2; x+=2)
	    		{
				uint8_t lobyte = bitmap[(x/2) + (y*ch->width)] >> 4;
				uint8_t hibyte = bitmap[(x/2) + (y*ch->width)] & 0x0F;

				if (hibyte!=0)
				{
					pixels[(offx + x) + ((ch->y_offset + y + offy) * surface->w)] = paloff + uint8_t(hibyte);
				};

				if (lobyte!=0) //(2 < ch->width) lobyte!=0)
				{
					pixels[(offx + x + 1) + ((ch->y_offset + y + offy) * surface->w)] = paloff + uint8_t(lobyte);
				};
			};
		};
	offx += (2*ch->width) + 1;
	};
}

