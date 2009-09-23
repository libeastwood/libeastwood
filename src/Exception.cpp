#include "eastwood/Exception.h"

namespace eastwood {

Exception::Exception(logLevel level, std::string location, std::string message) :
    _level(level), _location(location), _message(message)
{
}

FileException::FileException(logLevel level, std::string location, std::string filename, std::string message) :
    Exception(level, location, message), _filename(filename)
{
}

FileNotFoundException::FileNotFoundException(logLevel level, std::string location, std::string filename) :
    FileException(level, location, filename, "File not found!")
{
}

NullSizeException::NullSizeException(logLevel level, std::string location, std::string message) :
    Exception(level, location, message)
{
}

}
