#ifndef EASTWOOD_ISTREAM_H
#define EASTWOOD_ISTREAM_H

#include <istream>

namespace eastwood {

class IStream : public std::istream
{
    public:
	IStream() {}
	IStream(const std::istream &stream);
	uint16_t getU16BE();
	uint16_t getU16LE();
	uint32_t getU32BE();
	uint32_t getU32LE();

	IStream& readU16BE(uint16_t *buf, size_t n);
	IStream& readU16LE(uint16_t *buf, size_t n);
	IStream& readU32BE(uint32_t *buf, size_t n);
	IStream& readU32LE(uint32_t *buf, size_t n);

	std::streamsize size();

    private:
	template <typename T> inline
	    IStream& readT(T &value);
	template <typename T> inline
	    T getT();

};

}

#endif // EASTWOOD_ISTREAM_H
