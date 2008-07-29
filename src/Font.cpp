#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include "Application.h"
#include "Font.h"
#include "Log.h"
#include "ResMan.h"

Font::Font(FNTCharacter* characters, FNTHeader* header)
{
	m_characters = characters;
	m_header = header;
}

Font::~Font()
{
	delete [] m_characters;
	delete m_header;
}

void Font::extents(std::string text, Uint16& w, Uint16& h)
{
	FNTCharacter* ch;
	w = 0;
    	h = m_header->height;

    	for (unsigned int c=0; c!=text.length(); c++)
    	{
		ch = &m_characters[(int)text[c]];
		w += (2 * ch->width) + 1;
    };
}

void Font::render(std::string text, SDL_Surface *image, int offx, int offy, Uint8 paloff)
{
	std::string test = text;
	FNTCharacter* ch;
	byte* bitmap;

	SDL_Surface * surface = image;
	Uint8* pixels = (Uint8*)surface->pixels;

    	for (unsigned int c=0; c!=text.length(); c++)
	{
		ch = &m_characters[(int)text[c]];
		bitmap = ch->bitmap;

		for (byte y=0; y!=ch->height; y++)
		{
			for (byte x=0; x!=ch->width*2; x+=2)
	    		{
				byte lobyte = bitmap[(x/2) + (y*ch->width)] >> 4;
				byte hibyte = bitmap[(x/2) + (y*ch->width)] & 0x0F;

				if (hibyte!=0)
				{
					pixels[(offx + x) + ((ch->y_offset + y + offy) * surface->w)] = paloff + Uint8(hibyte);
				};

				if (lobyte!=0) //(2 < ch->width) lobyte!=0)
				{
					pixels[(offx + x + 1) + ((ch->y_offset + y + offy) * surface->w)] = paloff + Uint8(lobyte);
				};
			};
		};
	offx += (2*ch->width) + 1;
	};
}

