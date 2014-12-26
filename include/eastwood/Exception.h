#ifndef	EASTWOOD_EXCEPTION_H
#define	EASTWOOD_EXCEPTION_H

#include <sstream>
#include <string>

#include "eastwood/StdDef.h"

namespace eastwood {

enum logLevel
{
    LOG_FATAL,
    LOG_ERROR,
    LOG_WARNING,
    LOG_INFO
};

class Exception {
    public:
       Exception(logLevel level, const char *location, const std::string& message) : _level(level), _location(std::move(location)), _message(std::move(message)){}
       Exception(logLevel level, const char *location, const std::string&& message) : _level(level), _location(std::move(std::string(location))), _message(std::move(message)) {}

       //Exception(logLevel level, char *location, const char* const message) : _level(level), _location(std::string(std::move(location))), _message(std::string(std::move(message))) {}

        Exception(logLevel level, const char *location, const char *format, ...);
 //       Exception(logLevel level, const char *format, ...);

        virtual ~Exception(){}
        virtual inline logLevel getLogLevel() const noexcept { return _level; }
        virtual inline const std::string& getMessage() const noexcept { return _message; }
        virtual inline const std::string& getLocation() const noexcept { return _location; }

    protected:
        logLevel _level;
        std::string _location;
        std::string _message;
};
class FileException : public Exception {
    public:
        FileException(logLevel level, const char *location, const std::string& filename, const std::string&& message);
        FileException(logLevel level, const char *location, std::string&& message);
        virtual inline const std::string& getFilename()
            const noexcept { return _filename; };

    public:
        std::string _filename;


};

class FileNotFoundException : public FileException {
    public:
        FileNotFoundException(logLevel level, const std::string& filename);
};
}
#endif // EASTWOOD_EASTWOODEXCEPTION_H

// vim:ts=8:sw=4:et
