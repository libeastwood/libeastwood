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
        PakFile(std::string pakFileName);
        ~PakFile();

        unsigned char *getFile(std::string fileName, size_t *size);

        inline std::string getFileName(unsigned int index) {
            return m_fileEntry[index].fileName;
        };

        inline unsigned int getNumFiles() {
            return m_fileEntry.size();
        };

    private:
        void readIndex();

        std::ifstream *m_pakFile;

        std::string m_fileName;

        std::vector<PakFileEntry> m_fileEntry;
};

#endif // EASTWOOD_PAKFILE_H
// vim:ts=8:sw=4:et
