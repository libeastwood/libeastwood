#ifndef EASTWOOD_BUFFER_H
#define EASTWOOD_BUFFER_H

#include <tr1/memory>
#include <tr1/shared_ptr.h>

namespace eastwood {

enum BufferAllocator {
    BufMalloc,
    BufNew,
    BufNewArray
};

template <typename T>
class Buffer
{
    public:
	inline Buffer(T* buffer, BufferAllocator alloc = BufNewArray) :
	    _buffer(buffer), _alloc(alloc) {}

	inline Buffer(const Buffer& buffer) {
	    *this = buffer;
	}

	inline Buffer &operator=(const Buffer &buffer) {
	    _buffer = buffer._buffer;
	    _alloc = buffer._alloc;
	    return *this;
	} 

	inline virtual ~Buffer() {
	    switch(_alloc) {
		case BufMalloc:
		    free(_buffer);
		    break;
		case BufNew:
		    delete _buffer;
		    break;
		case BufNewArray:
		    delete [] _buffer;
		    break;
	    }
	};

	inline virtual operator T*() {
	    return _buffer;
	}

    private:
	T* _buffer;
	BufferAllocator _alloc;
};

typedef Buffer<uint8_t> Bytes;
typedef std::tr1::shared_ptr<Bytes> BytesPtr;
}

#endif // EASTWOOD_BUFFER_H
