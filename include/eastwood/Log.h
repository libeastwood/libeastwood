#ifndef EASTWOOD_LOG_H
#define EASTWOOD_LOG_H

#include <errno.h>

#define clean_errno() (errno == 0 ? "None" : strerror(errno))

#ifdef NDEBUG
#define LOG_DEBUG(M, ...)
#else
#define LOG_DEBUG(M, ...) fprintf(stderr, "[DEBUG] (%s:%d): " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#endif

#define LOG_FATAL(...) ((void)(0))
#define LOG_ERROR(M, ...) fprintf(stderr, "[ERROR] (%s:%d: errno: %s) " M "\n", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)
#define LOG_WARNING(M, ...) fprintf(stderr, "[WARN] (%s:%d: errno: %s) " M "\n", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)
#define LOG_INFO(M, ...) fprintf(stderr, "[INFO] (%s:%d): " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#endif // EASTWOOD_LOG_H
