#include <SDL_endian.h>
#include <new>

#include "EastwoodException.h"
#include "Log.h"
#include "PakFile.h"

PakFile::PakFile(std::string PakFilename)
{
    Filename = PakFilename;
    fPakFile = new std::ifstream(Filename.c_str());
    fPakFile->exceptions ( std::ifstream::eofbit | std::ifstream::failbit | std::ifstream::badbit );

    readIndex();
}

PakFile::~PakFile()
{
    fPakFile->close();
    delete fPakFile;
}

void PakFile::readIndex()
{
    char name[256];

    while(1) {
        PakFileEntry fileEntry = { 0, 0, ""};
        fPakFile->read((char*)&fileEntry.StartOffset, sizeof(int));

        // pak-files are always little endian encoded
        fileEntry.StartOffset = SDL_SwapLE32(fileEntry.StartOffset);

        fPakFile->getline(name, 256, 0);
        fileEntry.Filename = name;

        LOG_INFO("PakFile", "Found file %s", name);

        if(FileEntry.size() > 0)
            FileEntry.back().EndOffset = fileEntry.StartOffset - 1;

        FileEntry.push_back(fileEntry);
        if(fPakFile->peek() == 0x0)
        {
            fPakFile->seekg(0, std::ios::end);
            FileEntry.back().EndOffset = (size_t)fPakFile->tellg() - 1;
            break;
        }
    }
}

unsigned char *PakFile::getFile(std::string fname, size_t *size)
{
    PakFileEntry fileEntry;
    unsigned char *content;
    size_t fileSize;
    for(std::vector<PakFileEntry>::iterator it = FileEntry.begin(); it <= FileEntry.end(); it++ )
    {
        if(it == FileEntry.end())
            throw(FileNotFoundException(fname));

        if((fileEntry = *it).Filename.compare(fname) == 0)
            break;
    }

    fileSize = fileEntry.EndOffset - fileEntry.StartOffset + 1;

    if(fileSize == 0)
        throw(NullSizeException(fname));

    if( (content = (unsigned char*) malloc(fileSize)) == NULL)
        throw(std::bad_alloc());

    fPakFile->seekg(fileEntry.StartOffset, std::ios::beg);

    fPakFile->read((char*)content, fileSize);

    if(size != NULL)
        *size = fileSize;

    return content;	
}

// vim:ts=8:sw=4:et
