#ifndef	EASTWOOD_IFF_H
#define	EASTWOOD_IFF_H

#include "eastwood/StdDef.h"

#if __BYTE_ORDER == __LITTLE_ENDIAN
    #define MKID_BE(a,b,c,d) ((uint32_t) (d | (c << 8) | (b << 16) | (a << 24)))
#else
    #define MKID_BE(a,b,c,d) ((uint32_t) (a | (b << 8) | (c << 16) | (d << 24)))
#endif

enum IFF_ID {
    ID_FORM = MKID_BE('F','O','R','M'),
    ID_ICON = MKID_BE('I','C','O','N'),
    ID_RTBL = MKID_BE('R','T','B','L'),
    ID_RPAL = MKID_BE('R','P','A','L'),
    ID_SSET = MKID_BE('S','S','E','T'),
    ID_SINF = MKID_BE('S','I','N','F'),
    ID_FILLER = 0
};
#endif // EASTWOOD_IFF_H
