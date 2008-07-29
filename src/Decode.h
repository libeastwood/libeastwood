#ifndef DECODE_H_INCLUDED
#define DECODE_H_INCLUDED

class Decode
{
public:
	Decode();
	~Decode();

protected:

	int decode80(unsigned char *image_in, unsigned char *image_out,unsigned checksum);
	void my_memcpy(unsigned char *dst, unsigned char *src, unsigned cnt);
	void shp_correct_lf(unsigned char *in, unsigned char *out, int size);
	void apply_pal_offsets(unsigned char *offsets, unsigned char *data,unsigned int length);
	int decode40(unsigned char *image_in, unsigned char *image_out);
};

#endif // DECODE_H_INCLUDED
