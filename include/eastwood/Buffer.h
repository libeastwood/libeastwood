#ifndef EASTWOOD_BUFFER_H
#define EASTWOOD_BUFFER_H

#include <tr1/memory>
#include <tr1/shared_ptr.h>

namespace eastwood {

template <typename T>
class Buffer
{
    public:
	inline Buffer(T* buffer) : _buffer(buffer) {}

	inline Buffer(const Buffer& buffer) {
	    *this = buffer;
	}

	inline Buffer &operator=(const Buffer &buffer) {
	    _buffer = buffer;
	    return *this;
	} 

	inline virtual ~Buffer() {
	    delete [] _buffer;
	};

	inline virtual operator T*() {
	    return _buffer;
	}

    private:
	T* _buffer;
};

typedef Buffer<uint8_t> Bytes;
typedef std::tr1::shared_ptr<Bytes> BytesPtr;
}

#endif // EASTWOOD_BUFFER_H
