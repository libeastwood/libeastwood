#ifndef	EASTWOOD_STDDEF_H
#define	EASTWOOD_STDDEF_H

#include <inttypes.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

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
    return((x<<8)|(x>>8));
}

static inline uint32_t htobe32(uint32_t x) {
    return((x<<24)|((x<<8)&0x00FF0000)|((x>>8)&0x0000FF00)|(x>>24));
}
#endif
#endif

#endif // EASTWOOD_STDDEF_H
