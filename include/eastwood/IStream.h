#ifndef EASTWOOD_ISTREAM_H
#define EASTWOOD_ISTREAM_H

#include <istream>

#include "eastwood/StdDef.h"

namespace eastwood {

class IStream : public std::istream
{
public:
    IStream() : std::istream(NULL) {}
    IStream(std::streambuf *sb) : std::istream(sb) {}
    IStream(const std::istream &stream) : std::istream(stream.rdbuf()) {}

    IStream& operator=(const std::istream &stream)
    {
	    std::ios::init(stream.rdbuf());
	    return *this;
    }

    IStream& operator=(const IStream &stream)
    {
	    std::ios::init(stream.rdbuf());
	    return *this;
    }

    std::streamsize sizeg()
    {
	    std::streamsize size;
	    std::streampos pos = tellg();
	    seekg(0, std::ios::end);
	    size = static_cast<std::streamsize>(tellg());
	    seekg(pos);
	    return size;
    }

    private:
	template <typename T> inline
	IStream& readT(T &value) 
	{
		return reinterpret_cast<IStream&>(this->read(reinterpret_cast<char*>(&value), sizeof(value)));
	}

	template <typename T> inline
	T getT() 
	{
		T value;
		readT<T>(value);
		return value;
	}

    public:
	uint16_t getU16BE() 
	{
		return htobe16(getT<uint16_t>());
	}

	uint16_t getU16LE() 
	{
		return htole16(getT<uint16_t>());
	}

	uint32_t getU32BE() 
	{
		return htobe32(getT<uint32_t>());
	}

	uint32_t getU32LE() 
	{
		return htole32(getT<uint32_t>());
	}

#if __BYTE_ORDER == __BIG_ENDIAN
	IStream& readU16BE(uint16_t *buf, size_t n)
	{
		return reinterpret_cast<IStream&>(this->read((char*)buf, n*sizeof(buf[0])));
	}

	IStream& readU32BE(uint32_t *buf, size_t n)
	{
		return reinterpret_cast<IStream&>(this->read((char*)buf, n*sizeof(buf[0])));
	}
	IStream& readU16LE(uint16_t *buf, size_t n)
	{
		for(size_t i = 0; i < n; i++)
			buf[i] = getU16LE();
		return *this;
	}

	IStream& readU32LE(uint32_t *buf, size_t n)
	{
		for(size_t i = 0; i < n; i++)
			buf[i] = getU32LE();
		return *this;
	}
#else
	IStream& readU16BE(uint16_t *buf, size_t n)
	{
		for(size_t i = 0; i < n; i++)
			buf[i] = getU16BE();
		return *this;
	}

	IStream& readU32BE(uint32_t *buf, size_t n)
	{
		for(size_t i = 0; i < n; i++)
			buf[i] = getU32BE();
		return *this;
	}

	IStream& readU16LE(uint16_t *buf, size_t n)
	{
		return reinterpret_cast<IStream&>(this->read(reinterpret_cast<char*>(buf), n*sizeof(buf[0])));
	}

	IStream& readU32LE(uint32_t *buf, size_t n)
	{
		return reinterpret_cast<IStream&>(this->read(reinterpret_cast<char*>(buf), n*sizeof(buf[0])));
	}
#endif
};

}

#endif // EASTWOOD_ISTREAM_H
