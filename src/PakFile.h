#ifndef	EASTWOOD_PAKFILE_H
#define	EASTWOOD_PAKFILE_H

#include <iostream>
#include <fstream>
#include <vector>
#include <boost/shared_ptr.hpp>

struct PakFileEntry {
	size_t StartOffset;
	size_t EndOffset;
	std::string Filename;
};

class PakFile
{
public:
	PakFile(std::string PakFilename);
	~PakFile();

	std::string getFilename(int index);

	unsigned char *getFile(std::string fname, size_t *size);

	inline int getNumFiles() {return NumFileEntry;};

private:

	void readIndex();

	boost::shared_ptr<std::ifstream> fPakFile;
	FILE *File;
	std::string Filename;

	std::vector<PakFileEntry> FileEntry;
	int NumFileEntry;
};

#endif // EASTWOOD_PAKFILE_H
