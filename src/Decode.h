#ifndef EASTWOOD_DECODE_H
#define EASTWOOD_DECODE_H

#include <istream>

class SDL_Surface;
class SDL_Palette;

class Decode
{
public:
	Decode(std::istream &stream, uint16_t width, uint16_t height, SDL_Palette *palette);
	virtual ~Decode();

protected:
	int decode80(const unsigned char *image_in, unsigned char *image_out,unsigned checksum);
	void my_memcpy(unsigned char *dst, unsigned char *src, unsigned cnt);
	void shp_correct_lf(const unsigned char *in, unsigned char *out, int size);
	void apply_pal_offsets(const unsigned char *offsets, unsigned char *data, unsigned int length);
	int decode40(const unsigned char *image_in, unsigned char *image_out);
	SDL_Surface *createSurface(const uint8_t *buffer, uint32_t flags);
	SDL_Surface *createSurface(const uint8_t *buffer, uint16_t width, uint16_t height, uint32_t flags);
	
	std::istream &_stream;
	uint16_t _width,
		 _height;
	SDL_Palette *_palette;
};

#endif // EASTWOOD_DECODE_H
