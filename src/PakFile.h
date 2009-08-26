#ifndef	EASTWOOD_PAKFILE_H
#define	EASTWOOD_PAKFILE_H

#include <iostream>
#include <fstream>
#include <vector>

struct PakFileEntry {
    uint32_t startOffset;
    uint32_t endOffset;
    std::string fileName;
};

class PakFile
{
    public:
        PakFile(std::istream &stream);
        ~PakFile();

        std::istream *getFileStream(std::string fileName);


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
