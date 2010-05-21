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
	OStream(const std::ostream &stream);
	OStream& putU16BE(uint16_t value);
	OStream& putU16LE(uint16_t value);
	OStream& putU32BE(uint32_t value);
	OStream& putU32LE(uint32_t value);

	OStream& writeU16BE(uint16_t *buf, size_t n);
	OStream& writeU16LE(uint16_t *buf, size_t n);
	OStream& writeU32BE(uint32_t *buf, size_t n);
	OStream& writeU32LE(uint32_t *buf, size_t n);

	std::streamsize sizep();

    private:
	template <typename T> inline
	    OStream& writeT(T &value);
	template <typename T> inline
	    OStream& putT(T value);
};

}

#endif // EASTWOOD_OSTREAM_H

