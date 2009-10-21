#include "eastwood/StdDef.h"
#include "eastwood/ISubStream.h"
#include "eastwood/Exception.h"

namespace eastwood {

static inline std::streambuf *getBuffer(IStream &stream, size_t size)
{
    if(static_cast<size_t>((stream.sizeg() - stream.tellg())) < size)
	throw(Exception(LOG_ERROR, "ISubStream()", "Size is greater than remaining file length"));
    std::string buffer(size, 0);
    stream.read(const_cast<char*>(buffer.data()), size);
    return new std::stringbuf(buffer);

}

ISubStream::ISubStream(IStream &stream, size_t size) : IStream(getBuffer(stream, size))
{
    
}

}
