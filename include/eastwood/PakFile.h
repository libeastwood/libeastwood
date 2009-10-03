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

	off_t close();
	void open(std::string fileName, std::ios::openmode mode = std::ios_base::in | std::ios_base::binary);

	bool is_open() const throw() {
            return rdbuf() != NULL;
        }
        std::string getFileName(uint32_t index) const {
            return _fileNames.at(index);
        };
        uint32_t entries() const throw() {
            return _fileEntries.size();
        };

	std::streamsize size() { return IStream::size(); }

    private:
        void readIndex();
        void writeIndex();
	void insertPadding(off_t offset, uint32_t n, const char padbyte = 0);        
        off_t removeBytes(off_t offset, uint32_t n);

        bool _newFile;
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
