#include <cstdarg>
#include "StdDef.h"

#include "Log.h"

#include "Exception.h"
#include "ShpFile.h"

namespace eastwood {

static inline uint32_t getIndex(const uint32_t x) {
    return (x & (TILE_NORMAL-1));
}

static inline TileType getType(const uint32_t x) {
    return (TileType)(x & (TILE_NORMAL-1)<<16);
}

ShpFile::ShpFile(std::istream &stream, Palette *palette) :
    Decode(stream, 0, 0, palette), _index(std::vector<ShpFileEntry>(1)), _numFiles(0)
{
    readIndex();
}

ShpFile::~ShpFile()
{
}

void ShpFile::readIndex()
{
    // First get number of files in shp-file
    _numFiles = readU16LE(_stream);

    if(_numFiles == 0) {
	throw(Exception(LOG_ERROR, "ShpFile", "There are no files in this shp-File!"));
    }

    if(_numFiles == 1) {
	uint16_t start = readU16LE(_stream),
		 end = readU16LE(_stream);
	/* files with only one image might be different */
	if (end != 0) {
	    /* File has special header with only 2 byte offset */

	    _index[0].startOffset = start;
	    _index[0].endOffset = end-1;


	} else {
	    /* File has normal 4 byte offsets */
	    _index[0].startOffset = start;
	    _index[0].endOffset = end - 1 + 2;
	}

    } else {
    	uint32_t fileSize = getStreamSize(_stream);

	/* File contains more than one image */

	if( fileSize < (uint32_t) ((_numFiles * 4) + 2 + 2)) {
	    char error[256];
	    sprintf(error, "Shp-File-Header is not complete! Header should be %d bytes big, but Shp-File is only %d bytes long.",(_numFiles * 4) + 2 + 2, fileSize);
	    throw(Exception(LOG_ERROR, "ShpFile", error));
	}

	_index.resize(_numFiles);

	// now fill Index with start and end-offsets
	for(int i = 0; i < _numFiles; i++) {
	    _index[i].startOffset = readU32LE(_stream) + 2;

	    if(i > 0) {
		char error[256];
		_index[i-1].endOffset = _index[i].startOffset - 1;
		sprintf(error, "The File with Index %d, goes until byte %d, but this SHP-File is only %d bytes big.",
			i, _index[i-1].endOffset, fileSize);
		if(_index[i-1].endOffset > fileSize)
		    throw(Exception(LOG_ERROR, "ShpFile", error));
	    }
	}

	// Add the endOffset for the last file
	_index[_numFiles-1].endOffset = readU16LE(_stream) - 1 + 2;
    }
}

static void decode2(std::istream &stream, uint8_t *out, int size)
{
    int count;
    while (size > 0) {
	stream.getline((char*)out, size, 0);
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

static void decode2(const uint8_t *in, uint8_t *out, int size)
{
    const uint8_t *end = in + size;
    while (in < end) {
	uint8_t val = *in;
	in++;

	if (val != 0) {
	    *out = val;
	    out++;
	} else {
	    uint8_t count;
	    count = *in;
	    in++;
	    if (count == 0) {
		return;
	    }
	    memset(out, 0, count);

	    out += count;
	}
    }
}

static void apply_pal_offsets(const uint8_t *offsets, uint8_t *data, uint16_t length)
{
    for (uint16_t i = 0; i < length; i ++)
	data[i] = offsets[data[i]];
}

std::vector<uint8_t> ShpFile::getImage(uint16_t fileIndex, uint8_t &width, uint8_t &height)
{
    uint8_t slices;
    uint16_t flags,
	     fileSize,
	     imageSize;
    std::vector<uint8_t>
	palOffsets,
	decodeDestination,
	imageOut;

    _stream.seekg(_index[fileIndex].startOffset, std::ios::beg);
    flags = readU16LE(_stream);

    slices = _stream.get();
    width = readU16LE(_stream);
    height = _stream.get();

    fileSize = readU16LE(_stream);
    /* size and also checksum */
    imageSize = readU16LE(_stream);
    imageOut.resize(width*height);

    LOG_INFO("ShpFile", "File Nr.: %d (Size: %dx%d)",fileIndex,width,height);

    switch(flags) {
	case 0:
	    decodeDestination.resize(imageSize);
	    
	    if(decode80(&decodeDestination.front(), imageSize) == -1)
		LOG_WARNING("ShpFile","Checksum-Error in Shp-File");

	    eastwood::decode2(&decodeDestination.front(),&imageOut.front(), imageSize);
	    break;

	case 1:
	    decodeDestination.resize(imageSize);
	    palOffsets.resize(16);

	    readLE(_stream, &palOffsets.front(), palOffsets.size());

	    if(decode80(&decodeDestination.front(), imageSize) == -1)
		LOG_WARNING("ShpFile", "Checksum-Error in Shp-File");
	    
	    eastwood::decode2(&decodeDestination.front(), &imageOut.front(), imageSize);

	    apply_pal_offsets(&palOffsets.front(),&imageOut.front(), imageOut.size());
	    break;

	case 2:
	    eastwood::decode2(_stream, &imageOut.front(),imageSize);
	    break;

	case 3:
	    palOffsets.resize(16);
	    readLE(_stream, &palOffsets.front(), palOffsets.size());
	    eastwood::decode2(_stream, &imageOut.front(), imageSize);

	    apply_pal_offsets(&palOffsets.front(), &imageOut.front(), imageOut.size());
	    break;

	default:
	    char error[256];
	    sprintf(error, "Type %d in SHP-Files not supported!", flags);
	    throw(Exception(LOG_ERROR, "ShpFile", error));
    }

    return imageOut;    
}

Surface ShpFile::getSurface(uint16_t fileIndex)
{
    uint8_t width,
	    height;
    std::vector<uint8_t> imageOut = getImage(fileIndex, width, height);

    return Surface(&imageOut.front(), width, height, 8, _palette);
}

Surface ShpFile::getSurfaceArray(uint8_t tilesX, uint8_t tilesY, ...) {
    std::vector<uint32_t> tiles(tilesX*tilesY);

    va_list arg_ptr;
    va_start(arg_ptr, tilesY);

    for(uint32_t i = 0; i < tilesX*tilesY; i++) {
	tiles[i] = va_arg( arg_ptr, uint32_t );
	if(getIndex(tiles[i]) >= _numFiles) {
	    char error[256];
	    sprintf(error, "getSurfaceArray(): There exist only %d files in this *.shp.",_numFiles);
	    throw(Exception(LOG_ERROR, "ShpFile", error));
	}
    }

    va_end(arg_ptr);
    return getSurfaceArray(tilesX, tilesY, &tiles.front());
}

Surface ShpFile::getSurfaceArray(const uint8_t tilesX, const uint8_t tilesY, const uint32_t *tiles) {
    uint8_t width,
	    height;
    uint16_t index = getIndex(tiles[0]);
    std::vector<uint8_t> imageOut;

    _stream.seekg(_index[index].startOffset+3, std::ios::beg);
    width = readU16LE(_stream);
    height = _stream.get();    

    for(uint32_t i = 1; i < tilesX*tilesY; i++) {
	_stream.seekg(_index[getIndex(tiles[i])].startOffset+2, std::ios::beg);
	if(_stream.get() != height || _stream.get() != width) {
	    throw(Exception(LOG_ERROR, "ShpFile", "getSurfaceArray(): Not all pictures have the same size!"));
	}
    }

    Surface pic(width*tilesX, height*tilesY, 8, _palette);

    for(uint32_t j = 0; j < tilesY; j++)	{
	for(uint32_t i = 0; i < tilesX; i++) {

	    uint8_t sx, sz;
	    imageOut = getImage(getIndex(tiles[j*tilesX+i]), sx, sz);

	    //Now we can copy line by line
	    switch(getType(tiles[i])) {
		case TILE_NORMAL:
		    for(int y = 0; y < height; y++)
			memcpy(	((char*) (pic._pixels)) + i*width + (y+j*height) * pic._pitch , &imageOut.front() + y * width, width);
		    break;

		case TILE_FLIPH:
		    for(int y = 0; y < height; y++)
			memcpy(	((char*) (pic._pixels)) + i*width + (y+j*height) * pic._pitch , &imageOut.front() + (height-1-y) * width, width);
		    break;

		case TILE_FLIPV:
		    for(int y = 0; y < height; y++)
			for(int x = 0; x < width; x++)
			    *(((char*) (pic._pixels)) + i*width + (y+j*height) * pic._pitch + x) = *(&imageOut.front() + y * width + (width-1-x));
		    break;

		case TILE_ROTATE:
		    for(int y = 0; y < height; y++)
			for(int x = 0; x < width; x++)
			    *(((char*) (pic._pixels)) + i*width + (y+j*height) * pic._pitch + x) = *(&imageOut.front() + (height-1-y) * width + (width-1-x));
		    break;

		default:
		    throw(Exception(LOG_ERROR, "ShpFile", "Invalid type for this parameter. Must be one of TILE_NORMAL, TILE_FLIPH, TILE_FLIPV or TILE_ROTATE!"));
		    break;
	    }
	}
    }

    return pic;
}

}
