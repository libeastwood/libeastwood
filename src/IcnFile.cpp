#include "StdDef.h"

#include "Exception.h"
#include "IcnFile.h"
#include "Log.h"

namespace eastwood {

IcnFile::IcnFile(std::istream &stream, MapFile &map, Palette *palette) :
    Decode(stream, 16, 16, palette),
    _map(map), _size(0), _SSET(NULL), _RPAL(NULL), _RTBL(NULL)
{
    readHeader();
}

IcnFile::~IcnFile()
{
    if(_SSET)
	delete _SSET;
    if(_RPAL)
	delete _RPAL;
    if(_RTBL)
	delete _RTBL;
}

void IcnFile::readHeader()
{
    char signature[8];
    uint16_t sectionSize;

    _stream.seekg(0, std::ios::beg);
    
    _stream.read(signature, 4);
    if(strncmp(signature, "FORM", 4))
	throw(Exception(LOG_ERROR, "IcnFile", "Invalid ICN-File: Missing signature"));

    if(readU32BE(_stream) != getStreamSize(_stream) - (uint32_t)_stream.tellg())
	throw(Exception(LOG_ERROR, "IcnFile", "Invalid ICN-File: File size doesn't match size specified in header"));

    _stream.read(signature, 8);
    if(strncmp(signature, "ICONSINF", 8))
	throw(Exception(LOG_ERROR, "IcnFile", "Invalid ICN-File: Missing signature"));

    //TODO: Figure out what these 8 bytes are for
    _stream.ignore(8);

    _stream.read(signature, 4);
    if(strncmp(signature, "SSET", 4))
	throw(Exception(LOG_ERROR, "IcnFile", "Invalid ICN-File: No SSET-Section found"));
    // Don't *really* have any idea what this is good for (sanity check?):
    // Section header looks like this:
    // [BBBB00LL]
    //
    // BBBB: Size is first stored as a 32 bit integer, big endian
    // 00: Two byte space (00)
    // LL: SSET section size - header size is stored again as 16 bit integer, little endian

    // So yeah, let's use this a sort of sanity check...
    if(readU32BE(_stream) - 8 != (sectionSize = readU16LE(_stream) + readU16LE(_stream)))
	throw(Exception(LOG_ERROR, "IcnFile", "Invalid ICN-File: SSET-Section size mismatch"));
    _stream.ignore(4);
    _SSET = new std::vector<uint8_t>(sectionSize);
    _stream.read((char*)&_SSET->front(), _SSET->size());

    _stream.read(signature, 4);
    if(strncmp(signature, "RPAL", 4))
	throw(Exception(LOG_ERROR, "IcnFile", "Invalid ICN-File: No SSET-Section found"));
    _RPAL = new std::vector<uint8_t>(readU32BE(_stream));
    _stream.read((char*)&_RPAL->front(), _RPAL->size());

    

    _stream.read(signature, 4);
    if(strncmp(signature, "RTBL", 4))
	throw(Exception(LOG_ERROR, "IcnFile", "Invalid ICN-File: No SSET-Section found"));
    _RTBL = new std::vector<uint8_t>(readU32BE(_stream));
    _stream.read((char*)&_RTBL->front(), _RTBL->size());

}

Surface IcnFile::getSurface(uint32_t indexOfFile)
{
	uint8_t *paletteStart = &(*_RPAL)[(*_RTBL)[indexOfFile] << 4],
    		*fileStart = &(*_SSET)[(indexOfFile * ((_width * _height)>>1))];
	
	Surface pic(_width, _height, 8, _palette);
	
	//Now we can copy to surface
	uint8_t *dest = pic._pixels;
	for(int y = 0; y < _height;y++, dest += pic._pitch)
	    for(int x = 0; x < _width; x++) {
		uint8_t startPixel = fileStart[ (y*_width + x) >> 1];
		dest[x++] = paletteStart[startPixel >> 4];
		dest[x] = paletteStart[startPixel & 0x0F];
	    }

	LOG_INFO("IcnFile", "File Nr.: %d (Size: %dx%d)", indexOfFile, _width, _height);

	return pic;
}


Surface IcnFile::getSurfaceArray(uint32_t mapFileIndex, int tilesX, int tilesY, int tilesN)
{
    std::vector<uint16_t> &row = _map[mapFileIndex];

    int size = row.size();

    if((tilesX == 0) && (tilesY == 0) && (tilesN == 0)) {
	// guess what is best
	if(size == 24) {
	    // special case (radar station and light factory)
	    tilesX = 2;
	    tilesY = 2;
	    tilesN = 6;
	} else if((size % 9) == 0) {
	    tilesX = 3;
	    tilesY = 3;
	    tilesN = size / 9;
	} else if((size % 6) == 0) {
	    tilesX = 3;
	    tilesY = 2;
	    tilesN = size / 6;
	} else if((size % 4) == 0) {
	    tilesX = 2;
	    tilesY = 2;
	    tilesN = size / 4;
	} else if((size>=40) && ((size % 5) == 0)) {
	    tilesX = size/5;
	    tilesY = 5;
	    tilesN = 1;
	} else {
	    tilesX = 1;
	    tilesY = 1;
	    tilesN = size;
	}

    } else if( ((tilesX == 0) || (tilesY == 0)) && (tilesN == 0))
	throw Exception(LOG_ERROR, "IcnFile", "Invalid tile geometry");
    else if((tilesX == 0) && (tilesY == 0) && (tilesN != 0)) {
	if(size % tilesN == 0) {
	    // guess what is best
	    if((size % 3) == 0) {
		tilesX = size/3;
		tilesY = 3;
	    } else if((size % 2) == 0) {
		tilesX = size/2;
		tilesY = 2;
	    } else {
		tilesX = size;
		tilesY = 1;
	    }
	} else
    	    throw Exception(LOG_ERROR, "IcnFile", "Invalid tile geometry");
    } else if(tilesX*tilesY*tilesN != size)
    	    throw Exception(LOG_ERROR, "IcnFile", "Invalid tile geometry");

    Surface pic(_width*tilesX*tilesN,_height*tilesY, 8, _palette);

    std::vector<uint16_t>::const_iterator idx = row.begin();
    for(int n = 0; n < tilesN; n++)
	for(int y = 0; y < tilesY; y++)
	    for(int x = 0; x < tilesX; x++, idx++) {
		Surface subPic = getSurface(*idx);
		uint8_t *dest = pic._pixels + (pic._pitch)*y*_height + (x+n*tilesX) * _width;
		for(int y = 0; y < _height;y++, dest += pic._pitch)
		    memcpy(dest, &subPic._pixels[y*_width], _width);
	    }

    return pic;
}

Surface IcnFile::getSurfaceRow(uint32_t startIndex, uint32_t endIndex) {
    uint32_t numTiles = endIndex - startIndex + 1;
    Surface pic(_width*numTiles,_height, 8, _palette);

    for(unsigned int i = 0; i < numTiles; i++) {
	int indexOfFile = i+startIndex;
	Surface subPic = getSurface(indexOfFile);

	//Now we can copy to surface
	uint8_t *dest = pic._pixels + i*_width;
	for(int y = 0; y < _height;y++, dest += pic._pitch)
	    memcpy(dest, &subPic._pixels[y*_width], _width);
    }

    return pic;
}

}
