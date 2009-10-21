#ifndef EASTWOOD_ISUBSTREAM_H
#define EASTWOOD_ISUBSTREAM_H

#include "IStream.h"

namespace eastwood {

class ISubStream : public IStream
{
    public:
	ISubStream() : IStream(){}
	ISubStream(IStream &stream, size_t size);

	ISubStream& operator=(const IStream &stream);
};

}

#endif // EASTWOOD_ISUBSTREAM_H
