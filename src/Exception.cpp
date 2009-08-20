#include "Exception.h"
using namespace eastwood;

Exception::Exception(logLevel level, std::string location, std::string message) :
    m_level(level), m_location(location), m_message(message)
{
}

FileException::FileException(logLevel level, std::string location, std::string filename, std::string message) :
    Exception(level, location, message), m_filename(filename)
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

