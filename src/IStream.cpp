#include "eastwood/StdDef.h"
#include "eastwood/IStream.h"

namespace eastwood {

IStream::IStream(const std::istream &stream) :
    std::istream(stream.rdbuf())
{
}

IStream& IStream::operator=(const std::istream &stream)
{
    std::ios::init(stream.rdbuf());

    return *this;
}

IStream& IStream::operator=(const IStream &stream)
{
    std::ios::init(stream.rdbuf());

    return *this;
}

template <typename T> inline
IStream& IStream::readT(T &value) {
    return reinterpret_cast<IStream&>(read(reinterpret_cast<char*>(&value), sizeof(value)));
}

template <typename T> inline
T IStream::getT() 
{
    T value;
    readT<T>(value);
    return value;
}

uint16_t IStream::getU16BE() 
{
    return htobe16(getT<uint16_t>());
}

uint16_t IStream::getU16LE() 
{
    return htole16(getT<uint16_t>());
}


uint32_t IStream::getU32BE() 
{
    return htobe32(getT<uint32_t>());
}

uint32_t IStream::getU32LE() 
{
    return htole32(getT<uint32_t>());
}

#if __BYTE_ORDER == __BIG_ENDIAN
IStream& IStream::readU16BE(uint16_t *buf, size_t n)
{
    return reinterpret_cast<IStream&>(read((char*)buf, n*sizeof(buf[0])));
}

IStream& IStream::readU32BE(uint32_t *buf, size_t n)
{
    return reinterpret_cast<IStream&>(read((char*)buf, n*sizeof(buf[0])));
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
    return reinterpret_cast<IStream&>(read(reinterpret_cast<char*>(buf), n*sizeof(buf[0])));
}

IStream& IStream::readU32LE(uint32_t *buf, size_t n)
{
    return reinterpret_cast<IStream&>(read(reinterpret_cast<char*>(buf), n*sizeof(buf[0])));
}
#endif

std::streamsize IStream::sizeg()
{
    std::streamsize size;
    std::streampos pos = tellg();
    seekg(0, std::ios::end);
    size = static_cast<std::streamsize>(tellg());
    seekg(pos);
    return size;
}

}
