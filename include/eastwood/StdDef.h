#ifndef	EASTWOOD_STDDEF_H
#define	EASTWOOD_STDDEF_H

#include <algorithm>
#include <istream>

#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#ifndef _MSC_VER
  #include <tr1/cinttypes>
  #include <tr1/memory>
  #include <tr1/shared_ptr.h>
#else
  #include <memory>
  typedef signed char int8_t;
  typedef unsigned char   uint8_t;
  typedef short  int16_t;
  typedef unsigned short  uint16_t;
  typedef int  int32_t;
  typedef unsigned   uint32_t;
#endif

#ifndef offsetof
  #define offsetof(type, member)  __builtin_offsetof (type, member)
#endif

#ifndef restrict
  #define restrict __restrict__
#endif

#if defined(__GNUC__)
  #define GNUC_ATTRIBUTE(attr) __attribute__(attr)
  #define PACK
#else
  #define GNUC_ATTRIBUTE(attr)
  #define PACK _Pragma("pack(1)")
#endif

#ifdef _WIN32
  #ifndef __LITTLE_ENDIAN
    #define __LITTLE_ENDIAN 1234
  #endif
  #ifndef __BYTE_ORDER
    #define __BYTE_ORDER __LITTLE_ENDIAN
  #endif
  #include <tchar.h>
  #include <winsock2.h>
  #ifdef _MSC_VER
    #define usleep Sleep
    #define snprintf _snprintf
    #ifdef min
      #undef min
    #endif
  #else
    #include <unistd.h>
  #endif
#else
  #include <unistd.h>
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

namespace eastwood {

static inline void delayMillis(uint32_t msec)
{
    usleep(msec * 1000);
}

}
#endif // EASTWOOD_STDDEF_H
