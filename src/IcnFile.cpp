#include "eastwood/StdDef.h"

#include "eastwood/Exception.h"
#include "eastwood/IcnFile.h"
#include "eastwood/IFFContainer.h"
#include "eastwood/Log.h"

namespace eastwood {

IcnFile::IcnFile(std::istream &stream, MapFile &map, Palette palette) :
    Decode(stream, 0, 0, palette),
    _map(map), _SSET(), _RPAL(), _RTBL(), _bpp(0), _tileSize(0)
{
    uint32_t sectionSize;
    uint8_t shift;

    _stream.seekg(0, std::ios::beg);
    
    if(_stream.getU32BE() != ID_FORM)
	throw(Exception(LOG_ERROR, "IcnFile", "Invalid ICN-File: No FORM chunk found"));

    if((sectionSize = _stream.getU32BE()) != _stream.sizeg() - (uint32_t)_stream.tellg())
	throw(Exception(LOG_ERROR, "IcnFile", "Invalid ICN-File: File size doesn't match size specified in header"));

    if(_stream.getU32BE() != ID_ICON)
	throw(Exception(LOG_ERROR, "IcnFile", "Invalid ICN-File: No ICON chunk found"));

    // Session Information
    if(_stream.getU32BE() != ID_SINF)
	throw(Exception(LOG_ERROR, "IcnFile", "Invalid ICN-File: No SINF chunk found "));

    sectionSize = _stream.getU32BE();

    _width = _stream.get();
    _height = _stream.get();
    shift = _stream.get();
    _width <<= shift;
    _height <<= shift;
    _bpp = _stream.get();
    _tileSize = ((_width*_height)<<shift)>>_bpp;


    // Structure Set
    if(_stream.getU32BE() != ID_SSET)
	throw(Exception(LOG_ERROR, "IcnFile", "Invalid ICN-File: No SSET chunk found"));
    // Don't *really* have any idea what this is good for (sanity check?):
    // Section header looks like this:
    // [BBBB00LL]
    //
    // BBBB: Size is first stored as a 32 bit integer, big endian
    // 00: Two byte space (00)
    // LL: SSET section size - header size is stored again as 16 bit integer, little endian

    // So yeah, let's use this a sort of sanity check...
    if(_stream.getU32BE() - 8 != (sectionSize = _stream.getU16LE() + _stream.getU16LE()))
	throw(Exception(LOG_ERROR, "IcnFile", "Invalid ICN-File: SSET chunk size mismatch"));
    if(_stream.getU32BE() != ID_FILLER)
	throw(Exception(LOG_WARNING, "IcnFile", "Suspicious ICN-File: Found non-null bytes where null bytes expected"));
    _SSET.resize(sectionSize);
    _stream.read((char*)&_SSET.front(), _SSET.size());

    // RIFF Palette
    if(_stream.getU32BE() != ID_RPAL)
	throw(Exception(LOG_ERROR, "IcnFile", "Invalid ICN-File: No RPAL chunk found"));
    _RPAL.resize(_stream.getU32BE());
    _stream.read((char*)&_RPAL.front(), _RPAL.size());
    

    // Reference table
    if(_stream.getU32BE() != ID_RTBL)
	throw(Exception(LOG_ERROR, "IcnFile", "Invalid ICN-File: No RTBL chunk found"));
    _RTBL.resize(_stream.getU32BE());
    _stream.read((char*)&_RTBL.front(), _RTBL.size());
}

IcnFile::~IcnFile()
{
}

void IcnFile::createImage(uint16_t index, uint8_t *dest, uint16_t pitch)
{
    const uint8_t *paletteStart = &_RPAL.at(_RTBL.at(index) << _bpp),
	  *fileStart = &_SSET.at(index * _tileSize);

    for(uint16_t y = 0; y < _height; y++, dest += pitch)
	for(uint16_t x = 0; x < _width; x++) {
	    dest[x++] = paletteStart[*fileStart >> _bpp];
	    dest[x] = paletteStart[*fileStart++ & ((1<<_bpp)-1)];
	}
}

Surface IcnFile::getSurface(uint16_t index)
{
    Surface pic(_width, _height, 8, _palette);

    createImage(index, pic, pic.pitch());

    LOG_INFO("IcnFile", "File Nr.: %d (Size: %dx%d)", index, _width, _height);

    return pic;
}

Surface IcnFile::getTiles(uint16_t index, bool frameByFrame)
{
    const std::vector<uint16_t> &row = _map[index];

    int tilesX = 1,
	tilesY = 1,
	tilesN = row.size();

    if(frameByFrame) {
	// guess what is best
	if(tilesN == 24) {
	    // special case (radar station and light factory)
	    tilesX = 2;
	    tilesY = 2;
	    tilesN = 6;
	} else if((tilesN % 9) == 0) {
	    tilesX = 3;
	    tilesY = 3;
	    tilesN /= 9;
	} else if((tilesN % 6) == 0) {
	    tilesX = 3;
	    tilesY = 2;
	    tilesN /= 6;
	} else if((tilesN % 4) == 0) {
	    tilesX = 2;
	    tilesY = 2;
	    tilesN /= 4;
	} else if((tilesN>=40) && ((tilesN % 5) == 0)) {
	    tilesX = tilesN/5;
	    tilesY = 5;
	    tilesN = 1;
	}
    }
    Surface pic(_width*tilesX*tilesN,_height*tilesY, 8, _palette);

    std::vector<uint16_t>::const_iterator idx = row.begin();
    for(int n = 0; n < tilesN; n++)
	for(int y = 0; y < tilesY; y++)
	    for(int x = 0; x < tilesX; x++, idx++)
		createImage(*idx, (uint8_t*)pic + (pic.pitch())*y*_height + (x+n*tilesX) * _width, pic.pitch());

    return pic;
}

}
