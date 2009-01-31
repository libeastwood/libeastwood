#include "EastwoodException.h"

EastwoodException::EastwoodException(logLevel level, std::string location, std::string message)
{
}

FileNotFoundException::FileNotFoundException(logLevel level, std::string location, std::string message) : EastwoodException(level, location, message)
{
}

NullSizeException::NullSizeException(logLevel level, std::string location, std::string message) : EastwoodException(level, location, message)
{
}

