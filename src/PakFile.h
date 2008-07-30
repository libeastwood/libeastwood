#ifndef	PAKFILE_H
#define	PAKFILE_H

#include <stdio.h>

struct PakFileEntry {
	long StartOffset;
	long EndOffset;
	char *Filename; // dunk - better to be const char* ? ( or even std::string ? )
};

class PakFile
{
public:
	PakFile(const char * PakFilename);
	~PakFile();

	char *getFilename(int index);

	unsigned char *getFile(const char *fname, int *size);

	inline int getNumFiles() {return NumFileEntry;};

private:

	void readIndex();

	FILE *fPakFile;
	char *Filename;

	PakFileEntry *FileEntry;
	int	NumFileEntry;
};

#endif // PAKFILE_H
