#ifndef	EASTWOOD_PAKFILE_H
#define	EASTWOOD_PAKFILE_H

#include <map>
#include <vector>

#include <iostream>
#include "eastwood/IStream.h"
#include "eastwood/OStream.h"

namespace eastwood {

typedef std::pair<uint32_t, uint32_t> FileEntry;

class PakFile : public IStream, public OStream
{
    public:
        PakFile(std::iostream &stream);
        virtual ~PakFile();

	void close();
	void open(std::string fileName, std::ios::openmode mode = std::ios_base::in | std::ios_base::binary);
        bool erase(std::string fileName);

	bool is_open() const throw() {
            return rdbuf() != NULL;
        }
        std::string getFileName(uint32_t index) const {
            return _fileNames.at(index);
        };
        uint32_t entries() const throw() {
            return _fileEntries.size();
        };
        // If negative, file needs to be truncated
        int32_t sizediff() {
            FileEntry &entry = _fileEntries[_fileNames.back()];
            std::cout << "hm: " << entry.first + entry.second << "  size: " << reinterpret_cast<IStream*>(&_stream)->size() << std::endl;
            int32_t diff = (entry.first + entry.second) - reinterpret_cast<IStream*>(&_stream)->size();
            return diff;
        }

	std::streamsize size() { return IStream::size(); }

    private:
        void readIndex();
        void writeIndex();
	void insertPadding(off_t offset, uint32_t n, const char padbyte = 0);        
        void removeBytes(off_t offset, uint32_t n);

        std::ios_base::openmode _mode;
        std::map<std::string, FileEntry>::iterator _currentFile;
        std::iostream &_stream;
        std::map<std::string, FileEntry> _fileEntries;
        std::vector<std::string> _fileNames;
};

bool truncateFile(const char *fileName, off_t size);

}
#endif // EASTWOOD_PAKFILE_H
// vim:ts=8:sw=4:et
