#ifndef EASTWOOD_DECODE_H
#define EASTWOOD_DECODE_H

class Decode
{
public:
	Decode();
	~Decode();

protected:

	int decode80(const unsigned char *image_in, unsigned char *image_out,unsigned checksum);
	void my_memcpy(unsigned char *dst, unsigned char *src, unsigned cnt);
	void shp_correct_lf(const unsigned char *in, unsigned char *out, int size);
	void apply_pal_offsets(const unsigned char *offsets, unsigned char *data, unsigned int length);
	int decode40(const unsigned char *image_in, unsigned char *image_out);
};

#endif // EASTWOOD_DECODE_H
