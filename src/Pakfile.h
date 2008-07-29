#ifndef	PAKFILE_H_INCLUDED
#define	PAKFILE_H_INCLUDED

#include <stdio.h>

struct PakFileEntry {
	long StartOffset;
	long EndOffset;
	char* Filename; // dunk - better to be const char* ? ( or even std::string ? )
};

class Pakfile
{
public:
	Pakfile(const char * Pakfilename);
	~Pakfile();

	char * getFilename(int index);

	unsigned char *getFile(const char *fname, int *size);

	inline int getNumFiles() {return NumFileEntry;};

private:

	void readIndex();

	FILE * fPakFile;
	char * Filename;

	PakFileEntry *FileEntry;
	int	NumFileEntry;
};

#endif // PAKFILE_H_INCLUDED
