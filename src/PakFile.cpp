#include "eastwood/StdDef.h"

#include "eastwood/Exception.h"
#include "eastwood/Log.h"
#include "eastwood/PakFile.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#endif

namespace eastwood {

PakFile::PakFile(std::iostream &stream) :
    _mode(std::ios::in), _currentFile(), _stream(stream), _fileEntries(),
    _fileNames()
{
    readIndex();
}

PakFile::~PakFile()
{
    close();
}

void PakFile::close() {
    if(is_open()) {
        if(_mode & std::ios_base::out) {
            char buf[BUFSIZ];
            memset(buf, 0, BUFSIZ);
            uint32_t size = sizep();
            seekg(0, std::ios::beg);
            if(size != _currentFile->second.second) {
                if(size < _currentFile->second.second)
                    removeBytes(_currentFile->second.first, _currentFile->second.second - size);
                else
                    insertPadding(_currentFile->second.first, size - _currentFile->second.second);
                _currentFile->second.second = size;
            }
            writeIndex(_fileEntries[_fileNames[0]].first);

            _stream.seekg(_currentFile->second.first, std::ios::beg);
            while(size) {
                uint32_t n = (size < BUFSIZ) ? size : BUFSIZ;
                read(buf, n);
                _stream.write(buf, n);
                size -= n;
            }
        }
        delete rdbuf();
        std::ios::init(NULL);
        _mode = std::ios::in;
    }
}

void PakFile::open(std::string fileName, std::ios::openmode mode) {
    close();

    _mode |= mode;
    _currentFile = _fileEntries.find(fileName);
    if(_currentFile != _fileEntries.end()) {
        _stream.seekg(_currentFile->second.first, std::ios::beg);
        if(_mode & std::ios::trunc)
            std::ios::init(new std::stringbuf(_mode));
        else {
            std::string buffer(_currentFile->second.second, 0);
            _stream.read((char*)buffer.data(), buffer.size());
            std::ios::init(new std::stringbuf(buffer, _mode));
        }
    } else if(_mode & std::ios_base::out) {
        _fileNames.push_back(fileName);
        _stream.seekg(0, std::ios::end);
        _fileEntries.insert(make_pair(fileName, FileEntry((uint32_t)_stream.tellg(), 0)));
        _currentFile = _fileEntries.find(fileName);
        std::ios::init(new std::stringbuf(_mode));
    } else
        throw(FileNotFoundException(LOG_ERROR, "PakFile", fileName));

}

bool PakFile::erase(std::string fileName)
{
    uint32_t startOffset = _fileEntries[_fileNames[0]].first;
    _currentFile = _fileEntries.find(fileName);
    if(_currentFile == _fileEntries.end())
        return false;
    removeBytes(_currentFile->second.first, _currentFile->second.second);
    for(std::vector<std::string>::iterator it = _fileNames.begin();
            it != _fileNames.end(); ++it) {
        if(*it == fileName) {
            _fileNames.erase(it);
            break;
        }
    }
    _fileEntries.erase(_currentFile);
    writeIndex(startOffset);

    return true;
}

int32_t PakFile::sizediff()
{
    int32_t actualSize;
    if(!_fileNames.empty()) {
        FileEntry &entry = _fileEntries[_fileNames.back()];
        actualSize = (entry.first + entry.second);
    } else
        actualSize = 0;
    return (actualSize - reinterpret_cast<IStream*>(&_stream)->sizeg());
}


void PakFile::insertPadding(uint32_t offset, uint32_t n, const char padbyte)
{
    char buf[BUFSIZ];
    _stream.seekp(0, std::ios::end);
    std::streampos end = _stream.tellp();
    uint32_t left = static_cast<uint32_t>(end) - offset;

    for(uint32_t i = 0; i < n; i++)
	_stream.put(padbyte);
    uint32_t roffset = static_cast<uint32_t>(end);
    while(left > 0) {
	uint32_t nb = (left < BUFSIZ) ? left : BUFSIZ;
        roffset -= nb;
        _stream.seekg(roffset, std::ios::beg);
	_stream.read(buf, nb);
        _stream.seekp(roffset + n, std::ios::beg);
        _stream.write(buf, nb);
        left -= nb;
    }
    _stream.seekp(offset, std::ios::beg);
    for(uint32_t i = 0; i < n; i++)
	_stream.put(padbyte);    

}

void PakFile::removeBytes(uint32_t offset, uint32_t n)
{
    char buf[BUFSIZ];
    _stream.seekp(0, std::ios::end);
    std::streampos end = _stream.tellp();
    uint32_t left = (static_cast<uint32_t>(end) - (offset + n));

    while(left) {
        uint32_t nb = (left < BUFSIZ) ? left : BUFSIZ;

        _stream.seekg((offset + n), std::ios::beg);
        _stream.read(buf, nb);
        _stream.seekp(offset, std::ios::beg);
        _stream.write(buf, nb);
        left -= nb;
        offset += nb;
    }
    while(_stream.tellg() != end)
        _stream.put(0);

}

void PakFile::readIndex()
{
    IStream &stream = *reinterpret_cast<IStream*>(&_stream);
    char name[256];
    uint32_t offset = stream.getU32LE();

    while(stream.peek()) {
        uint32_t start = offset,
                 size;

	stream.getline(name, 256, 0);
        LOG_INFO("PakFile", "Found file %s", name);

        size = ((stream.peek() != 0) ? (offset = stream.getU32LE()) : stream.sizeg()) - start;

        _fileEntries.insert(make_pair(name, FileEntry(start, size)));
        _fileNames.push_back(name);
    }
}

void PakFile::writeIndex(uint32_t firstOffset)
{
    OStream &stream = *reinterpret_cast<OStream*>(&_stream);

    uint32_t offset = sizeof(uint32_t);
    for(std::vector<std::string>::const_iterator it = _fileNames.begin();
            it != _fileNames.end(); ++it) {
        offset += sizeof(uint32_t) + it->size() + 1;
    }
    int32_t move = offset - firstOffset;
    if(move) {
        if(move < 0)
            removeBytes(offset, std::abs(move));
        else
            insertPadding(offset, move);
    }
    _stream.seekp(0, std::ios::beg);
    uint32_t size = 0;
    for(std::vector<std::string>::const_iterator it = _fileNames.begin();
            it != _fileNames.end(); ++it) {
        FileEntry &entry = _fileEntries[*it];
        
        entry.first = (offset += size);
        size = entry.second;

        stream.putU32LE(entry.first);
        stream.write(it->c_str(), it->size());
        stream.put(0);
    }
    stream.putU32LE(0);
}

bool truncateFile(const char *fileName, uint32_t size)
{
#ifdef _WIN32 // Not tested...
    HANDLE filehand;
    uint32_t filesize;
    uint32_t result;
    WIN32_FILE_ATTRIBUTE_DATA filedata;

    /* Learn the file's length */
    if (!GetFileAttributesEx( filename, GetFileExInfoStandard, &filedata )) return false;
    filesize = (filedata.nFileSizeHigh << 32) +filedata.nFileSizeLow;


    /* Calculate the new length */
    filesize -= bytes;
    filedata.nFileSizeHigh = filesize >> 32;
    filedata.nFileSizeLow  = filesize & 0xFFFFFFFFL;

    /* Modify the file's length */
    filehand = CreateFile(
            filename,
            GENERIC_READ|GENERIC_WRITE,
            0,
            NULL,
            OPEN_EXISTING,
            FILE_FLAG_RANDOM_ACCESS,
            NULL
            );
    if (filehand == INVALID_HANDLE_VALUE) return false;

    result = SetFilePointer(
            filehand,
            filedata.nFileSizeLow,
            &filedata.nFileSizeHigh,
            FILE_BEGIN
            ) != 0xFFFFFFFF;

    if (result) result = SetEndOfFile( filehand );

    CloseHandle( filehand );
    return result;
#else
    struct stat fileData;
    uint32_t fileSize;
    int fd;
    bool ret = false;
    if (stat(fileName, &fileData)) return ret;
    fileSize = fileData.st_size - size;
    fd = open(fileName, O_RDWR);
    ret = truncate(fileName, fileSize) == 0;
    close(fd);
    return ret;
#endif
}

}
// vim:ts=8:sw=4:et
