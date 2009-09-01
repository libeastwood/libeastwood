#include <SDL.h>

#include "StdDef.h"

#include "Log.h"

#include "Exception.h"
#include "ShpFile.h"

using namespace eastwood;

static inline uint32_t getIndex(const uint32_t x) {
    return (x & (TILE_NORMAL-1));
}

static inline TileType getType(const uint32_t x) {
    return (TileType)(x & (TILE_NORMAL-1)<<16);
}

ShpFile::ShpFile(std::istream &stream, SDL_Palette *palette) :
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

static void shp_correct_lf(std::istream &stream, uint8_t *out, int size)
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

static void shp_correct_lf(const uint8_t *in, uint8_t *out, int size)
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

std::vector<uint8_t> ShpFile::getImage(uint16_t fileIndex, uint8_t &sizeX, uint8_t &sizeY)
{
    uint8_t type,
	    slices;
    uint16_t size,
	     fileSize,
	     imageSize;
    std::vector<uint8_t>
	buf,
	decodeDestination,
	imageOut;

    _stream.seekg(_index[fileIndex].startOffset, std::ios::beg);
    type = _stream.get();

    slices = _stream.get();
    sizeY = _stream.get();
    sizeX = _stream.get();

    fileSize = readU16LE(_stream);
    imageSize = readU16LE(_stream);
    /* size and also checksum */
    size = readU16LE(_stream);
    imageOut.resize(sizeX*sizeY);

    LOG_INFO("ShpFile", "File Nr.: %d (Size: %dx%d)",fileIndex,sizeX,sizeY);

    switch(type) {
	case 0:
	    decodeDestination.resize(size);
	    
	    if(decode80(&decodeDestination.front(), size) == -1)
		LOG_WARNING("ShpFile","Checksum-Error in Shp-File");

	    ::shp_correct_lf(&decodeDestination.front(),&imageOut.front(), size);
	    break;

	case 1:
	    decodeDestination.resize(size);
	    buf.resize(16);

	    _stream.read((char*)&buf.front(), buf.size());

	    if(decode80(&decodeDestination.front(), size) == -1)
		LOG_WARNING("ShpFile", "Checksum-Error in Shp-File");
	    
	    ::shp_correct_lf(&decodeDestination.front(), &imageOut.front(), size);

	    apply_pal_offsets(&buf.front(),&imageOut.front(), imageOut.size());
	    break;

	case 2:
	    ::shp_correct_lf(_stream, &imageOut.front(),size);
	    break;

	case 3:
	    buf.resize(16);
	    _stream.read((char*)&buf.front(), buf.size());
	    ::shp_correct_lf(_stream, &imageOut.front(), size);

	    apply_pal_offsets(&buf.front(), &imageOut.front(), imageOut.size());
	    break;

	default:
	    char error[256];
	    sprintf(error, "Type %d in SHP-Files not supported!", type);
	    throw(Exception(LOG_ERROR, "ShpFile", error));
    }

    return imageOut;    
}

SDL_Surface *ShpFile::getSurface(uint16_t fileIndex)
{
    SDL_Surface *pic = NULL;
    uint8_t sizeX,
	    sizeY;
    std::vector<uint8_t> imageOut = getImage(fileIndex, sizeX, sizeY);
    pic = createSurface(&imageOut.front(), sizeX, sizeY, SDL_SWSURFACE);

    SDL_SetColorKey(pic, SDL_SRCCOLORKEY, 0);
    
    return pic;
}

SDL_Surface *ShpFile::getSurfaceArray(uint8_t tilesX, uint8_t tilesY, ...) {
    SDL_Surface *surface;
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
    surface = getSurfaceArray(tilesX, tilesY, &tiles.front());
    return surface;
}

SDL_Surface *ShpFile::getSurfaceArray(const uint8_t tilesX, const uint8_t tilesY, const uint32_t *tiles) {
    SDL_Surface *pic = NULL;
    uint8_t sizeX,
	    sizeY;
    uint16_t index = getIndex(tiles[0]);
    std::vector<uint8_t> imageOut;

    _stream.seekg(_index[index].startOffset+2, std::ios::beg);
    sizeY = _stream.get();
    sizeX = _stream.get();

    for(uint32_t i = 1; i < tilesX*tilesY; i++) {
	_stream.seekg(_index[getIndex(tiles[i])].startOffset+2, std::ios::beg);
	if(_stream.get() != sizeY || _stream.get() != sizeX) {
	    throw(Exception(LOG_ERROR, "ShpFile", "getSurfaceArray(): Not all pictures have the same size!"));
	}
    }

    // create new picture surface
    if((pic = SDL_CreateRGBSurface(SDL_HWSURFACE,sizeX*tilesX,sizeY*tilesY,8,0,0,0,0)) == NULL) {
	throw(Exception(LOG_ERROR, "ShpFile","getSurfaceArray(): Cannot create Surface."));
    }

    SDL_SetColors(pic, _palette->colors, 0, _palette->ncolors);
    SDL_LockSurface(pic);	

    for(uint32_t j = 0; j < tilesY; j++)	{
	for(uint32_t i = 0; i < tilesX; i++) {

	    uint8_t sx, sz;
	    imageOut = getImage(getIndex(tiles[j*tilesX+i]), sx, sz);

	    //Now we can copy line by line
	    switch(getType(tiles[i])) {
		case TILE_NORMAL:
		    for(int y = 0; y < sizeY; y++)
			memcpy(	((char*) (pic->pixels)) + i*sizeX + (y+j*sizeY) * pic->pitch , &imageOut.front() + y * sizeX, sizeX);
		    break;

		case TILE_FLIPH:
		    for(int y = 0; y < sizeY; y++)
			memcpy(	((char*) (pic->pixels)) + i*sizeX + (y+j*sizeY) * pic->pitch , &imageOut.front() + (sizeY-1-y) * sizeX, sizeX);
		    break;

		case TILE_FLIPV:
		    for(int y = 0; y < sizeY; y++)
			for(int x = 0; x < sizeX; x++)
			    *(((char*) (pic->pixels)) + i*sizeX + (y+j*sizeY) * pic->pitch + x) = *(&imageOut.front() + y * sizeX + (sizeX-1-x));
		    break;

		case TILE_ROTATE:
		    for(int y = 0; y < sizeY; y++)
			for(int x = 0; x < sizeX; x++)
			    *(((char*) (pic->pixels)) + i*sizeX + (y+j*sizeY) * pic->pitch + x) = *(&imageOut.front() + (sizeY-1-y) * sizeX + (sizeX-1-x));
		    break;

		default:
		    throw(Exception(LOG_ERROR, "ShpFile", "Invalid type for this parameter. Must be one of TILE_NORMAL, TILE_FLIPH, TILE_FLIPV or TILE_ROTATE!"));
		    break;
	    }
	}
    }

    SDL_UnlockSurface(pic);
    SDL_SetColorKey(pic, SDL_SRCCOLORKEY | SDL_RLEACCEL, 0);
    return pic;
}

