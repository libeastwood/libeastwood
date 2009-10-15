#ifndef	EASTWOOD_STDDEF_H
#define	EASTWOOD_STDDEF_H

#include <algorithm>
#include <istream>

#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <tr1/cinttypes>

#ifdef _WIN32
  #ifndef __LITTLE_ENDIAN
    #define __LITTLE_ENDIAN 1234
  #endif
  #ifndef __BYTE_ORDER
    #define __BYTE_ORDER __LITTLE_ENDIAN
  #endif
#ifdef _MSC_VER
  #define usleep Sleep
#else
  #include <unistd.h>
#endif
  #include <tchar.h>
  #include <winsock2.h>
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
static inline uint16_t htobe16(uint16_t x)
{
    std::swap(((uint8_t*)&x)[0], ((uint8_t*)&x)[1]);
    return x;
}

static inline uint32_t htobe32(uint32_t x)
{
    std::swap(((uint8_t*)&x)[0], ((uint8_t*)&x)[3]);
    std::swap(((uint8_t*)&x)[1], ((uint8_t*)&x)[2]);
    return x;
}

  #endif
#endif

//TODO: portability?
static inline int delayMillis(uint32_t msec)
{
    return usleep(msec * 1000);
}

static inline void stringToUpper(std::string &str)
{
    for(std::string::iterator it = str.begin();
            it != str.end(); ++it)
        *it = toupper(*it);
}

#endif // EASTWOOD_STDDEF_H
