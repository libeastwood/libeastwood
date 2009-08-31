#include <SDL/SDL_video.h>

#include "StdDef.h"

#include "Decode.h"
#include "Exception.h"

using namespace eastwood;

Decode::Decode(std::istream &stream, uint16_t width, uint16_t height, SDL_Palette *palette) :
    _stream(stream), _width(width), _height(height), _palette(palette)
{
}

Decode::~Decode()
{

}

int Decode::decode80(uint8_t *image_out, uint32_t checksum)
{
    //
    // should decode all the format80 stuff ;-) 
    //

    uint8_t *writep = image_out;

    uint16_t a = 0,
	     b = 0,
	     c = 0,
	     d = 0,
	     e = 0,
	     megacounta = 0,
	     megacountb = 0,
     	     megacountc = 0,
     	     megacountd = 0,
     	     megacounte = 0;
    /*
       1 10cccccc
       2 0cccpppp p
       3 11cccccc p p
       4 11111110 c c v
       5 11111111 c c p p
       */

    while (1) {
	uint8_t byte = _stream.get();
	if ((byte & 0xc0) == 0x80) {
	    //
	    // 10cccccc (1) 
	    //
	    uint16_t count = byte & 0x3f;
	    //printf("Cmd 1, count: %d\n", count);
	    megacounta += count;
	    if (!count)
		break;
	    _stream.read((char*)writep, count);
	    writep += count;
	    a++;
	} else if ((byte & 0x80) == 0x00) {
	    //
	    // 0cccpppp p (2) 
	    //
	    uint16_t count = ((byte & 0x70) >> 4) + 3;
	    uint16_t relpos =	(byte  & 0xf) << 8 | _stream.get();
	    //printf("Cmd 2, count: %d, relpos: %d\n", count, relpos);
	    megacountb += count;
	    my_memcpy(writep, writep - relpos, count);
	    writep += count;
	    b++;
	} else if (byte == 0xff) {
	    // 
	    // 11111111 c c p p (5)
	    //
	    uint16_t count = readU16LE(_stream);
	    uint16_t pos = readU16LE(_stream);
	    //printf("Cmd 5, count: %d, pos: %d\n", count, pos);
	    megacounte += count;
	    my_memcpy(writep, image_out + pos, count);
	    writep += count;
	    e++;
	} else if (byte == 0xfe) {
	    //
	    // 11111110 c c v(4) 
	    //
	    uint16_t count = readU16LE(_stream);
	    uint8_t color = _stream.get();
	    //printf("Cmd 4, count: %d, color: %d\n", count, color);
	    memset(writep, color, count);
	    writep += count;
	    megacountd += count;
	    d++;
	} else if ((byte & 0xc0) == 0xc0) {
	    //
	    // 11cccccc p p (3)
	    //
	    uint16_t count = (byte & 0x3f) + 3;
	    uint16_t pos = readU16LE(_stream);
	    //printf("Cmd 3, count: %d, pos: %d\n", count, pos);
	    megacountc += count;
	    my_memcpy(writep, image_out + pos, count);
	    writep += count;
	    c++;
	} else
	    throw(Exception(LOG_ERROR, "Decode", "Stream contains unknown format80 command"));
    };
    if ((uint16_t)(megacounta + megacountb + megacountc + megacountd + megacounte)
	    != checksum)
	return -1;
    return 0;
}

int Decode::decode80(const unsigned char *image_in, unsigned char *image_out,unsigned int checksum)
{
    //
    // should decode all the format80 stuff ;-) 
    //

    const unsigned char *readp = image_in;
    unsigned char *writep = image_out;

    uint16_t a = 0;
    uint16_t b = 0;
    uint16_t c = 0;
    uint16_t d = 0;
    uint16_t e = 0;
    //	uint16_t relposcount = 0;
    uint16_t megacounta = 0;
    uint16_t megacountb = 0;
    uint16_t megacountc = 0;
    uint16_t megacountd = 0;
    uint16_t megacounte = 0;
    /*
       1 10cccccc
       2 0cccpppp p
       3 11cccccc p p
       4 11111110 c c v
       5 11111111 c c p p
       */

    while (1) {
	if ((*readp & 0xc0) == 0x80) {
	    //
	    // 10cccccc (1) 
	    //
	    unsigned count = readp[0] & 0x3f;
	    //printf("Cmd 1, count: %d\n", count);
	    megacounta += count;
	    if (!count) {
		//#ifdef DEBUG
		//				pinfo80();
		//#endif
		break;
	    }
	    readp++;
	    my_memcpy(writep, (unsigned char*)readp, count);
	    readp += count;
	    writep += count;
	    a++;
	} else if ((*readp & 0x80) == 0x00) {
	    //
	    // 0cccpppp p (2) 
	    //
	    unsigned count = ((readp[0] & 0x70) >> 4) + 3;
	    unsigned short relpos =	(((unsigned short) (readp[0] & 0xf)) << 8) | ((unsigned short) readp[1]);
	    //printf("Cmd 2, count: %d, relpos: %d\n", count, relpos);
	    readp += 2;
	    megacountb += count;
	    my_memcpy(writep, writep - relpos, count);
	    writep += count;
	    b++;
	} else if (*readp == 0xff) {
	    // 
	    // 11111111 c c p p (5)
	    //
	    unsigned short count = htole16(*((unsigned short *) (readp + 1)));
	    unsigned short pos = htole16(*((unsigned short *) (readp + 3)));
	    //printf("Cmd 5, count: %d, pos: %d\n", count, pos);
	    readp += 5;
	    megacounte += count;
	    my_memcpy(writep, image_out + pos, count);
	    writep += count;
	    e++;
	} else if (*readp == 0xfe) {
	    //
	    // 11111110 c c v(4) 
	    //
	    unsigned short count = htole16(*((unsigned short *) (readp + 1)));
	    unsigned char color = readp[3];
	    //printf("Cmd 4, count: %d, color: %d\n", count, color);
	    readp += 4;
	    memset(writep, color, count);
	    writep += count;
	    megacountd += count;
	    d++;
	} else if ((*readp & 0xc0) == 0xc0) {
	    //
	    // 11cccccc p p (3)
	    //

	    unsigned short count = (*readp & 0x3f) + 3;
	    unsigned short pos = htole16(*((unsigned short *) (readp + 1)));
	    //printf("Cmd 3, count: %d, pos: %d\n", count, pos);
	    readp += 3;
	    megacountc += count;
	    my_memcpy(writep, image_out + pos, count);
	    writep += count;
	    c++;
	} else {
	    char error[256];
	    sprintf(error,"file contains unknown format80 command: %x\n",*readp);
	    throw(Exception(LOG_ERROR, "Decode", error));
	}
    };
    if ((unsigned)(megacounta + megacountb + megacountc + megacountd + megacounte)
	    != checksum)
	return -1;
    return 0;
}

void Decode::my_memcpy(unsigned char *dst, unsigned char *src, unsigned cnt)
{
    /* Copies memory areas that may overlap byte by byte from small memory
     * addresses to big memory addresses. Thus, already copied bytes can be
     * copied again. */
    if (dst + cnt < src || src + cnt < dst) {
	memcpy(dst, src, cnt);
	return;
    }
    for(uint16_t i = 0; i < cnt; i++)
	dst[i] = src[i];
}

int Decode::decode40(const uint8_t *image_in, uint8_t *image_out)
{
    /*
       0 fill 00000000 c v
       1 copy 0ccccccc
       2 skip 10000000 c 0ccccccc
       3 copy 10000000 c 10cccccc
       4 fill 10000000 c 11cccccc v
       5 skip 1ccccccc	
       */

    const uint8_t* readp = image_in;
    uint8_t* writep = image_out;
    uint16_t code;
    uint16_t count;
    while (1)
    {
	code = *readp++;
	if (~code & 0x80)
	{
	    //bit 7 = 0
	    if (!code)
	    {
		//command 0 (00000000 c v): fill
		count = *readp++;
		code = *readp++;
		while (count--)
		    *writep++ ^= code;
	    }
	    else
	    {
		//command 1 (0ccccccc): copy
		count = code;
		while (count--)
		    *writep++ ^= *readp++;
	    }

	}
	else
	{
	    //bit 7 = 1
	    if (!(count = code & 0x7f))
	    {
		count =  htole16(*((uint16_t*)readp));
		readp += 2;
		code = count >> 8;
		if (~code & 0x80)
		{
		    //bit 7 = 0
		    //command 2 (10000000 c 0ccccccc): skip
		    if (!count)
			// end of image
			break;
		    writep += count;
		}					
		else
		{
		    //bit 7 = 1
		    count &= 0x3fff;
		    if (~code & 0x40)
		    {
			//bit 6 = 0
			//command 3 (10000000 c 10cccccc): copy
			while (count--)
			    *writep++ ^= *readp++;
		    }
		    else
		    {
			//bit 6 = 1
			//command 4 (10000000 c 11cccccc v): fill
			code = *readp++;
			while (count--)
			    *writep++ ^= code;
		    }
		}
	    }
	    else
	    {
		//command 5 (1ccccccc): skip
		writep += count;
	    }
	}
    }
    return (writep - image_out);
}

SDL_Surface *Decode::createSurface(const uint8_t *buffer, uint32_t flags)
{
    return createSurface(buffer, _width, _height, flags);
}

SDL_Surface *Decode::createSurface(const uint8_t *buffer, uint16_t width, uint16_t height, uint32_t flags)
{
    SDL_Surface *surface;
    if((surface = SDL_CreateRGBSurface(flags, width, height, 8, 0, 0, 0, 0))== NULL)
	throw(Exception(LOG_ERROR, "Decode", "Unable to create SDL_Surface"));

    SDL_SetColors(surface, _palette->colors, 0, _palette->ncolors);

    SDL_LockSurface(surface);

    //Now we can copy line by line
    for(int y = 0; y < height; y++)
	memcpy(((uint8_t*)(surface->pixels)) + y * surface->pitch , buffer + y * width, width);

    SDL_UnlockSurface(surface);

    return surface;
}

