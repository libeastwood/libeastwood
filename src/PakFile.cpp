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
//    m_stream.close();
//    delete m_stream;
}

void PakFile::readIndex()
{
    char name[256];

    while(1) {
        PakFileEntry fileEntry = { 0, 0, ""};
        _stream.read((char*)&fileEntry.startOffset, sizeof(int));

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
            _fileEntry.back().endOffset = (size_t)_stream.tellg() - 1;
            break;
        }
    }
}

uint8_t *PakFile::getFile(std::string fileName, size_t *size)
{
    PakFileEntry fileEntry = { 0, 0, "" };
    uint8_t *content;
    for(std::vector<PakFileEntry>::iterator it = _fileEntry.begin(); it <= _fileEntry.end(); it++ )
    {
        if(it == _fileEntry.end())
            throw(FileNotFoundException(LOG_ERROR, "PakFile", fileName));

        if((fileEntry = *it).fileName.compare(fileName) == 0)
            break;
    }

    *size = fileEntry.endOffset - fileEntry.startOffset + 1;

    if(*size == 0)
        throw(NullSizeException(LOG_ERROR, "PakFile", fileName));

    if( (content = (uint8_t*) malloc(*size)) == NULL)
        throw(std::bad_alloc());

    _stream.seekg(fileEntry.startOffset, std::ios::beg);

    _stream.read((char*)content, *size);


    return content;	
}

// vim:ts=8:sw=4:et
