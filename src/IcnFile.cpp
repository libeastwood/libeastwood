#include "eastwood/StdDef.h"

#include "eastwood/Exception.h"
#include "eastwood/IcnFile.h"
#include "eastwood/IffFile.h"
#include "eastwood/Log.h"

namespace eastwood {

IcnFile::IcnFile(std::istream &stream, const Palette &palette, const MapFile &map) :
    _palette(palette), _map(map), _SSET(), _RPAL(), _RTBL(), _bpp(0), _tileSize(0), _width(0), _height(0)
{
    readHeader(stream);
}

IcnFile::IcnFile(std::istream &stream, const Palette &palette) :
    _palette(palette), _map(), _SSET(), _RPAL(), _RTBL(), _bpp(0), _tileSize(0), _width(0), _height(0)
{
    readHeader(stream);
}

IcnFile::IcnFile(std::istream &stream) :
    _palette(), _map(), _SSET(), _RPAL(), _RTBL(), _bpp(0), _tileSize(0), _width(0), _height(0)
{
    readHeader(stream);
}

void IcnFile::readHeader(std::istream &stream)
{
    IffFile iff(stream);

    if(iff.getGroupType() != ID_ICON)
	throw(Exception(LOG_ERROR, __FUNCTION__, "Invalid ICN-File: No ICON chunk found"));

    std::tr1::shared_ptr<IFFChunk> chunk = iff.getChunk();
    // Session Information
    if(chunk->id != ID_SINF)
	throw(Exception(LOG_ERROR, __FUNCTION__, "Invalid ICN-File: No SINF chunk found "));

    _width = chunk->get();
    _height = chunk->get();
    auto shift = chunk->get();
    _width <<= shift;
    _height <<= shift;
    _bpp = chunk->get();
    _tileSize = ((_width*_height)<<shift)>>_bpp;

    chunk = iff.next();

    // Structure Set
    if(chunk->id != ID_SSET)
	throw(Exception(LOG_ERROR, __FUNCTION__, "Invalid ICN-File: No SSET chunk found"));

    auto tmp = chunk->getU16LE();
    _SSET.resize(chunk->getU16LE()/_tileSize, std::vector<uint8_t>(_tileSize));

    if(tmp != 0 || chunk->getU32LE() != ID_FILLER)
	throw(Exception(LOG_WARNING, __FUNCTION__, "Suspicious ICN-File: Found non-null bytes where null bytes expected"));
    for(std::vector<std::vector<uint8_t> >::iterator file = _SSET.begin(); file != _SSET.end(); file++)
    	chunk->read(reinterpret_cast<char*>(&file->front()), file->size());

    chunk = iff.next();

    // RIFF Palette
    if(chunk->id != ID_RPAL)
	throw(Exception(LOG_ERROR, __FUNCTION__, "Invalid ICN-File: No RPAL chunk found"));
    _RPAL.resize(chunk->size >> _bpp, std::vector<uint8_t>(1<<_bpp));
    for(std::vector<std::vector<uint8_t> >::iterator pal = _RPAL.begin(); pal != _RPAL.end(); pal++)
    	chunk->read(reinterpret_cast<char*>(&pal->front()), pal->size());
    
    chunk = iff.next();

    // Reference table
    if(chunk->id != ID_RTBL)
	throw(Exception(LOG_ERROR, __FUNCTION__, "Invalid ICN-File: No RTBL chunk found"));
    _RTBL.resize(chunk->size);

    chunk->read(reinterpret_cast<char*>(&_RTBL.front()), _RTBL.size());

}

IcnFile::~IcnFile()
{
}

void IcnFile::createImage(uint16_t index, uint8_t *dest, uint16_t pitch)
{
    const std::vector<uint8_t>
	&palette = _RPAL.at(_RTBL.at(index)),
	&file = _SSET.at(index);

    for(uint16_t y = 0, i = 0; y < _height; y++, dest += pitch)
	for(uint16_t x = 0; x < _width; x++) {
	    dest[x++] = palette[file[i] >> _bpp];
	    dest[x] = palette[file[i++] & ((1<<_bpp)-1)];
	}
}

Surface IcnFile::getSurface(uint16_t index)
{
    if(!_palette)
	throw(Exception(LOG_ERROR, __FUNCTION__, "Object initialized without required Palette"));

    Surface pic(_width, _height, 8, _palette);

    createImage(index, pic, pic.pitch());

    LOG_INFO("File Nr.: %d (Size: %dx%d)", index, _width, _height);

    return pic;
}

Surface IcnFile::getTiles(uint16_t index, bool frameByFrame)
{
    if(!_map)
	throw(Exception(LOG_ERROR, __FUNCTION__,"Object initialized without required MapFile"));
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
		createImage(*idx, static_cast<uint8_t*>(pic) + (pic.pitch())*y*_height + (x+n*tilesX) * _width, pic.pitch());

    return pic;
}

}
