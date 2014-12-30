#ifndef	EASTWOOD_PAKFILE_H
#define	EASTWOOD_PAKFILE_H

#include <map>
#include <vector>

#include "eastwood/IOStream.h"

namespace eastwood {

typedef std::pair<uint32_t, uint32_t> FileEntry;

class PakFile : public IOStream
{
    public:
        PakFile(std::iostream &stream);
        ~PakFile();

	void close();
	void open(std::string fileName, std::ios::openmode mode = std::ios_base::in | std::ios_base::binary);
        bool erase(std::string fileName);

	bool is_open() const noexcept {
            return rdbuf() != NULL;
        }

        bool exists(std::string fileName) const noexcept {
            std::transform( fileName.begin(), fileName.end(), fileName.begin(), ::toupper );
            return _fileEntries.find(fileName) != _fileEntries.end();
        }

        std::string getFileName(uint32_t index) const {
            return _fileNames.at(index);
        }

        uint32_t entries() const noexcept {
            return _fileEntries.size();
        }
        // If negative, file needs to be truncated
        int32_t sizediff();

    private:
        void readIndex();
        void writeIndex(uint32_t firstOffset);
	void insertPadding(uint32_t offset, uint32_t n, const char padbyte = 0);        
        void removeBytes(uint32_t offset, uint32_t n);

        std::ios_base::openmode _mode;
        std::map<std::string, FileEntry>::iterator _currentFile;
        IOStream &_stream;
        std::map<std::string, FileEntry> _fileEntries;
        std::vector<std::string> _fileNames;
};

bool truncateFile(const char *fileName, uint32_t size);

}
#endif // EASTWOOD_PAKFILE_H
// vim:ts=8:sw=4:et
