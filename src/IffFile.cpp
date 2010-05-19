#include "eastwood/IffFile.h"

namespace eastwood {
static inline std::streambuf *getBuffer(IStream &stream, size_t size)
{
    if(static_cast<size_t>((stream.sizeg() - stream.tellg())) < size)
	throw(Exception(LOG_ERROR, "ISubStream()", "Size is greater than remaining file length"));
    std::string buffer(size, 0);
    stream.read(const_cast<char*>(buffer.data()), size);

    return new std::stringbuf(buffer);
}

std::string ID2string(IFF_ID id) {
    uint32_t strBE = htobe32(id);
    return std::string(reinterpret_cast<char*>(&strBE), sizeof(strBE));
}

IFFChunk::IFFChunk(IStream &stream) : _id(static_cast<IFF_ID>(stream.getU32BE())), _size(stream.getU32BE()) {
    std::ios::init(getBuffer(stream, _size));
}

IFFChunk::IFFChunk(uint32_t id, uint32_t size, IStream &stream) : _id(static_cast<IFF_ID>(id)), _size(size) {
    std::ios::init(getBuffer(stream, _size));
}

IFFChunk::~IFFChunk() {
    if(rdbuf()) delete rdbuf();
}

IffFile::IffFile(IStream &stream) : _id(static_cast<IFF_ID>(stream.getU32BE())), _size(stream.getU32BE()), _stream(stream), _formChunk(), _chunk() {
    if (_id != ID_FORM) {
	throw(Exception(LOG_ERROR, "IffFile", "IffFile input is not a FORM type IFF file"));
    }
    _id = static_cast<IFF_ID>(_stream.getU32BE());
    _stream.seekg(-_stream.gcount(), std::ios::cur);
    _formChunk.reset(new IFFChunk(_id, _size, _stream));
    _formChunk->seekg(4);
    next();
}

IffFile::~IffFile() {
}

std::tr1::shared_ptr<IFFChunk> IffFile::next() {
    _chunk.reset(new IFFChunk(*reinterpret_cast<IStream*>(_formChunk.get())));
    return _chunk;
}


}
