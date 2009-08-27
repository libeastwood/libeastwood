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
        // pak-files are always little endian encoded
        PakFileEntry fileEntry = { readU32LE(_stream), 0, "" };

        _stream.getline(name, 256, 0);
        fileEntry.fileName = name;

        LOG_INFO("PakFile", "Found file %s", name);

        if(_fileEntry.size() > 0)
            _fileEntry.back().endOffset = fileEntry.startOffset - 1;

        _fileEntry.push_back(fileEntry);
        if(_stream.peek() == 0x0)
        {
            _fileEntry.back().endOffset = getStreamSize(_stream) - 1;
            break;
        }
    }
}

std::istream *PakFile::getFileStream(std::string fileName)
{
    std::vector<uint8_t> buffer;
    PakFileEntry fileEntry = { 0, 0, "" };
    for(std::vector<PakFileEntry>::iterator it = _fileEntry.begin(); it <= _fileEntry.end(); it++ )
    {
        if(it == _fileEntry.end())
            throw(FileNotFoundException(LOG_ERROR, "PakFile", fileName));

        if((fileEntry = *it).fileName == fileName)
            break;
    }

    buffer.resize(fileEntry.endOffset - fileEntry.startOffset + 1);

    if(buffer.size() == 0)
        throw(NullSizeException(LOG_ERROR, "PakFile", fileName));

    _stream.seekg(fileEntry.startOffset, std::ios::beg);
    _stream.read((char*)&buffer.front(), buffer.size());

    return new std::istringstream(std::string((const char*)&buffer.front(), buffer.size()));
}
// vim:ts=8:sw=4:et
