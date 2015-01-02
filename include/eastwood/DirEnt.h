#ifndef EASTWOOD_DIRENT_H
#define	EASTWOOD_DIRENT_H

#include <sys/types.h>
#include <sys/stat.h>

#ifdef _MSC_VER
    #include "win32/dirent_win32.h"
#else
    #include <dirent.h>
#endif

#ifdef _WIN32
#define DIR_SEP '\\'
#else
#define DIR_SEP '/'
#endif

#endif	/* EASTWOOD_DIRENT_H */

