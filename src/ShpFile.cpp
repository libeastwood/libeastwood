#include <cstdarg>
#include "eastwood/StdDef.h"

#include "eastwood/Log.h"

#include "eastwood/Exception.h"
#include "eastwood/ShpFile.h"

namespace eastwood {

static inline uint32_t getIndex(const uint32_t x) {
    return (x & (TILE_NORMAL-1));
}

static inline TileType getType(const uint32_t x) {
    return static_cast<TileType>(x & static_cast<uint32_t>(TILE_NORMAL-1)<<16);
}

ShpFile::ShpFile(std::istream &stream, Palette palette) :
    Decode(stream, 0, 0, palette), _index(1), _size(0)
{
    readIndex();
}

ShpFile::~ShpFile()
{
}

void ShpFile::readIndex()
{
    auto fileSize = _stream.sizeg();
    auto offset = 0;

    // First get number of files in shp-file
    _size = _stream.getU16LE();

    if(_size == 0)
	throw(Exception(LOG_ERROR, __FUNCTION__, "There are no files in this SHP-File!"));


    if(fileSize < static_cast<uint32_t>((_size * 4) + 2 + 2)) 
	throw(Exception(LOG_ERROR, __FUNCTION__, "SHP file header is incomplete! Header should be %d bytes big, but file is only %d bytes long.",(_size * 4) + 2 + 2, fileSize));

    _index.at(0).startOffset = _stream.getU16LE();
    _index.at(0).endOffset = _stream.getU16LE();

    if (_index.at(0).endOffset == 0) {
	_index.at(0).startOffset += offset = 2;
	_index.at(0).endOffset = _stream.getU16LE() + offset;
    }
    _index.at(0).endOffset -= 1;

    if(_size > 1) {
	_index.resize(_size);

	// now fill Index with start and end-offsets
	for(auto i = 1; i < _size; i++) {
	    _index.at(i).startOffset = _index.at(i-1).endOffset + 1;
	    _stream.ignore(offset);
	    _index.at(i).endOffset = _stream.getU16LE() - 1 + offset;

	    if(_index.at(i).endOffset > fileSize)
		throw(Exception(LOG_ERROR, __FUNCTION__, "The File with Index %d, goes until byte %d, but this SHP-File is only %d bytes big.",
			i, _index.at(i).endOffset, fileSize));
	}
    }
}

static void apply_pal_offsets(const std::vector<uint8_t> &offsets, uint8_t *data, uint16_t length)
{
    for (auto i = 0; i < length; i++)
	data[i] = offsets[data[i]];
}

Surface ShpFile::getSurface(uint16_t fileIndex)
{
    std::vector<uint8_t>
	palOffsets,
	decodeDestination;

    _stream.seekg(_index.at(fileIndex).startOffset, std::ios::beg);
    auto flags = _stream.getU16LE();

    GNUC_ATTRIBUTE(unused) auto slices = _stream.get();
    auto width = _stream.getU16LE();
    auto height = _stream.get();

    GNUC_ATTRIBUTE(unused) auto fileSize = _stream.getU16LE();
    /* size and also checksum */
    auto imageSize = _stream.getU16LE();
    auto imageOutSize = width*height;
    auto imageOut = new uint8_t[imageOutSize];

    LOG_INFO("File Nr.: %d (Size: %dx%d)",fileIndex,width,height);

    switch(flags) {
	case 0:
	    decodeDestination.resize(imageSize);
	    
	    if(decode80(&decodeDestination.front(), imageSize) == -1)
		LOG_WARNING("Checksum-Error in Shp-File: %d", flags);

	    decode2(decodeDestination, imageOut);
	    break;

	case 1:
	    decodeDestination.resize(imageSize);
	    palOffsets.resize(16);

	    _stream.read(reinterpret_cast<char*>(&palOffsets.front()), palOffsets.size());

	    if(decode80(&decodeDestination.front(), imageSize) == -1)
		LOG_WARNING("Checksum-Error in Shp-File: %d", flags);
	    
	    decode2(decodeDestination, imageOut);

	    apply_pal_offsets(palOffsets,imageOut, imageOutSize);
	    break;

	case 2:
#if 0	//FIXME
	    decode2(_stream, imageOut, imageSize);
#else	    
	    decodeDestination.resize(imageSize);	    
	    _stream.read(reinterpret_cast<char*>(&decodeDestination.front()), imageSize);
	    decode2(decodeDestination, imageOut);
#endif
	    break;

	case 3:
	    palOffsets.resize(16);
	    _stream.read(reinterpret_cast<char*>(&palOffsets.front()), palOffsets.size());

#if 0	//FIXME
	    decode2(_stream, imageOut, imageSize);
#else	    
	    decodeDestination.resize(imageSize);	    
	    _stream.read(reinterpret_cast<char*>(&decodeDestination.front()), imageSize);
	    decode2(decodeDestination, imageOut);
#endif

	    apply_pal_offsets(palOffsets, imageOut, imageOutSize);
	    break;

	default:
	    throw(Exception(LOG_ERROR, __FUNCTION__,"Type %d in SHP-Files not supported!", flags));
    }

    return Surface(imageOut, width, height, 8, _palette);
}

Surface ShpFile::getSurfaceArray(uint8_t tilesX, uint8_t tilesY, ...) {
    std::vector<uint32_t> tiles(tilesX*tilesY);

    va_list arg_ptr;
    va_start(arg_ptr, tilesY);

    for(auto i = 0; i < tilesX*tilesY; i++) {
	tiles[i] = va_arg( arg_ptr, uint32_t );
	if(getIndex(tiles[i]) >= _size)
	    throw(Exception(LOG_ERROR, __FILE__, "getSurfaceArray(): There exist only %d files in this *.shp.",_size));
    }

    va_end(arg_ptr);
    return getSurfaceArray(tilesX, tilesY, &tiles.front());
}

Surface ShpFile::getSurfaceArray(const uint8_t tilesX, const uint8_t tilesY, const uint32_t *tiles) {
    uint16_t index = getIndex(tiles[0]);

    _stream.seekg(_index.at(index).startOffset+3, std::ios::beg);
    auto width = _stream.getU16LE();
    auto height = _stream.get();
    for(auto i = 1; i < tilesX*tilesY; i++) {
	_stream.seekg(_index.at(getIndex(tiles[i])).startOffset+2, std::ios::beg);
	if(_stream.get() != height || _stream.get() != width) {
	    throw(Exception(LOG_ERROR, __FILE__, "getSurfaceArray(): Not all pictures have the same size!"));
	}
    }

    Surface pic(width*tilesX, height*tilesY, 8, _palette);

    for(auto j = 0; j < tilesY; j++)	{
	for(auto i = 0; i < tilesX; i++) {

	    Surface imageOut = getSurface(getIndex(tiles[j*tilesX+i]));

	    //Now we can copy line by line
	    switch(getType(tiles[i])) {
		case TILE_NORMAL:
		    for(auto y = 0; y < height; y++)
			memcpy(reinterpret_cast<char*>(static_cast<uint8_t*>(pic)) + i*width + (y+j*height) * pic.pitch(), static_cast<uint8_t*>(imageOut) + y * width, width);
		    break;

		case TILE_FLIPH:
		    for(auto y = 0; y < height; y++)
			memcpy(reinterpret_cast<char*>(static_cast<uint8_t*>(pic)) + i*width + (y+j*height) * pic.pitch(), static_cast<uint8_t*>(imageOut) + (height-1-y) * width, width);
		    break;

		case TILE_FLIPV:
		    for(auto y = 0; y < height; y++)
			for(auto x = 0; x < width; x++)
			    *(reinterpret_cast<char*>(static_cast<uint8_t*>(pic)) + i*width + (y+j*height) * pic.pitch() + x) = *(static_cast<uint8_t*>(imageOut) + y * width + (width-1-x));
		    break;

		case TILE_ROTATE:
		    for(auto y = 0; y < height; y++)
			for(auto x = 0; x < width; x++)
			    *(reinterpret_cast<char*>(static_cast<uint8_t*>(pic)) + i*width + (y+j*height) * pic.pitch() + x) = *(static_cast<uint8_t*>(imageOut) + (height-1-y) * width + (width-1-x));
		    break;

		default:
		    throw(Exception(LOG_ERROR, __FUNCTION__, "Invalid type for this parameter. Must be one of TILE_NORMAL, TILE_FLIPH, TILE_FLIPV or TILE_ROTATE!"));
		    break;
	    }
	}
    }

    return pic;
}

}
