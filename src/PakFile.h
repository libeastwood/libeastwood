#ifndef	EASTWOOD_PAKFILE_H
#define	EASTWOOD_PAKFILE_H

#include <istream>
#include <vector>

struct PakFileEntry {
    uint32_t startOffset;
    uint32_t endOffset;
    std::string fileName;
};

class PakFile : public std::istream
{
    public:
        PakFile(std::istream &stream);
        virtual ~PakFile();

	void close();
	void open(std::string fileName);

	inline bool is_open() {
            return rdbuf() != NULL;
        }
        inline std::string getFileName(uint32_t index) {
            return _fileEntry[index].fileName;
        };
        inline uint32_t size() {
            return _fileEntry.size();
        };

    private:
        void readIndex();

        std::istream &_stream;
        std::vector<PakFileEntry> _fileEntry;
};

#endif // EASTWOOD_PAKFILE_H
// vim:ts=8:sw=4:et
