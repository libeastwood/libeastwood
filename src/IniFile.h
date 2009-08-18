#ifndef EASTWOOD_INIFILE_H
#define EASTWOOD_INIFILE_H

#include <string>
#include <SDL_rwops.h>
#include <SDL.h>

//!  A class for reading and writing *.ini configuration files. 
/*!
	This class can be used to read or write to a *.ini file. An ini-File has a very simple format.<br>
	Example:<br>
		<br>
		; Comments start with ; or #<br>
		; start of the first section with name ""<br>
		key1 = value1<br>
		key2 = value2<br>
		; start of a section with name "Section1"<br>
		[Section1]<br>
		key3 = value3<br>
		key4 = value4<br>
*/
class IniFile
{
private:
	//\cond
	class CommentEntry;
	class SectionEntry;
	class KeyEntry;

	class CommentEntry
	{
	public:
		CommentEntry(std::string completeLine) {
			CompleteLine = completeLine;
			nextEntry = NULL;
			prevEntry = NULL;
		};

		std::string CompleteLine;
		CommentEntry *nextEntry;
		CommentEntry *prevEntry;
	};


	class SectionEntry : public CommentEntry
	{
	public:
		SectionEntry(std::string completeLine, int sectionstringbegin, int sectionstringlength)
		: CommentEntry(completeLine) {
			nextSection = NULL;
			prevSection = NULL;
			KeyRoot = NULL;
			SectionStringBegin = sectionstringbegin;
			SectionStringLength = sectionstringlength;
		};

		int SectionStringBegin;
		int SectionStringLength;
		SectionEntry *nextSection;
		SectionEntry *prevSection;
		KeyEntry *KeyRoot;
	};


	class KeyEntry : public CommentEntry
	{
	public:
		KeyEntry(std::string completeLine, int keystringbegin, int keystringlength, int valuestringbegin, int valuestringlength)
		: CommentEntry(completeLine) {
			nextKey = NULL;
			prevKey = NULL;
			KeyStringBegin = keystringbegin;
			KeyStringLength = keystringlength;
			ValueStringBegin = valuestringbegin;
			ValueStringLength = valuestringlength;
		};
	
		int KeyStringBegin;
		int KeyStringLength;
		int ValueStringBegin;
		int ValueStringLength;
		KeyEntry *nextKey;
		KeyEntry *prevKey;
	};
	//\endcond 

public:
	typedef IniFile::KeyEntry* KeyListHandle;		///< A handle to a KeyList opened with KeyList_Open().

	IniFile(unsigned char *bufFiledata, int bufsize);
	IniFile(SDL_RWops *RWopsFile);
	~IniFile();
	
	std::string getStringValue(std::string section, std::string key, std::string defaultValue = "");
	int getIntValue(std::string section, std::string key, int defaultValue = 0);
	bool getBoolValue(std::string section, std::string key, bool defaultValue = false);
	
	void setStringValue(std::string section, std::string key, std::string value);
	void setIntValue(std::string section, std::string key, int value);
	void setBoolValue(std::string section, std::string key, bool value);
	
	KeyListHandle KeyList_Open(std::string sectionname);
	bool KeyList_EOF(KeyListHandle handle);
	std::string KeyList_GetNextKey(KeyListHandle *handle);
	void KeyList_Close(KeyListHandle *handle);
	
	bool SaveChangesTo(std::string filename);
	bool SaveChangesTo(SDL_RWops *file);
	
	
private:
	CommentEntry *FirstLine;
	SectionEntry *SectionRoot;
	
	void flush();
	void readfile(SDL_RWops *file);
	
	void InsertSection(SectionEntry *newSection);
	void InsertKey(SectionEntry *section, KeyEntry *newKeyEntry);

	SectionEntry *getSection(std::string sectionname);
	KeyEntry *getKey(SectionEntry* sectionentry, std::string keyname);
		
	int getNextChar(const unsigned char *line, int startpos);
	int skipName(const unsigned char *line, int startpos);
	int skipValue(const unsigned char *line, int startpos);
	int skipKey(const unsigned char *line, int startpos);
	int getNextQuote(const unsigned char *line, int startpos);
	
	bool isWhitespace(unsigned char s);
	bool isNormalChar(unsigned char s);
};

#endif // EASTWOOD_INIFILE_H
