#include "eastwood/StdDef.h"
#include "eastwood/OStream.h"

namespace eastwood {

OStream::OStream(const std::ostream &stream) :
    std::ostream(stream.rdbuf())
{
}

template <typename T> inline
OStream& OStream::writeT(T &value)
{
    return reinterpret_cast<OStream&>(write(reinterpret_cast<char*>(&value), sizeof(value)));
}

template <typename T> inline
OStream& OStream::putT(T value)
{
    return writeT<T>(value);
}

OStream& OStream::putU16BE(uint16_t value)
{
    return putT<uint16_t>(htobe16(value));
}

OStream& OStream::putU16LE(uint16_t value)
{
    return putT<uint16_t>(htole16(value));
}

OStream& OStream::putU32BE(uint32_t value)
{
    return putT<uint32_t>(htobe32(value));
}

OStream& OStream::putU32LE(uint32_t value)
{
    return putT<uint32_t>(htole32(value));
}

#if __BYTE_ORDER == __BIG_ENDIAN
OStream& OStream::writeU16BE(uint16_t *buf, size_t n)
{
    return reinterpret_cast<OStream&>(write((char*)buf, n*sizeof(buf[0])));
}

OStream& OStream::writeU32BE(uint32_t *buf, size_t n)
{
    return reinterpret_cast<OStream&>(write((char*)buf, n*sizeof(buf[0])));
}

OStream& OStream::writeU16LE(uint16_t *buf, size_t n)
{
    for(size_t i = 0; i < n; i++)
	putU16LE(buf[i]);
    return *this;
}

OStream& OStream::writeU32LE(uint32_t *buf, size_t n)
{
    for(size_t i = 0; i < n; i++)
	putU32LE(buf[i]);
    return *this;
}
#else
OStream& OStream::writeU16BE(uint16_t *buf, size_t n)
{
    for(size_t i = 0; i < n; i++)
	putU16BE(buf[i]);
    return *this;
}

OStream& OStream::writeU32BE(uint32_t *buf, size_t n)
{
    for(size_t i = 0; i < n; i++)
	putU32BE(buf[i]);
    return *this;
}

OStream& OStream::writeU16LE(uint16_t *buf, size_t n)
{
    return reinterpret_cast<OStream&>(write(reinterpret_cast<char*>(buf), n*sizeof(buf[0])));
}

OStream& OStream::writeU32LE(uint32_t *buf, size_t n)
{
    return reinterpret_cast<OStream&>(write(reinterpret_cast<char*>(buf), n*sizeof(buf[0])));
}
#endif

std::streamsize OStream::sizep()
{
    std::streamsize size;
    std::streampos pos = tellp();
    seekp(0, std::ios::end);
    size = static_cast<std::streamsize>(tellp());
    seekp(pos);
    return size;
}

}

