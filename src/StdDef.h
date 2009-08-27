#ifndef	EASTWOOD_STDDEF_H
#define	EASTWOOD_STDDEF_H

#include <inttypes.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <istream>

#ifdef _WIN32
#include <tchar.h>
#else
#include <netinet/in.h>
#endif
#ifndef htobe16
#define htobe16(x) htons(x)
#define htobe32(x) htonl(x)
#if __BYTE_ORDER == __LITTLE_ENDIAN
#define htole16(x) (x)
#define htole32(x) (x)
#else
static inline uint16_t htobe16(uint16_t x) {
    std::swap(((uint8_t*)&x)[0], ((uint8_t*)&x)[1]);
    return x;
}

static inline uint32_t htobe32(uint32_t x) {
    std::swap(((uint8_t*)&x)[0], ((uint8_t*)&x)[3]);
    std::swap(((uint8_t*)&x)[1], ((uint8_t*)&x)[2]);
    return x;
}
#endif
#endif

template <typename T>
static inline T readStream(std::istream &stream) {
    T value;
    stream.read((char*)&value, sizeof(value));
    return value;
}

static inline uint16_t readU16BE(std::istream &stream) {
    return htobe16(readStream<uint16_t>(stream));
}

static inline uint16_t readU16LE(std::istream &stream) {
    return htole16(readStream<uint16_t>(stream));
}

static inline uint32_t readU32BE(std::istream &stream) {
    return htobe32(readStream<uint32_t>(stream));
}

static inline uint32_t readU32LE(std::istream &stream) {
    return htole32(readStream<uint32_t>(stream));
}

static inline size_t getStreamSize(std::istream &stream) {
    	size_t size;
	std::streampos pos = stream.tellg();
    	stream.seekg(0, std::ios::end);
    	size = static_cast<std::streamoff>(stream.tellg());
    	stream.seekg(pos);
	return size;
}
#endif // EASTWOOD_STDDEF_H
