#include "StdDef.h"
#include "IStream.h"

namespace eastwood {

template <typename T>
static inline T readStream(std::istream &stream) {
    T value;
    stream.read((char*)&value, sizeof(value));
    return value;
}

IStream::IStream(const std::istream &stream) :
    std::istream(stream.rdbuf()), _size(-1)
{
}

uint16_t IStream::getU16BE() 
{
    return htobe16(readStream<uint16_t>(*this));
}

uint16_t IStream::getU16LE() 
{
    return htole16(readStream<uint16_t>(*this));
}


uint32_t IStream::getU32BE() 
{
    return htobe32(readStream<uint32_t>(*this));    
}

uint32_t IStream::getU32LE() 
{
    return htole32(readStream<uint32_t>(*this));
}

#if __BYTE_ORDER == __BIG_ENDIAN
IStream& IStream::readU16BE(uint16_t *buf, size_t n)
{
    read((char*)buf, n*sizeof(buf[0]));
    return *this;
}

IStream& IStream::readU32BE(uint32_t *buf, size_t n)
{
    read((char*)buf, n*sizeof(buf[0]));
    return *this;
}

IStream& IStream::readU16LE(uint16_t *buf, size_t n)
{
    for(size_t i = 0; i < n; i++)
	buf[i] = getU16LE();
    return *this;
}

IStream& IStream::readU32LE(uint32_t *buf, size_t n)
{
    for(size_t i = 0; i < n; i++)
	buf[i] = getU32LE();
    return *this;
}
#else
IStream& IStream::readU16BE(uint16_t *buf, size_t n)
{
    for(size_t i = 0; i < n; i++)
	buf[i] = getU16BE();
    return *this;
}

IStream& IStream::readU32BE(uint32_t *buf, size_t n)
{
    for(size_t i = 0; i < n; i++)
	buf[i] = getU32BE();
    return *this;
}

IStream& IStream::readU16LE(uint16_t *buf, size_t n)
{
    read((char*)buf, n*sizeof(buf[0]));
    return *this;
}

IStream& IStream::readU32LE(uint32_t *buf, size_t n)
{
    read((char*)buf, n*sizeof(buf[0]));
    return *this;
}
#endif

std::streamsize IStream::size()
{
    if(_size == -1) {
    	std::streampos pos = tellg();
    	seekg(0, std::ios::end);
    	_size = static_cast<std::streamsize>(tellg());
    	seekg(pos);
    }
    return _size;
}

}
