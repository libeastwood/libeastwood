#include "eastwood/StdDef.h"
#include "eastwood/ExeFile.h"
#include "eastwood/Exception.h"

#define MAGIC1 0x5a4d
#define MAGIC2 0x4d5a

namespace eastwood {

ExeFile::ExeFile(std::istream &stream) :
    IStream(stream), _header(ExeHeader()), _relocations(0), _codeSegment(0)
{
    readHeader();
}

void ExeFile::readHeader()
{
    seekg(0, std::ios::beg);
    readU16LE((uint16_t*)&_header, sizeof(_header)/sizeof(uint16_t));

    // File header must start with "MZ" or "ZM"
    if(_header.signature != MAGIC1 && _header.signature != MAGIC2)
	throw(Exception(LOG_ERROR, "ExeFile", "Invalid signature"));
    _relocations = std::vector<uint32_t>(_header.relocations);
    seekg(_header.relocTable, std::ios::beg);
    readU32LE(&_relocations.front(), _relocations.size());

    _codeSegment = _header.headerSize << 4;
}

IStream& ExeFile::seekSegOff(uint32_t segoff)
{
    return seekSegOff(segoff >> 16, segoff & ((1<<16)-1));
}

IStream& ExeFile::seekSegOff(uint16_t seg, uint16_t off)
{
    seekg(_codeSegment + (seg<<4) + off);
    return *this;
}

uint16_t ExeFile::findOff(uint16_t seg, std::string str)
{
    std::vector<uint8_t> tmp(str.data(), str.data() + str.size());
    return findOff(seg, tmp);
}

uint16_t ExeFile::findOff(uint16_t seg, std::vector<uint8_t> &data)
{
    seekg(_codeSegment + (seg<<4));
    char buf[0xffff];    
    read(buf, sizeof(buf));

    for(uint16_t off = 0, end = (uint16_t)gcount() - data.size(); off < end; ++off)
	if(memcmp(&buf[off], &data.front(), data.size()) == 0)
	    return off;

    return -1;
}

}
