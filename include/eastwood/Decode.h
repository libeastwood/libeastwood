#ifndef EASTWOOD_DECODE_H
#define EASTWOOD_DECODE_H

#include "eastwood/IStream.h"
#include "eastwood/Surface.h"

namespace eastwood {

class Decode
{
public:
	Decode(const std::istream &stream, uint16_t width, uint16_t height, Palette *palette);
	virtual ~Decode();

protected:
	int decode80(uint8_t *image_out, uint32_t checksum);
	int decode40(const unsigned char *image_in, unsigned char *image_out);
	void decode2(std::istream &stream, uint8_t *out, int size);
	void decode2(const uint8_t *in, uint8_t *out, int size);
	//FIXME:
	void encode2(const uint8_t *source, int len, int slices, std::ostream &dest);




	IStream &_stream;
	uint16_t _width,
		 _height;
	Palette *_palette;
};

}
#endif // EASTWOOD_DECODE_H
