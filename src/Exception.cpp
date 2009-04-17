#include "Exception.h"
using namespace eastwood;

Exception::Exception(logLevel level, std::string location, std::string message)
{
}

FileNotFoundException::FileNotFoundException(logLevel level, std::string location, std::string message) : Exception(level, location, message)
{
}

NullSizeException::NullSizeException(logLevel level, std::string location, std::string message) : Exception(level, location, message)
{
}

