#include <SDL_endian.h>

#include "Log.h"
#include "PakFile.h"

PakFile::PakFile(std::string PakFilename)
{
	Filename = PakFilename;
	fPakFile.reset(new std::ifstream(Filename.c_str()));
	
	readIndex();
}

PakFile::~PakFile()
{
	fPakFile->close();
}

void PakFile::readIndex()
{
	char name[256];

	while(1) {
		PakFileEntry fileEntry;
		fPakFile->read((char*)&fileEntry.StartOffset, sizeof(int));

		// pak-files are always little endian encoded
		fileEntry.StartOffset = SDL_SwapLE32(fileEntry.StartOffset);
		
		fPakFile->getline(name, 256, 0);
		fileEntry.Filename = name;
			
		LOG_INFO("PakFile", "Found file %s", name);
		
		if(FileEntry.size() > 0)
			FileEntry.back().EndOffset = fileEntry.StartOffset - 1;

		FileEntry.push_back(fileEntry);
		if(fPakFile->peek() == 0x0)
		{
			fPakFile->seekg(0, std::ios::end);
			FileEntry.back().EndOffset = (size_t)fPakFile->tellg() - 1;
			break;
		}
	}
}

std::string PakFile::getFilename(unsigned int index) {
	if((index >= FileEntry.size()))
		return NULL;
	
	return FileEntry[index].Filename;
}

unsigned char *PakFile::getFile(std::string fname, size_t *size)
{
	PakFileEntry fileEntry;
	unsigned char *content;
	size_t fileSize;
	for(std::vector<PakFileEntry>::iterator it = FileEntry.begin(); it < FileEntry.end(); it++ )
	{
		if((fileEntry = *it).Filename.compare(fname) == 0)
			break;
		else if(fileEntry.EndOffset == FileEntry.back().EndOffset)
			return NULL;
	}

	fileSize = fileEntry.EndOffset - fileEntry.StartOffset + 1;
	
	if(fileSize == 0)
		return NULL;
	
	if( (content = (unsigned char*) malloc(fileSize)) == NULL)
		return NULL;
	
	fPakFile->seekg(fileEntry.StartOffset, std::ios::beg);
	if(fPakFile->fail())
		return NULL;

	fPakFile->read((char*)content, fileSize);
	if(fPakFile->fail())
		return NULL;
	
	if(size != NULL)
		*size = fileSize;
	
	return content;	
}
