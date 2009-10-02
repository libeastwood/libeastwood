#ifndef	EASTWOOD_PAKFILE_H
#define	EASTWOOD_PAKFILE_H

#include <map>
#include <vector>

#include "eastwood/IStream.h"

namespace eastwood {

typedef std::pair<uint32_t, uint32_t> FileEntry;

class PakFile : public IStream
{
    public:
        PakFile(const std::istream &stream);
        virtual ~PakFile();

	void close();
	void open(std::string fileName);

	bool is_open() const throw() {
            return rdbuf() != NULL;
        }
        std::string getFileName(uint32_t index) const {
            return _fileNames.at(index);
        };
        uint32_t entries() const throw() {
            return _fileEntries.size();
        };

    private:
        void readIndex();

        IStream &_stream;
        std::map<std::string, FileEntry> _fileEntries;
        std::vector<std::string> _fileNames;
};

}
#endif // EASTWOOD_PAKFILE_H
// vim:ts=8:sw=4:et
