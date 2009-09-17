#ifndef	EASTWOOD_PAKFILE_H
#define	EASTWOOD_PAKFILE_H

#include <vector>

#include "IStream.h"

namespace eastwood {

struct PakFileEntry {
    uint32_t startOffset;
    uint32_t endOffset;
    std::string fileName;
};

class PakFile : public IStream
{
    public:
        PakFile(const std::istream &stream);
        ~PakFile();

	void close();
	void open(std::string fileName);

	inline bool is_open() {
            return rdbuf() != NULL;
        }
        inline std::string getFileName(uint32_t index) {
            return _fileEntry[index].fileName;
        };
        inline uint32_t entries() {
            return _fileEntry.size();
        };

    private:
        void readIndex();

        IStream &_stream;
        std::vector<PakFileEntry> _fileEntry;
};

}
#endif // EASTWOOD_PAKFILE_H
// vim:ts=8:sw=4:et
