#ifndef EASTWOOD_EXEFILE_H
#define EASTWOOD_EXEFILE_H

#include <vector>

#include "IStream.h"

namespace eastwood {

struct ExeHeader {
        uint16_t signature;	// EXE_Header Signature MZ or ZM
        uint16_t extraBytes;	// Bytes on the last page
        uint16_t pages;		// Pages in file
        uint16_t relocations;	// Relocations in file
        uint16_t headerSize;	// Paragraphs in header
        uint16_t minMemory;	// Minimum amount of memory
        uint16_t maxMemory;	// Maximum amount of memory
        uint16_t initSS;
        uint16_t initSP;
        uint16_t checksum;
        uint16_t initIP;
        uint16_t initCS;
        uint16_t relocTable;
        uint16_t overlay;
} __attribute__ ((packed));

class ExeFile : public IStream
{
    public:
	ExeFile(std::istream &stream);
	
	IStream& seekSegOff(uint32_t segoff);
	IStream& seekSegOff(uint16_t seg, uint16_t off);

    private:
	void readHeader();
	ExeHeader _header;
    	std::vector<uint32_t> _relocations;
	uint32_t _codeSegment;
};

}
#endif // EASTWOOD_EXEFILE_H
