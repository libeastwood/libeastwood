#ifndef EASTWOOD_IOSTREAM_H
#define EASTWOOD_IOSTREAM_H

#include <iostream>

#include "eastwood/IStream.h"
#include "eastwood/OStream.h"

namespace eastwood {

class IOStream : public IStream, public OStream
{
public:
    explicit IOStream() {}
    explicit IOStream(std::streambuf *sb) : IStream(sb), OStream(sb) {}
};

}

#endif // EASTWOOD_ISTREAM_H
