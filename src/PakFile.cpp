#include "StdDef.h"

#include "Exception.h"
#include "Log.h"
#include "PakFile.h"

using namespace eastwood;

PakFile::PakFile(std::istream &stream) : _stream(stream), _fileEntry(std::vector<PakFileEntry>())
{
    readIndex();
}

PakFile::~PakFile()
{
}

void PakFile::readIndex()
{
    char name[256];

    while(1) {
        PakFileEntry fileEntry = { 0, 0, "" };
        _stream.read((char*)&fileEntry.startOffset, sizeof(fileEntry.startOffset));

        // pak-files are always little endian encoded
        fileEntry.startOffset = htole32(fileEntry.startOffset);

        _stream.getline(name, 256, 0);
        fileEntry.fileName = name;

        LOG_INFO("PakFile", "Found file %s", name);

        if(_fileEntry.size() > 0)
            _fileEntry.back().endOffset = fileEntry.startOffset - 1;

        _fileEntry.push_back(fileEntry);
        if(_stream.peek() == 0x0)
        {
            _stream.seekg(0, std::ios::end);
            _fileEntry.back().endOffset = (uint32_t)_stream.tellg() - 1;
            break;
        }
    }
}

std::istream *PakFile::getFileStream(std::string fileName)
{
    uint32_t size;
    std::string content;
    PakFileEntry fileEntry = { 0, 0, "" };
    for(std::vector<PakFileEntry>::iterator it = _fileEntry.begin(); it <= _fileEntry.end(); it++ )
    {
        if(it == _fileEntry.end())
            throw(FileNotFoundException(LOG_ERROR, "PakFile", fileName));

        if((fileEntry = *it).fileName == fileName)
            break;
    }

    size = fileEntry.endOffset - fileEntry.startOffset + 1;

    if(size == 0)
        throw(NullSizeException(LOG_ERROR, "PakFile", fileName));

    _stream.seekg(fileEntry.startOffset, std::ios::beg);

    //FIXME:
#if 0
    _stream >> std::noskipws >> std::setw(size) >> content;
#else
    for(uint32_t i = 0; i < size; i++)
        content += _stream.get();
#endif

    return new std::istringstream(content);
}
// vim:ts=8:sw=4:et
