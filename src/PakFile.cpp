#include <SDL_endian.h>

#include "Exception.h"
#include "Log.h"
#include "PakFile.h"

using namespace eastwood;

PakFile::PakFile(std::string pakFileName)
{
    m_fileName = pakFileName;
    m_pakFile = new std::ifstream(m_fileName.c_str());
    m_pakFile->exceptions ( std::ifstream::eofbit | std::ifstream::failbit | std::ifstream::badbit );

    readIndex();
}

PakFile::~PakFile()
{
    m_pakFile->close();
    delete m_pakFile;
}

void PakFile::readIndex()
{
    char name[256];

    while(1) {
        PakFileEntry fileEntry = { 0, 0, ""};
        m_pakFile->read((char*)&fileEntry.startOffset, sizeof(int));

        // pak-files are always little endian encoded
        fileEntry.startOffset = SDL_SwapLE32(fileEntry.startOffset);

        m_pakFile->getline(name, 256, 0);
        fileEntry.fileName = name;

        LOG_INFO("PakFile", "Found file %s", name);

        if(m_fileEntry.size() > 0)
            m_fileEntry.back().endOffset = fileEntry.startOffset - 1;

        m_fileEntry.push_back(fileEntry);
        if(m_pakFile->peek() == 0x0)
        {
            m_pakFile->seekg(0, std::ios::end);
            m_fileEntry.back().endOffset = (size_t)m_pakFile->tellg() - 1;
            break;
        }
    }
}

unsigned char *PakFile::getFile(std::string fileName, size_t *size)
{
    PakFileEntry fileEntry;
    unsigned char *content;
    for(std::vector<PakFileEntry>::iterator it = m_fileEntry.begin(); it <= m_fileEntry.end(); it++ )
    {
        if(it == m_fileEntry.end())
            throw(FileNotFoundException(LOG_ERROR, "PakFile", fileName));

        if((fileEntry = *it).fileName.compare(fileName) == 0)
            break;
    }

    *size = fileEntry.endOffset - fileEntry.startOffset + 1;

    if(*size == 0)
        throw(NullSizeException(LOG_ERROR, "PakFile", fileName));

    if( (content = (unsigned char*) malloc(*size)) == NULL)
        throw(std::bad_alloc());

    m_pakFile->seekg(fileEntry.startOffset, std::ios::beg);

    m_pakFile->read((char*)content, *size);


    return content;	
}

// vim:ts=8:sw=4:et
