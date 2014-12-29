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
        Exception(logLevel level, const std::string& location, const std::string& message);
        Exception(logLevel level, const std::string& location, const char *format, ...);        
        virtual ~Exception(){}
        virtual inline const logLevel getLogLevel() const noexcept { return _level; }
        virtual inline const std::string& getLocation() const noexcept { return _location; }
        virtual inline const std::string& getMessage() noexcept{ return _message; }

    protected:
        logLevel _level;
        std::string _location;
        std::string _message;
};

class FileException : public Exception {
    public:
        FileException(logLevel level, const std::string& location, const std::string& filename, const std::string& message);
        virtual inline const std::string& getFilename() const noexcept { return _filename; };

        virtual inline const std::string& getMessage() noexcept {
            std::stringstream ss;
            ss << _filename << ": " << _message;
            return std::move(ss.str());
        }

    private:
        std::string _filename;


};

class FileNotFoundException : public FileException {
    public:
        FileNotFoundException(logLevel level, const std::string& location, const std::string& filename);
};

}
#endif // EASTWOOD_EASTWOODEXCEPTION_H

// vim:ts=8:sw=4:et
