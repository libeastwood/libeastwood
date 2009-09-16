#ifndef	EASTWOOD_STDDEF_H
#define	EASTWOOD_STDDEF_H

#include <algorithm>
#include <istream>

#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <inttypes.h>

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
static inline void writeStream(T value, std::ostream &stream) {
    stream.write((char*)&value, sizeof(value));
}

static inline void writeU16BE(uint16_t x, std::ostream &stream) {
    writeStream<uint16_t>(htobe16(x), stream);
}

static inline void writeU16LE(uint16_t x, std::ostream &stream) {
    writeStream<uint16_t>(htole16(x), stream);
}

static inline void writeU32BE(uint16_t x, std::ostream &stream) {
    writeStream<uint32_t>(htobe32(x), stream);
}

static inline void writeU32LE(uint16_t x, std::ostream &stream) {
    writeStream<uint32_t>(htole32(x), stream);
}

#endif // EASTWOOD_STDDEF_H
