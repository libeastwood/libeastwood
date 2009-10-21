#include "eastwood/IFFContainer.h"

namespace eastwood {

std::string ID2string(IFF_ID id) {
    uint32_t strBE = htobe32(id);
    return std::string(reinterpret_cast<char*>(&strBE), sizeof(strBE));
}

IFFParser::IFFChunkNav::IFFChunkNav(IStream &stream) :
    _id(static_cast<IFF_ID>(stream.getU32BE())), _size(stream.getU32BE())
{
    if(_id == ID_FORM) {
/*	_id = static_cast<IFF_ID>(stream.getU32BE());*/
	_size -= stream.gcount();
    	std::streampos pos = stream.tellg();
    	*(ISubStream*)this = ISubStream(stream, _size);
    	stream.seekg(pos);
    } else
    	*(ISubStream*)this = ISubStream(stream, _size);
}

IFFParser::IFFChunkNav& IFFParser::IFFChunkNav::operator=(IStream &stream) {
    _id = static_cast<IFF_ID>(stream.getU32BE());
    _size = stream.getU32BE();
    if(_id == ID_FORM) {
/*	_id = static_cast<IFF_ID>(stream.getU32BE());*/
	_size -= stream.gcount();
    	std::streampos pos = stream.tellg();
    	*(ISubStream*)this = ISubStream(stream, _size);
    	stream.seekg(pos);
    } else
    	*(ISubStream*)this = ISubStream(stream, _size);

    return *this;
}


void IFFParser::IFFChunkNav::setInputStream(IStream &input) {
    *this = input;

}


}
