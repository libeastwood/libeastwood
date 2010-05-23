#include "eastwood/Exception.h"

namespace eastwood {

Exception::Exception(logLevel level, std::string location, std::string message) :
    _level(level), _location(location), _message(message)
{
}

Exception::Exception(logLevel level, std::string location, const char *format, ...) :
    _level(level), _location(location), _message(BUFSIZ, 0)
{
    size_t ret;
    va_list ap;

    va_start(ap, format);
    ret = vsprintf(const_cast<char*>(_message.data()), format, ap);
    va_end(ap);

}

FileException::FileException(logLevel level, std::string location, std::string filename, std::string message) :
    Exception(level, location, message), _filename(filename)
{
}

FileNotFoundException::FileNotFoundException(logLevel level, std::string location, std::string filename) :
    FileException(level, location, filename, "File not found!")
{
}

}
