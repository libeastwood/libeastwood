#ifndef	EASTWOOD_IFF_H
#define	EASTWOOD_IFF_H

#include "eastwood/StdDef.h"

#if __BYTE_ORDER == __LITTLE_ENDIAN
    #define MKID_BE(a,b,c,d) ((uint32_t) (d | (c << 8) | (b << 16) | (a << 24)))
#else
    #define MKID_BE(a,b,c,d) ((uint32_t) (a | (b << 8) | (c << 16) | (d << 24)))
#endif

enum IFF_ID {
    ID_CAT  = MKID_BE('C','A','T',' '),
    ID_DATA = MKID_BE('D','A','T','A'),
    ID_DESC = MKID_BE('D','E','S','C'),
    ID_EMC2 = MKID_BE('E','M','C','2'),
    ID_EVNT = MKID_BE('E','V','N','T'),
    ID_FORM = MKID_BE('F','O','R','M'),
    ID_ICON = MKID_BE('I','C','O','N'),
    ID_INFO = MKID_BE('I','N','F','O'),
    ID_MENT = MKID_BE('M','E','N','T'),
    ID_NAME = MKID_BE('N','A','M','E'),
    ID_ORDR = MKID_BE('O','R','D','R'),
    ID_RTBL = MKID_BE('R','T','B','L'),
    ID_RPAL = MKID_BE('R','P','A','L'),
    ID_SSET = MKID_BE('S','S','E','T'),
    ID_SINF = MKID_BE('S','I','N','F'),
    ID_TIMB = MKID_BE('T','I','M','B'),    
    ID_XDIR = MKID_BE('X','D','I','R'),    
    ID_XMID = MKID_BE('X','M','I','D'),
    ID_FILLER = 0
};
#endif // EASTWOOD_IFF_H
