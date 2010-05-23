#include "eastwood/IffFile.h"
#include <iostream>

namespace eastwood {
static inline std::streambuf *getBuffer(IStream &stream, size_t size)
{
    if(static_cast<size_t>(stream.sizeg() - stream.tellg()) < size)
	throw(Exception(LOG_ERROR, "IffFile()", "Size is greater than remaining file length"));
    std::string buffer(size, 0);
    stream.read(const_cast<char*>(buffer.data()), static_cast<std::streamsize>(size));

    return new std::stringbuf(buffer);
}

std::string ID2string(IFF_ID id) {
    uint32_t strBE = htobe32(id);
    return std::string(reinterpret_cast<char*>(&strBE), sizeof(strBE));
}

IFFChunk::IFFChunk(IStream &stream) : id(static_cast<IFF_ID>(stream.getU32BE())), size(stream.getU32BE()) {
    std::ios::init(getBuffer(stream, size));
}

IFFChunk::IFFChunk(uint32_t id, uint32_t size, IStream &stream) : id(static_cast<IFF_ID>(id)), size(size) {
    std::ios::init(getBuffer(stream, size));
}

IFFChunk::~IFFChunk() {
    if(rdbuf()) delete rdbuf();
}

IFFGroupChunk::IFFGroupChunk(uint32_t id, IStream &stream) : IFFChunk(), groupId() {
    this->id = static_cast<IFF_ID>(id);
    size = stream.getU32BE()-stream.gcount();
    groupId = static_cast<IFF_ID>(stream.getU32BE());
    std::ios::init(getBuffer(stream, size));
}

IffFile::IffFile(std::istream &stream) :
    _stream(reinterpret_cast<IStream&>(stream)), _formChunk(), _chunk() {
    next();
}

IffFile::~IffFile() {
}

IffChunk IffFile::next() {
    while(!_formChunk.empty() && _formChunk.back()->tellg() == _formChunk.back()->sizeg()) {
	_formChunk.pop_back();
	if(_formChunk.empty() && _stream.tellg() == _stream.sizeg()) {
	    _chunk.reset();
	    return _chunk;
	}
    }
    uint32_t id = (_formChunk.empty() ? _stream : *_formChunk.back()).getU32BE();
    switch(id) {
	case ID_FORM:
	case ID_CAT:
	case ID_LIST:
	case ID_PROP:
	    _formChunk.push_back(GroupChunk(new IFFGroupChunk(id, _formChunk.empty() ? _stream : *_formChunk.back())));
	    next();
	    break;
	default:
	    if(_formChunk.empty())
    		throw(Exception(LOG_ERROR, "IffFile::next()", "Invalid EA IFF 85 group identifier"));
    	    _chunk.reset(new IFFChunk(id, (_formChunk.empty() ? _stream : *_formChunk.back()).getU32BE(), _formChunk.empty() ? _stream : *_formChunk.back()));
    }
    return _chunk;
}


}
