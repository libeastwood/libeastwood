#include <iostream>

#include "eastwood/StdDef.h"

#include "eastwood/Exception.h"
#include "eastwood/FntFile.h"
#include "eastwood/IStream.h"
#include "eastwood/Log.h"

namespace eastwood {

struct FNTHeader
{
    uint16_t fsize;    /* Size of the file              */
    uint16_t unknown1; /* Unknown entry (always 0x0500) */
    uint16_t unknown2; /* Unknown entry (always 0x000e) */
    uint16_t unknown3; /* Unknown entry (always 0x0014) */
    uint16_t wpos;     /* Offset of char. widths array  (abs. from beg. of file) */
    uint16_t cdata;    /* Offset of char. graphics data (abs. from beg. of file) */
    uint16_t hpos;     /* Offset of char. heights array (abs. from beg. of file) */
    uint16_t unknown4; /* Unknown entry (always 0x1012) */
    uint8_t nchars;   /* Number of characters in font minus 1*/ // dunk- the doc says uint16_t 
    uint8_t height;   /* Font height                   */
    uint8_t maxw;     /* Max. character width          */
};

FntFile::FntFile(std::istream &stream) :
    _characters(), _height(0)
{
    IStream &_stream = reinterpret_cast<IStream&>(stream);
    FNTHeader header;
    _stream.readU16LE(reinterpret_cast<uint16_t*>(&header), offsetof(FNTHeader, nchars)/sizeof(uint16_t));
    if (header.unknown1 != 0x0500 || header.unknown2 != 0x000e || header.unknown3 != 0x0014)
	throw(Exception(LOG_ERROR, "FntFile", "Invalid header"));

    // alignment padding
    _stream.ignore(1);
    header.nchars = _stream.get() + 1;
    header.height = _stream.get();
    header.maxw = _stream.get();

    _height = header.height;

    std::vector<uint16_t> dchar(header.nchars);

    _stream.readU16LE(&dchar.front(), header.nchars);

    std::vector<uint8_t> wchar(header.nchars);

    _stream.seekg(header.wpos, std::ios::beg);
    _stream.read(reinterpret_cast<char*>(&wchar.front()), header.nchars);

    //if (wchar[0] != 8) LOG(LV_WARNING, "Font", "%d: bad!!", wchar[0]);

    std::vector<uint16_t> hchar(dchar.size());

    _stream.seekg(header.hpos, std::ios::beg);
    _stream.readU16LE(&hchar.front(), header.nchars);

    _stream.seekg(header.cdata, std::ios::beg);

    _characters.resize(header.nchars);


    for (uint8_t i=0; i!=_characters.size(); i++) {
	uint8_t offset = hchar[i] & 0xFF;
	uint8_t height = hchar[i] >> 8;
	uint8_t width = (wchar[i] + 1)/ 2;

	_characters[i].width = width;
	_characters[i].height = height;
	_characters[i].y_offset = offset;
	_characters[i].bitmap.resize(static_cast<uint8_t>(width*height));

	_stream.seekg(dchar[i], std::ios::beg);
	_stream.read(reinterpret_cast<char*>(&_characters[i].bitmap.front()), static_cast<uint8_t>(_characters[i].bitmap.size()));
    };
}

FntFile::~FntFile()
{
}

void FntFile::extents(std::string text, uint16_t& w, uint16_t& h) const
{
    w = 0;
    h = _height;

    for (size_t c=0; c!=text.length(); c++) {
	const FNTCharacter &ch = _characters[static_cast<uint8_t>(text[c])];
	w += (2 * ch.width) + 1;
    };
}

void FntFile::render(std::string text, Surface &surface, uint16_t offx, uint16_t offy, uint8_t paloff) const
{
    uint8_t* pixels = surface;

    for (size_t c=0; c!=text.length(); c++) {
	const FNTCharacter &ch = _characters[static_cast<uint8_t>(text[c])];
	const std::vector<uint8_t> &bitmap = ch.bitmap;

	for (uint8_t y=0; y!=ch.height; y++) {
	    for (uint8_t x=0; x!=ch.width*2; x+=2) {
		uint8_t byte = bitmap[static_cast<uint8_t>((x/2) + (y*ch.width))];
		uint8_t lobyte = byte >> 4,
			hibyte = byte & 0x0F;

		if (hibyte!=0)
		    pixels[(offx + x) + ((ch.y_offset + y + offy) * surface.width())] = paloff + hibyte;

		if (lobyte!=0) //(2 < ch.width) lobyte!=0)
		    pixels[(offx + x + 1) + ((ch.y_offset + y + offy) * surface.width())] = paloff + lobyte;
	    };
	};
	offx += (2*ch.width) + 1;
    };
}

}
