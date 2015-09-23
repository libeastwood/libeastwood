#ifndef EASTWOOD_OSTREAM_H
#define EASTWOOD_OSTREAM_H

#include <ostream>

#include "eastwood/StdDef.h"

namespace eastwood {

class OStream : public std::ostream
{
public:
    OStream() : std::ostream(NULL){};
    OStream(std::streambuf *sb) : std::ostream(sb) {}
    OStream(const std::ostream &stream) : std::ostream(stream.rdbuf()) {}

    std::streamsize sizep()
    {
	std::streamsize size;
	std::streampos pos = tellp();
	seekp(0, std::ios::end);
	size = static_cast<std::streamsize>(tellp());
	seekp(pos);
	return size;

    }

private:
    template <typename T> inline
    OStream& writeT(T &value)
    {
        return reinterpret_cast<OStream&>(this->write(reinterpret_cast<char*>(&value), sizeof(value)));
    }

    template <typename T> inline
    OStream& putT(T value)
    {
        return writeT<T>(value);
    }
    
public:
    OStream& putU16BE(uint16_t value)
    {
        return putT<uint16_t>(htobe16(value));
    }

    OStream& putU16LE(uint16_t value)
    {
        return putT<uint16_t>(htole16(value));
    }

    OStream& putU32BE(uint32_t value)
    {
        return putT<uint32_t>(htobe32(value));
    }

    OStream& putU32LE(uint32_t value)
    {
        return putT<uint32_t>(htole32(value));
    }

#if __BYTE_ORDER == __BIG_ENDIAN
    OStream& writeU16BE(uint16_t *buf, size_t n)
    {
        return reinterpret_cast<OStream&>(this->write((char*)buf, n*sizeof(buf[0])));
    }

    OStream& writeU32BE(uint32_t *buf, size_t n)
    {
        return reinterpret_cast<OStream&>(this->write((char*)buf, n*sizeof(buf[0])));
    }

    OStream& writeU16LE(uint16_t *buf, size_t n)
    {
        for(size_t i = 0; i < n; i++)
            putU16LE(buf[i]);
        return *this;
    }

    OStream& writeU32LE(uint32_t *buf, size_t n)
    {
        for(size_t i = 0; i < n; i++)
            putU32LE(buf[i]);
        return *this;
    }
#else
    OStream& writeU16BE(uint16_t *buf, size_t n)
    {
        for(size_t i = 0; i < n; i++)
            putU16BE(buf[i]);
        return *this;
    }

    OStream& writeU32BE(uint32_t *buf, size_t n)
    {
        for(size_t i = 0; i < n; i++)
            putU32BE(buf[i]);
        return *this;
    }

    OStream& writeU16LE(uint16_t *buf, size_t n)
    {
        return reinterpret_cast<OStream&>(this->write(reinterpret_cast<char*>(buf), n*sizeof(buf[0])));
    }

    OStream& writeU32LE(uint32_t *buf, size_t n)
    {
        return reinterpret_cast<OStream&>(this->write(reinterpret_cast<char*>(buf), n*sizeof(buf[0])));
    }
#endif
};

}

#endif // EASTWOOD_OSTREAM_H

