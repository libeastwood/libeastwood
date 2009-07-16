#ifndef	EASTWOOD_STDDEF_H
#define	EASTWOOD_STDDEF_H

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <tchar.h>
#define htobe16(x) htons(x)
#define htobe32(x) htonl(x)
#define htonl16(x) (x)
#define htonl32(x) (x)
#else
#include <netinet/in.h>
#endif

#endif // EASTWOOD_STDDEF_H
