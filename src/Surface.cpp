#include "eastwood/StdDef.h"
#include "eastwood/Surface.h"
#include "eastwood/OStream.h"
#include "eastwood/Exception.h"

#include "eastwood/scaler/scalebit.h"

namespace eastwood {

enum BMPCompression {
    BMP_RGB = 0,
    BMP_RLE8 = 1,
    BMP_RLE4 = 2,
    BMP_BITFIELDS
};

/* The Win32 BMP file header (14 bytes) */
struct BMPHeader {
    char   magic[2];
    uint32_t size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t offBits;
};

/* The Win32 BITMAPINFOHEADER struct (40 bytes) */
struct BMPInfoHeader {
    uint32_t size;
    int32_t width;
    int32_t height;
    uint16_t planes;
    uint16_t bitCount;
    uint32_t compression;
    uint32_t sizeImage;
    int32_t xPelsPerMeter;
    int32_t yPelsPerMeter;
    uint32_t colorsUsed;
    uint32_t colorsImportant;
};

Surface::Surface(uint16_t width, uint16_t height, uint8_t bpp, Palette palette) :
    _bpp(bpp), _width(width), _height(height), _pitch(width*(bpp/8)),
    _pixelsPtr(new Bytes( new uint8_t[(width*(bpp/8)) * _height])),
    _pixels((uint8_t*)*_pixelsPtr.get()), _palette(palette)
{
}

Surface::Surface(uint8_t *buffer, uint16_t width, uint16_t height, uint8_t bpp, Palette palette) :
    _bpp(bpp), _width(width), _height(height), _pitch(width*(bpp/8)),
    _pixelsPtr(new Bytes(buffer)), _pixels((uint8_t*)*_pixelsPtr.get()),
    _palette(palette)
{
}

Surface::Surface(const Surface &surface, bool copy) :
    _bpp(surface._bpp), _width(surface._width), _height(surface._height), _pitch(surface._pitch),
    _pixelsPtr(surface._pixelsPtr), _pixels(surface._pixels), _palette(surface._palette)
{
    if(copy) {
	_pixelsPtr.reset(new Bytes( new uint8_t[(_width*(_bpp/8)) * _height]));
	memcpy(*_pixelsPtr.get(), surface._pixels, (_width*(_bpp/8)) * _height);
	_pixels = *_pixelsPtr.get();
    }
}

Surface::~Surface() {
}

bool Surface::scalePrecondition(Scaler scaler)
{
    return scale_precondition(scaler, _bpp/8, _width, _height);
}

Surface Surface::getScaled(Scaler scaler)
{
    Surface scaled;
    switch(scaler) {
	case Scale2X:
	case Scale2X3:
	case Scale2X4:
	case Scale3X:
	case Scale4X:
	    scaled = Surface(_width * (scaler & ((1<<8)-1)), _height * (scaler>>8), _bpp, _palette);
    	break;
	default:
	    throw(Exception(LOG_ERROR, "Surface", "getScaled(): Unsupported scaler"));
    }
    scale(scaler, scaled._pixels, scaled._pitch, _pixels - _pitch, _pitch, _bpp/8, _width, _height);
    return scaled;
}

bool Surface::saveBMP(std::ostream &output)
{
    uint32_t fp_offset;
    uint8_t *bits;
    BMPHeader header = {{'B', 'M'}, 0, 0, 0,0 };
    BMPInfoHeader info = { sizeof(BMPInfoHeader), _width, _height, 1, _bpp, BMP_RGB, _pitch, 0, 0, _palette.size(), 0};
    OStream &os(const_cast<OStream&>(reinterpret_cast<const OStream&>(output)));


    if (!_palette.size() || _bpp != 8)
	throw Exception(LOG_ERROR, "Surface", "Format not supported");

    /* Write the BMP file header values */
    fp_offset = (uint32_t)os.tellp();
    os.write(header.magic, sizeof(header.magic));
    os.putU32LE(header.size);
    os.putU16LE(header.reserved1);
    os.putU16LE(header.reserved2);
    os.putU32LE(header.offBits);

    /* Write the BMP info values */
    os.putU32LE(info.size);
    os.putU32LE(info.width);
    os.putU32LE(info.height);
    os.putU16LE(info.planes);
    os.putU16LE(info.bitCount);
    os.putU32LE(info.compression);
    os.putU32LE(info.sizeImage);
    os.putU32LE(info.xPelsPerMeter);
    os.putU32LE(info.yPelsPerMeter);
    os.putU32LE(info.colorsUsed);
    os.putU32LE(info.colorsImportant);

    /* Write the palette (in BGR color order) */
    if (_palette.size()) {
	for (uint16_t i = 0; i < _palette.size(); i++) {
	    os.put(_palette[i].b);
	    os.put(_palette[i].g);
	    os.put(_palette[i].r);
	    os.put(0);
	}
    }

    /* Write the bitmap offset */
    header.offBits = (uint32_t)os.tellp()-fp_offset;
    os.seekp(fp_offset+10, std::ios::beg);
    
    os.putU32LE(header.offBits);
    os.seekp(fp_offset+header.offBits);

    /* Write the bitmap image upside down */
    for(bits = _pixels+((_height-1)*_pitch); bits > _pixels; bits-= _pitch)
	os.write((char*)bits, _pitch);

    /* Write the BMP file size */
    header.size = (uint32_t)os.tellp()-fp_offset;
    os.seekp(fp_offset+2, std::ios::beg);
    os.putU32LE(header.size);
    os.seekp(fp_offset+header.size, std::ios::beg);

    return true;
}

}
