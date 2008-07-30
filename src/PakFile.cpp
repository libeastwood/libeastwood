#include "SDL_endian.h"
#include <stdlib.h>
#include <string>


#include "Log.h"
#include "PakFile.h"

PakFile::PakFile(const char *PakFilename)
{
	FileEntry = NULL;
	NumFileEntry = 0;
	
	if(PakFilename == NULL) {
		LOG_ERROR("PakFile", "PakFile::PakFile(): PakFilename == NULL\n");
		exit(EXIT_FAILURE);
	}
	
	if((Filename = (char*) malloc(strlen(PakFilename)+1)) == NULL) {
		LOG_ERROR("PakFile", "PakFile::PakFile()");
		exit(EXIT_FAILURE);
	}
	
	strcpy(Filename,PakFilename);
	
	if( (fPakFile = fopen(Filename, "rb")) == NULL) {
		LOG_ERROR("PakFile", "PakFile::PakFile()");
		exit(EXIT_FAILURE);	
	}
	
	readIndex();	
}

PakFile::~PakFile()
{
	if(fPakFile != NULL) {
		fclose(fPakFile);
	}
	
	for(int i=0;i<NumFileEntry;i++) {
		free(FileEntry[i].Filename);
	}
	
	free(FileEntry);
}

void PakFile::readIndex()
{
	int i;
	int startoffset;
	char name[256];
	
	while(1) {
		if(fread((void*) &startoffset, 4, 1,fPakFile) != 1) {
			perror("fread()");
			exit(EXIT_FAILURE);
		}
		
		//pak-files are always little endian encoded
		startoffset = SDL_SwapLE32(startoffset);

		
		if(startoffset == 0) {
			break;
		}
		
		if((FileEntry = (PakFileEntry*) realloc(FileEntry,(NumFileEntry+1) * sizeof(PakFileEntry))) == NULL) {
			perror("realloc()");
			exit(EXIT_FAILURE);			
		}
		
		FileEntry[NumFileEntry].StartOffset = startoffset;
		
		i = 0;
		while(1) {
			if(fread(&name[i],1,1,fPakFile) != 1) {
				perror("fread()");
				exit(EXIT_FAILURE);				
			}
			
			i++;
			
			if(name[i-1] == '\0') {
				break;
			}
			
			if(i >= 256) {
				LOG_ERROR("PakFile", "readIndex(): Filename in PakFile too long");
				exit(EXIT_FAILURE);
			}
		}
			
		if((FileEntry[NumFileEntry].Filename = (char *) malloc(i)) == NULL) {
				perror("malloc()");
				exit(EXIT_FAILURE);		
		}
		
		strcpy(FileEntry[NumFileEntry].Filename,name);
        LOG_INFO("PakFile", "Found file %s", name);
		
		if(NumFileEntry > 0) {
			FileEntry[NumFileEntry - 1].EndOffset = startoffset - 1;
		}
		
		NumFileEntry++;
	}
	
	if(fseek(fPakFile,0,SEEK_END) != 0) {
		perror("fseek()");
		exit(EXIT_FAILURE);
	}

	if((FileEntry[NumFileEntry-1].EndOffset = (ftell(fPakFile) - 1)) < 0) {
		perror("ftell()");
		exit(EXIT_FAILURE);		
	}
}

char * PakFile::getFilename(int index) {
	if((index >= NumFileEntry) || (index < 0))
		return NULL;
	
	return FileEntry[index].Filename;
}

unsigned char *PakFile::getFile(const char *fname, int *size)
{
	int Index = -1;
	
	for(int i=0;i<NumFileEntry;i++) {
		if(strcmp(FileEntry[i].Filename,fname) == 0) {
			Index = i;
			break;
		}
	}
	
	if(Index == -1) {
		return NULL;
	}
	
	int filesize = FileEntry[Index].EndOffset - FileEntry[Index].StartOffset + 1;
	
	if(filesize == 0) {
		return NULL;
	}
	
	unsigned char * content;
	
	if( (content = (unsigned char*) malloc(filesize)) == NULL) {
		return NULL;
	}
	
	if(fseek(fPakFile,FileEntry[Index].StartOffset,SEEK_SET) != 0) {
		return NULL;
	}
	
	if(fread(content,filesize,1,fPakFile) != 1) {
		return NULL;
	}
	
	if(size != NULL) {
		*size = filesize;
	}
	
	return content;	
}
