#ifndef _WIN32
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#endif

#include "eastwood/StdDef.h"

#include "eastwood/Exception.h"
#include "eastwood/Log.h"
#include "eastwood/PakFile.h"

namespace eastwood {

PakFile::PakFile(std::iostream &stream) :
    _mode(std::ios::in), _currentFile(), _stream(reinterpret_cast<IOStream&>(stream)), _fileEntries(),
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
            uint32_t size = sizep();
            char buf[BUFSIZ];
            memset(buf, 0, BUFSIZ);
            seekg(0, std::ios::beg);
            writeIndex(_fileEntries[_fileNames[0]].first);
            if(size != _currentFile->second.second) {
                if(size < _currentFile->second.second)
                    removeBytes(_currentFile->second.first, _currentFile->second.second - size);
                else
                    insertPadding(_currentFile->second.first, size - _currentFile->second.second);
                _currentFile->second.second = size;
            }

            _stream.seekp(_currentFile->second.first, std::ios::beg);
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

// This function will check if the string is a valid 8.3 filename
// See http://en.wikipedia.org/wiki/8.3_filename
static inline bool validateFileName(std::string &fileName)
{
    char legalNonAlphaNum[] = { '!', '#', '$', '%', '&', '\'', '(', ')', '-',
       '@', '^', '_', '`', '{', '}', '~', ' ' };
    bool delimFound = false;
    size_t delim = fileName.find_first_of(".");
    if((fileName.size() > 8 && delim > 8) ||
            (++delim && fileName.substr(delim).size() > 3))
        return false;
    for(std::string::iterator it = fileName.begin();
            it != fileName.end(); ++it) {
        if(*it == '.') {
            if(delimFound)
                return false;
            delimFound = true;
        } else if(!isalnum(*it)) {
            bool legalChar = false;
            for(uint8_t i = 0; i < sizeof(legalNonAlphaNum) && !legalChar; i++)
                if(legalNonAlphaNum[i] == *it)
                    legalChar = true;
            if(!legalChar)
                return false;
        }
    }
    return true;
}

void PakFile::open(std::string fileName, std::ios::openmode mode)
{
    close();

    if(!validateFileName(fileName))
        throw(FileException(LOG_ERROR, "PakFile", fileName, "Filename must be DOS-style (8.3 format)"));

    std::transform( fileName.begin(), fileName.end(), fileName.begin(), ::toupper );

    _mode |= mode;
    _currentFile = _fileEntries.find(fileName);
    if(_currentFile != _fileEntries.end()) {
        _stream.seekg(_currentFile->second.first, std::ios::beg);
        if(_mode & std::ios::trunc)
            std::ios::init(new std::stringbuf(_mode));
        else {
            std::string buffer(_currentFile->second.second, 0);
            _stream.read(const_cast<char*>(reinterpret_cast<const char*>(buffer.data())), buffer.size());
            std::ios::init(new std::stringbuf(buffer, _mode));
        }
    } else if(_mode & std::ios_base::out) {
        _fileNames.push_back(fileName);
        _stream.seekg(0, std::ios::end);
        _fileEntries.insert(std::make_pair(fileName, FileEntry(static_cast<uint32_t>(_stream.tellg()), 0)));
        _currentFile = _fileEntries.find(fileName);
        std::ios::init(new std::stringbuf(_mode));
    } else
        throw(FileNotFoundException(LOG_ERROR, "PakFile", fileName));

}

bool PakFile::erase(std::string fileName)
{
    uint32_t startOffset = _fileEntries[_fileNames[0]].first;

    std::transform( fileName.begin(), fileName.end(), fileName.begin(), ::toupper );

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
    return (actualSize - _stream.sizeg());
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
    if(!_stream.sizeg())
        return;
    char name[256];
    uint32_t offset = _stream.getU32LE();

    while(offset) {
        uint32_t start = offset,
                 size;

	_stream.getline(name, 256, 0);
        LOG_INFO("PakFile", "Found file %s", name);

        size = ((offset = _stream.getU32LE()) != 0 ? offset : _stream.sizeg()) - start;
        _fileEntries.insert(make_pair(name, FileEntry(start, size)));
        _fileNames.push_back(name);
    }
}

void PakFile::writeIndex(uint32_t firstOffset)
{
    int32_t move;
    uint32_t offset = sizeof(uint32_t);
    uint32_t size = 0;
    for(std::vector<std::string>::const_iterator it = _fileNames.begin();
            it != _fileNames.end(); ++it) {
        offset += sizeof(uint32_t) + it->size() + 1;
    }

    if(_stream.sizep() && (move = offset - firstOffset)) {
        if(move < 0)
            removeBytes(offset, std::abs(move));
        else
            insertPadding(offset, move);
    }

    _stream.seekp(0, std::ios::beg);
    for(std::vector<std::string>::const_iterator it = _fileNames.begin();
            it != _fileNames.end(); ++it) {
        FileEntry &entry = _fileEntries[*it];
        
        entry.first = (offset += size);
        size = entry.second;

        _stream.putU32LE(entry.first);
        _stream.write(it->c_str(), it->size());
        _stream.put(0);
    }
    _stream.putU32LE(0);
}

bool truncateFile(const char *fileName, uint32_t size)
{
#ifdef _WIN32 // Not tested...
    HANDLE filehand;
    uint32_t result;

    /* Modify the file's length */
    filehand = CreateFile(
            fileName,
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
            size,
            NULL,
            FILE_END
            ) != INVALID_SET_FILE_POINTER;

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
