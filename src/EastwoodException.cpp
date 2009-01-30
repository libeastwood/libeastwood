#include "EastwoodException.h"

EastwoodException::EastwoodException(std::string message)
{
}

FileNotFoundException::FileNotFoundException(std::string filename) : EastwoodException(filename)
{
}

NullSizeException::NullSizeException(std::string filename) : EastwoodException(filename)
{
}

