#ifndef EASTWOOD_LOG_H
#define EASTWOOD_LOG_H
typedef enum logLevel_e
{
    LOG_FATAL,
    LOG_ERROR,
    LOG_WARNING,
    LOG_INFO
} logLevel;

#define LOG(...)           ((void)(0))
#define LOG_FATAL(...)     ((void)(0))
#define LOG_ERROR(...)     ((void)(0))
#define LOG_WARNING(...)   ((void)(0))
#define LOG_INFO(...)      ((void)(0))

#define LOG_INDENT(...)      ((void)(0))
#define LOG_UNINDENT(...)    ((void)(0))
#endif // EASTWOOD_LOG_H
