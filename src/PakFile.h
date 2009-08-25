#ifndef	EASTWOOD_PAKFILE_H
#define	EASTWOOD_PAKFILE_H

#include <iostream>
#include <fstream>
#include <vector>

struct PakFileEntry {
    size_t startOffset;
    size_t endOffset;
    std::string fileName;
};

class PakFile
{
    public:
        PakFile(std::istream &stream);
        ~PakFile();

        uint8_t *getFile(std::string fileName, size_t *size);

        inline std::string getFileName(uint32_t index) {
            return _fileEntry[index].fileName;
        };

        inline uint32_t getNumFiles() {
            return _fileEntry.size();
        };

    private:
        void readIndex();

        std::istream &_stream;
        std::vector<PakFileEntry> _fileEntry;
};

#endif // EASTWOOD_PAKFILE_H
// vim:ts=8:sw=4:et
