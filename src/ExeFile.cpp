#include "StdDef.h"
#include "ExeFile.h"
#include "Exception.h"

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
    if(_header.signature != MAGIC1 || _header.signature != MAGIC2)
	throw(Exception(LOG_ERROR, "ExeFile", "Invalid signature"));
    printf("%x\n", _header.signature);
    _relocations = std::vector<uint32_t>(_header.relocations);
    seekg(_header.relocTable, std::ios::beg);
    readU32LE(&_relocations.front(), _relocations.size());

    _codeSegment = _header.headerSize << 4;
}

IStream& ExeFile::seekSegOff(uint32_t segoff)
{
    seekg(_codeSegment + segoff, std::ios::beg);
    return *this;
}

IStream& ExeFile::seekSegOff(uint16_t seg, uint16_t off)
{
    return seekSegOff((seg<<4) + off);
}

}
