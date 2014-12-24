#include "eastwood/Exception.h"

namespace eastwood {

Exception::Exception(logLevel level, const std::string& message) :
    _level(level), _message(message)
{
}

Exception::Exception(logLevel level, const char *format, ...) :
    _level(level), _message(BUFSIZ, 0)
{
    va_list ap;

    va_start(ap, format);
    vsprintf(const_cast<char*>(_message.data()), format, ap);
    va_end(ap);
}

FileException::FileException(logLevel level, const std::string& filename, const std::string& message) :
    Exception(level, message), _filename(filename)
{
}

FileNotFoundException::FileNotFoundException(logLevel level, const std::string& filename) :
    FileException(level, filename, "File not found!")
{
}

}
