#include <string>
#include <sstream>
#include <iostream>
#include "eastwood/Exception.h"

namespace eastwood {


Exception::Exception(logLevel level, const char *location, const char *format, ...) : _level(level), _location(std::move(location)), _message(BUFSIZ, 0)
{
    va_list ap;

    va_start(ap, format);
    vsprintf(const_cast<char*>(_message.data()), format, ap);
    va_end(ap);
}

FileException::FileException(logLevel level, const char *location, const std::string& filename, const std::string&& message) : Exception(level, location, message), _filename(filename)
{
}

FileException::FileException(logLevel level, const char *location, std::string&& message) : Exception(level, location, message), _filename()
{
}

FileNotFoundException::FileNotFoundException(logLevel level, const std::string& filename) :
    FileException(level, nullptr, filename, std::move("file not found!"))
{
}

}
