#ifndef	EASTWOOD_STDDEF_H
#define	EASTWOOD_STDDEF_H

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <tchar.h>
#else
#include <netinet/in.h>
#endif
#ifndef htobe16
#define htobe16(x) htons(x)
#define htobe32(x) htonl(x)
#if __BYTE_ORDER == __LITTLE_ENDIAN
#define htonl16(x) (x)
#define htonl32(x) (x)
#else
static inline uint16_t htonl16(uint16_t x) {
	return((x<<8)|(x>>8));
}

static inline uint32_t htonl32(uint32_t x) {
	return((x<<24)|((x<<8)&0x00FF0000)|((x>>8)&0x0000FF00)|(x>>24));
}
#endif
#endif

#endif // EASTWOOD_STDDEF_H
