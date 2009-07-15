#ifndef	EASTWOOD_STDDEF_H
#define	EASTWOOD_STDDEF_H

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef _WIN32
#include <endian.h>
#endif
#if defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN
#define SwapLE16(x) __bswap_16(x)
#define SwapLE32(x) __bswap_32(x)
#define SwapBE16(x) (x)
#define SwapBE32(x) (x)
#else
#define SwapLE16(x) (x)
#define SwapLE32(x) (x)
#ifndef _WIN32
#define SwapBE16(x) __bswap_16(x)
#define SwapBE32(x) __bswap_32(x)
#else
static inline uint16_t SwapBE16(uint16_t x) {
    asm("xchg %b0,%h0" : "=q" (x) :  "0" (x));
}

static inline uint32_t SwapBE32(uint32_t x) {
    asm("bswap %0" : "=r" (x) : "0" (x));
}
#endif
#endif

#endif // EASTWOOD_STDDEF_H
