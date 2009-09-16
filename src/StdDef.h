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

#endif // EASTWOOD_STDDEF_H
