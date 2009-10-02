#include "eastwood/StdDef.h"

#include "eastwood/Exception.h"
#include "eastwood/Log.h"
#include "eastwood/PakFile.h"

namespace eastwood {

PakFile::PakFile(const std::istream &stream) :
    _stream(const_cast<IStream&>(reinterpret_cast<const IStream&>(stream))),
    _fileEntries(), _fileNames()
{
    readIndex();
}

PakFile::~PakFile()
{
    close();
}

void PakFile::close() {
    if(is_open()) {
	delete rdbuf();
	std::ios::init(NULL);
    }
}

void PakFile::open(std::string fileName) {
    close();

    std::map<std::string, FileEntry>::const_iterator fileEntry =_fileEntries.find(fileName);
    if(fileEntry != _fileEntries.end()) {
        _stream.seekg(fileEntry->second.first, std::ios::beg);
        std::string buffer(fileEntry->second.second, 0);
        _stream.read((char*)buffer.data(), buffer.size());
        
        std::ios::init(new std::stringbuf(buffer));
    } else
        throw(FileNotFoundException(LOG_ERROR, "PakFile", fileName));

}

void PakFile::readIndex()
{
    char name[256];
    uint32_t offset = _stream.getU32LE();

    while(_stream.peek()) {
        uint32_t start = offset,
                 size;

	_stream.getline(name, 256, 0);
        LOG_INFO("PakFile", "Found file %s", name);

        size = ((_stream.peek() != 0) ? (offset = _stream.getU32LE()) : _stream.size()) - start;

        _fileEntries.insert(make_pair(name, FileEntry(start, size)));
        _fileNames.push_back(name);
    }
}

}
// vim:ts=8:sw=4:et
