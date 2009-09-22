#ifndef EASTWOOD_BUFFER_H
#define EASTWOOD_BUFFER_H

#include <memory>

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
typedef std::auto_ptr<Bytes > BytesPtr;
}

#endif // EASTWOOD_BUFFER_H
