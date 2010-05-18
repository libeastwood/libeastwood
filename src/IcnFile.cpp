#include "eastwood/StdDef.h"

#include "eastwood/Exception.h"
#include "eastwood/IcnFile.h"
#include "eastwood/IffFile.h"
#include "eastwood/Log.h"

namespace eastwood {

IcnFile::IcnFile(std::istream &stream, MapFile &map, Palette palette) :
    Decode(stream, 0, 0, palette),
    _map(map), _SSET(), _RPAL(), _RTBL(), _bpp(0), _tileSize(0)
{
    uint16_t tmp;
    uint8_t shift;

    _stream.seekg(0, std::ios::beg);
    
    IffFile iff(_stream);

    if(iff.getFORMType() != ID_ICON)
	throw(Exception(LOG_ERROR, "IcnFile", "Invalid ICN-File: No ICON chunk found"));

    IFFChunk *chunk = iff.getChunk();
    // Session Information
    if(chunk->_id != ID_SINF)
	throw(Exception(LOG_ERROR, "IcnFile", "Invalid ICN-File: No SINF chunk found "));

    _width = chunk->get();
    _height = chunk->get();
    shift = chunk->get();
    _width <<= shift;
    _height <<= shift;
    _bpp = chunk->get();
    _tileSize = ((_width*_height)<<shift)>>_bpp;

    iff.next();

    // Structure Set
    if(chunk->_id != ID_SSET)
	throw(Exception(LOG_ERROR, "IcnFile", "Invalid ICN-File: No SSET chunk found"));

    tmp = chunk->getU16LE();
    _SSET.resize(chunk->getU16LE()/_tileSize, std::vector<uint8_t>(_tileSize));

    if(tmp != 0 || chunk->getU32LE() != ID_FILLER)
	throw(Exception(LOG_WARNING, "IcnFile", "Suspicious ICN-File: Found non-null bytes where null bytes expected"));
    for(std::vector<std::vector<uint8_t> >::iterator file = _SSET.begin(); file != _SSET.end(); file++)
    	chunk->read((char*)&file->front(), file->size());

    iff.next();

    // RIFF Palette
    if(chunk->_id != ID_RPAL)
	throw(Exception(LOG_ERROR, "IcnFile", "Invalid ICN-File: No RPAL chunk found"));
    _RPAL.resize(chunk->_size >> _bpp, std::vector<uint8_t>(1<<_bpp));
    for(std::vector<std::vector<uint8_t> >::iterator pal = _RPAL.begin(); pal != _RPAL.end(); pal++)
    	chunk->read((char*)&pal->front(), pal->size());
    
    iff.next();

    // Reference table
    if(chunk->_id != ID_RTBL)
	throw(Exception(LOG_ERROR, "IcnFile", "Invalid ICN-File: No RTBL chunk found"));
    _RTBL.resize(chunk->_size);

    chunk->read((char*)&_RTBL.front(), _RTBL.size());

}

IcnFile::~IcnFile()
{
}

void IcnFile::createImage(uint16_t index, uint8_t *dest, uint16_t pitch)
{
    const std::vector<uint8_t>
	&palette = _RPAL.at(_RTBL.at(index)),
	&file = _SSET.at(index);

    uint16_t i = 0;
    for(uint16_t y = 0; y < _height; y++, dest += pitch)
	for(uint16_t x = 0; x < _width; x++) {
	    dest[x++] = palette[file[i] >> _bpp];
	    dest[x] = palette[file[i++] & ((1<<_bpp)-1)];
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
