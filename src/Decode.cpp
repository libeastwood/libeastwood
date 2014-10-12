#include "eastwood/StdDef.h"

#include "eastwood/Decode.h"
#include "eastwood/Exception.h"

enum fmtCmd {
    CMD_OFFSET_THRESHOLD	= 1<<1,
    CMD_FILL_THRESHOLD		= (1<<2)-1,
    CMD_OFFSET_MAX		= (1<<3)+CMD_OFFSET_THRESHOLD,

    CMD_SMALL_MAX		= 1<<6,
    CMD_LARGE_MAX		= (1<<16)-1,
    CMD_TRANSFER_MAX		= CMD_SMALL_MAX-1,

    CMD_TRANSFER		= 1<<7,
    CMD_COPY_SMALL		= CMD_SMALL_MAX+CMD_TRANSFER,
    CMD_COPY_LARGE		= 1<<8,

    CMD_FILL			= CMD_COPY_LARGE-1

};

namespace eastwood {

Decode::Decode(std::istream &stream, uint16_t width, uint16_t height, Palette palette) :
    _stream(reinterpret_cast<IStream&>(stream)),
    _width(width), _height(height), _palette(palette)
{
}

Decode::~Decode()
{

}

static inline void my_memcpy(uint8_t *dst, const uint8_t *src, uint16_t cnt)
{
    /* Copies memory areas that may overlap command by command from small memory
     * addresses to big memory addresses. Thus, already copied commands can be
     * copied again. */
    if (dst + cnt < src || src + cnt < dst) {
	memcpy(dst, src, cnt);
	return;
    }
    for(uint16_t i = 0; i < cnt; i++)
	dst[i] = src[i];
}

void Decode::decodeRLE(uint8_t* image_out)
{
    unsigned char Pixel;
    unsigned char Number;
    unsigned int y, x;
    long Index;
    unsigned int offset;


    uint32_t Height = _height;
    uint32_t Width = _width;

    /* RLE (Run Length Encoding) */
    offset = 128;
    Index = 0;
    y = 0;
    while( y < Height ) {
        x = 0;
        while( x < Width ){
            Pixel = _stream.get();
            if( Pixel > 192 ){
                Number = Pixel-192;
                Pixel = _stream.get();
                for( unsigned int i=0; i<Number; i++ ){
                        image_out[Index++] = Pixel;
                        x++;
                }
            }else{
                image_out[Index++] = Pixel;
                x++;
            }
        }
        y++;
    }
}

int Decode::decode80(uint8_t *image_out, uint32_t checksum)
{
    uint8_t *writep = image_out;

    uint16_t count,
	     pos;
    /*
       1 0cccpppp p
       2 10cccccc
       3 11cccccc p p
       4 11111110 c c v
       5 11111111 c c p p
       */

    while (true) {
	uint8_t command = _stream.get();
	if ((command & 0x80) == 0x00) {
	    // 0cccpppp p (1) 
	    count = ((command & 0x70) >> 4) + 3;
	    pos = (command  & 0xf) << 8 | _stream.get();
	    //FIXME: This happens at least with WSA animations from Dune 2 demo...
	    if(writep - pos < image_out)
		throw(Exception(LOG_ERROR, "Decode", "Decode80 position is outside memory area (format probably not supported yet)"));
	    //printf("Cmd 1(0x%x), count: %d, pos: %d\n", command, count, pos);
	    my_memcpy(writep, writep - pos, count);
	    writep += count;
	} else {
	    // 10cccccc (2) 
	    count = command & 0x3f;
	    if((command & 0x40) == 0) {
		//printf("Cmd 2(0x%x), count: %d\n", command, count);
		// Finished decoding
		if (!count)
		    break;
		_stream.read(reinterpret_cast<char*>(writep), count);
		writep += count;
	    } else {
		if(count < 0x3e) {
		    // 11cccccc p p (3)
		    count += 3;
		    pos = _stream.getU16LE();
		    //printf("Cmd 3(0x%x), count: %d, pos: %d\n", command, count, pos);
		    my_memcpy(writep, image_out + pos, count);
		    writep += count;
		    pos += count;
		} else if (count == 0x3e) {
		    // 11111110 c c v(4) 
		    count = _stream.getU16LE();
		    uint8_t color = _stream.get();
		    //printf("Cmd 4(0x%x), count: %d, color: %d\n", command, count, color);
		    memset(writep, color, count);
		    writep += count;
		} else {
		    // 11111111 c c p p (5)
		    count = _stream.getU16LE();
		    pos = _stream.getU16LE();
		    //printf("Cmd 5(0x%x), count: %d, pos: %d\n", command, count, pos);
		    my_memcpy(writep, image_out + pos, count);
		    writep += count;
		}
	    }
	}
    }
    if ((writep - image_out) != checksum)
	return -1;
    return 0;
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
		count =  htole16(*(reinterpret_cast<const uint16_t*>(readp)));
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

void Decode::decode2(std::istream &stream, uint8_t *out, int size)
{
    int count;
    while (size > 0) {
	stream.getline(reinterpret_cast<char*>(out), size, 0);
	count = stream.gcount();
	out += count;
	size -= count;
	count = stream.get();
	size--;
	if (count == 0)
	    return;

	if(--count) {
    	    memset(out, 0, count);
    	    out += count;
	}
    }
}

void Decode::decode2(const std::vector<uint8_t> &in, uint8_t *out)
{
    for(std::vector<uint8_t>::const_iterator it = in.begin(); it != in.end();) {
	uint8_t val = *it++;

	if (val != 0) {
	    *out = val;
	    out++;
	} else {
	    uint8_t count;
	    count = *it++;
	    if (count == 0) {
		return;
	    }
	    memset(out, 0, count);

	    out += count;
	}
    }
}

void Decode::encode2(const uint8_t *source, int len, int slices, std::ostream &dest) {
    int count;

    for(int i = 0; i < len; i++) {
	count = 0;
	uint8_t value = *source++;
	dest.put(value);
	if(value == 0) {
	    count++;
	    while(*source == 0)
	    {
		source++;
		count++;
		i++;
	    }
	    dest.put(count);
	}
    }
}

}
