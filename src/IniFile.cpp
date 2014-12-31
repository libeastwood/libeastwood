#include <iostream>

#include "eastwood/StdDef.h"

#include "eastwood/IniFile.h"
#include "eastwood/Exception.h"

namespace eastwood {

// public methods

/// Constructor for reading the INI-File from a file.
/**
	This constructor reads the INI-File from the file specified by filename. The file opened in readonly-mode. After
	reading the file is closed immediately. If the file does not exist, it is treated as empty.
	\param	filename	The file to be opened.
*/
IniFile::IniFile(std::istream &stream) :
    _stream(reinterpret_cast<IStream&>(stream))
{
	FirstLine = nullptr;
	SectionRoot = nullptr;
	
	readFile();
}


///	Destructor.
/**
	This is the destructor. Changes to the INI-Files are not automaticly saved. Call IniFile::SaveChangesTo() for that purpose.
*/
IniFile::~IniFile()
{
	CommentEntry *curEntry = FirstLine;
	CommentEntry *tmp;
	while(curEntry != nullptr) {
		tmp = curEntry;
		curEntry = curEntry->nextEntry;
		delete tmp;
	}

	// now we have to delete the "" section
	delete SectionRoot;
}


/// Reads the std::string that is adressed by the section/key pair.
/**
	Returns the value that is adressed by the section/key pair as a string. If the key could not be found in
	this section defaultValue is returned. If no defaultValue is specified then "" is returned.
	\param	section			sectionname
	\param	key				keyname
	\param	defaultValue	default value for defaultValue is ""
	\return	The read value or default
*/
std::string IniFile::getStringValue(std::string section, std::string key, std::string defaultValue)
{
	SectionEntry *curSection = getSection(section);
	if(curSection == nullptr) {
		return defaultValue;
	}
	
	KeyEntry *curKey = getKey(curSection,key);
	if(curKey == nullptr) {
		return defaultValue;
	}
	
	return curKey->CompleteLine.substr(curKey->ValueStringBegin,curKey->ValueStringLength);
}


/// Reads the int that is adressed by the section/key pair.
/**
	Returns the value that is adressed by the section/key pair as a int. If the key could not be found in
	this section defaultValue is returned. If no defaultValue is specified then 0 is returned. If the value
	could not be converted to an int 0 is returned.
	\param	section			sectionname
	\param	key				keyname
	\param	defaultValue	default value for defaultValue is 0
	\return	The read number, defaultValue or 0
*/
int IniFile::getIntValue(std::string section, std::string key, int defaultValue )
{
	std::string value = getStringValue(section,key,"");
	if(value.size() == 0) {
		return defaultValue;
	}
	
	long ret;
	if(value.at(0) == '-') {
		ret = -(atol(value.c_str()+1));
	} else if (value.at(0) == '+') {
		ret = atol(value.c_str()+1);	
	} else {
		ret = atol(value.c_str());
	}
	
	return ret;
}

/// Reads the boolean that is adressed by the section/key pair.
/**
	Returns the value that is adressed by the section/key pair as a boolean. If the key could not be found in
	this section defaultValue is returned. If no defaultValue is specified then false is returned. If the value
	is one of "true", "enabled", "on" or "1" then true is returned; if it is one of "false", "disabled", "off" or
	"0" than false is returned; otherwise defaultValue is returned.
	\param	section			sectionname
	\param	key				keyname
	\param	defaultValue	default value for defaultValue is 0
	\return	true for "true", "enabled", "on" and "1"<br>false for "false", "disabled", "off" and "0"
*/
bool IniFile::getBoolValue(std::string section, std::string key, bool defaultValue)
{
	std::string value = getStringValue(section,key,"");
	if(value.size() == 0) {
		return defaultValue;
	}
	
	// convert std::string to lower case
	transform(value.begin(),value.end(), value.begin(), ::tolower);
	
	if((value == "true") || (value == "enabled") || (value == "on") || (value == "1")) {
		return true;
	} else if((value == "false") || (value == "disabled") || (value == "off") || (value == "0")) {
		return false;
	} else {
		return defaultValue;
	}
}

/// Sets the std::string that is adressed by the section/key pair.
/**
	Sets the std::string that is adressed by the section/key pair to value. If the section and/or the key does not exist it will
	be created. A valid sectionname/keyname is not allowed to contain '[',']',';' or '#' and can not start or end with
	whitespaces (' ' or '\\t').
	\param	section			sectionname
	\param	key				keyname
	\param	value			value that should be set
*/
void IniFile::setStringValue(std::string section, std::string key, std::string value)
{
	CommentEntry *curEntry = nullptr;
	SectionEntry *curSection = getSection(section);
	
	if(curSection == nullptr) {
		// create new section
		
		// test for valid section name
		bool NonNormalCharFound = false;
		for(unsigned int i = 0; i < section.size(); i++) {
			if( (!isNormalChar(section[i])) && (!isWhitespace(section[i])) ) {
				NonNormalCharFound = true;
			}
		}
		
		if(isWhitespace(section[0]) || isWhitespace(section[section.size()-1])) {
			NonNormalCharFound = true;
		}
		
		if(NonNormalCharFound == true) {
		    std::cerr << "IniFile: Cannot create section with name " << section << "!" << std::endl;
			return;
		}
		
		std::string completeLine = "[" + section + "]";
		if((curSection = new SectionEntry(completeLine,1,section.size())) == nullptr)
	    	    throw(std::bad_alloc());

		if(FirstLine == nullptr) {
			FirstLine = curSection;
		} else {
			curEntry = FirstLine;
			while(curEntry->nextEntry != nullptr) {
				curEntry = curEntry->nextEntry;
			}
			
			curEntry->nextEntry = curSection;
			curSection->prevEntry = curEntry;
			curEntry = curSection;
		}
					
		InsertSection(curSection);
	}
	
	KeyEntry *curKey = getKey(curSection,key);
	if(curKey == nullptr) {
		// create new key
		
		// test for valid key name
		bool NonNormalCharFound = false;
		for(unsigned int i = 0; i < key.size(); i++) {
			if( (!isNormalChar(key[i])) && (!isWhitespace(key[i])) ) {
				NonNormalCharFound = true;
			}
		}
		
		if(isWhitespace(key[0]) || isWhitespace(key[key.size()-1])) {
			NonNormalCharFound = true;
		}
		
		if(NonNormalCharFound == true) {
		    std::cerr << "IniFile: Cannot create key with name " << key << "!" << std::endl;
			return;
		}

		std::string completeLine;
		int KeyStringStart;
		int KeyStringLength;
		int ValueStringStart;
		int ValueStringLength;
		if(value == "") {
			completeLine = key + " = \"" + value + "\"" ;
			KeyStringStart = 0;
			KeyStringLength = key.size();
			ValueStringStart = key.size()+4;
			ValueStringLength = value.size();
		} else {
			
			// test for non normal char
			NonNormalCharFound = false;
			for(unsigned int i = 0; i < value.size(); i++) {
				if(!isNormalChar(value[i])) {
					NonNormalCharFound = true;
				}
			}
			
			if(NonNormalCharFound == true) {
				completeLine = key + " = \"" + value + "\"" ;
				KeyStringStart = 0;
				KeyStringLength = key.size();
				ValueStringStart = key.size()+4;
				ValueStringLength = value.size();			
			} else {
				completeLine = key + " = " + value;
				KeyStringStart = 0;
				KeyStringLength = key.size();
				ValueStringStart = key.size()+3;
				ValueStringLength = value.size();			
			}
		}
		if((curKey = new KeyEntry(completeLine,KeyStringStart,KeyStringLength,ValueStringStart,ValueStringLength)) == nullptr)
	    	    throw(std::bad_alloc());
		
		if(curEntry != nullptr) {
			curEntry->nextEntry = curKey;
			curKey->prevEntry = curEntry;
		} else {
			KeyEntry *pKey = curSection->KeyRoot;
			if(pKey == nullptr) {
				// Section has no key yet
				if(curSection->nextEntry == nullptr) {
					// no line after this section declaration
					curSection->nextEntry = curKey;
					curKey->prevEntry = curSection;
				} else {
					// lines after this section declaration
					curSection->nextEntry->prevEntry = curKey;
					curKey->nextEntry = curSection->nextEntry;
					curSection->nextEntry = curKey;
					curKey->prevEntry = curSection;
				}
			} else {
				// Section already has some keys
				while(pKey->nextKey != nullptr) {
					pKey = pKey->nextKey;
				}
				
				if(pKey->nextEntry == nullptr) {
					// no line after this key
					pKey->nextEntry = curKey;
					curKey->prevEntry = pKey;					
				} else {
					// lines after this section declaration
					pKey->nextEntry->prevEntry = curKey;
					curKey->nextEntry = pKey->nextEntry;
					pKey->nextEntry = curKey;
					curKey->prevEntry = pKey;
				}
			}
			
		}
					
		InsertKey(curSection,curKey);
	} else {
		curKey->CompleteLine.replace(curKey->ValueStringBegin,curKey->ValueStringLength,value);
	}
}

/// Sets the int that is adressed by the section/key pair.
/**
	Sets the int that is adressed by the section/key pair to value. If the section and/or the key does not exist it will
	be created. A valid sectionname/keyname is not allowed to contain '[',']',';' or '#' and can not start or end with
	whitespaces (' ' or '\\t').
	\param	section			sectionname
	\param	key				keyname
	\param	value			value that should be set
*/
void IniFile::setIntValue(std::string section, std::string key, int value)
{
	char tmp[20];
	sprintf(tmp,"%d",value);
	setStringValue(section, key, tmp);
}

/// Sets the boolean that is adressed by the section/key pair.
/**
	Sets the boolean that is adressed by the section/key pair to value. If the section and/or the key does not exist it will
	be created. A valid sectionname/keyname is not allowed to contain '[',']',';' or '#' and can not start or end with
	whitespaces (' ' or '\\t').
	\param	section			sectionname
	\param	key				keyname
	\param	value			value that should be set
*/
void IniFile::setBoolValue(std::string section, std::string key, bool value)
{
	if(value == true) {
		setStringValue(section, key, "true");
	} else {
		setStringValue(section, key, "false");	
	}
}

/// Opens a IniFile::KeyListHandle.
/**
	A IniFile::KeyListHandle can be used to list all keys of one section. This method opens the #IniFile::KeyListHandle.
	To iterate all keys use KeyList_GetNextKey(). #IniFile::KeyListHandle should be closed by KeyList_Close().
	If the section is empty IniFile::KeyList_EOF on the returned handle of IniFile::KeyList_Open(sectionname) is true.<br>
	<br>
	Example:<br>
	&nbsp;&nbsp;IniFile::KeyListHandle myHandle;<br>
	&nbsp;&nbsp;myHandle = myIniFile.KeyList_Open("Section1");<br>
	<br>
	&nbsp;&nbsp;while(!myIniFile.KeyList_EOF(myHandle)) {<br>
	&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;cout << myIniFile.KeyList_GetNextKey(&myHandle) << std::endl;<br>
	&nbsp;&nbsp;}<br>
	<br>
	&nbsp;&nbsp;myIniFile.KeyList_Close(&myHandle);
	\param	sectionname	The name of the section
	\return	handle to this section
	\see	KeyList_Close, KeyList_GetNextKey, KeyList_EOF
*/
IniFile::KeyListHandle IniFile::KeyList_Open(std::string sectionname) {
	SectionEntry *curSection = getSection(sectionname);
	if(curSection == nullptr) {
		return nullptr;
	} else {
		return curSection->KeyRoot;
	}
}

/// Tests if handle is at the end of the KeyList.
/**
	Tests if handle is at the end of the KeyList. If the section is empty IniFile::KeyList_EOF on the returned
	handle of IniFile::KeyList_Open(sectionname) is true.
	\param	handle	The handle to the KeyList
	\return	true if the handle is at the end of the list, otherwise false
	\see	KeyList_Open
*/
bool IniFile::KeyList_EOF(KeyListHandle handle) {
	if(handle == nullptr) {
		return true;
	} else {
		return false;
	}
}

/// Returns the keyname of the next key in the KeyList.
/**
	Returns the keyname of the next key in the KeyList. If the iteration through the KeyList is finished "" is returned.
	\param	handle	Pointer to the handle to the KeyList
	\return	The keyname or "" if the list is empty
	\see	KeyList_Open, KeyList_EOF
*/
std::string IniFile::KeyList_GetNextKey(KeyListHandle* handle) {
	if(handle == nullptr) {
		return "";
	} else {
		std::string ret = (*handle)->CompleteLine.substr((*handle)->KeyStringBegin,(*handle)->KeyStringLength);
		*handle = (*handle)->nextKey;
		return ret;
	}
}

/// Closes the KeyList.
/**
	Closes the KeyList.
	\param	handle	Pointer to the handle to the KeyList
	\see	KeyList_Open
*/
void IniFile::KeyList_Close(KeyListHandle *handle) {
	if(handle != nullptr)
		*handle = nullptr;
}

/// Saves the changes made in the INI-File to an output stream.
/**
	Saves the changes made in the INI-File to an output stream specified by output.
	If something goes wrong false is returned otherwise true.
	\param	output	ostream that is used for writing. (Cannot be readonly)
	\return	true on success otherwise false.
*/
bool IniFile::SaveChangesTo(std::ostream &output) {
	CommentEntry *curEntry = FirstLine;
	
	while(curEntry != nullptr) {
	    output.write(curEntry->CompleteLine.c_str(), curEntry->CompleteLine.size());
	    curEntry = curEntry->nextEntry;
	}
	
	return true;
}

// private methods

void IniFile::flush()
{
	
	//cout << "Flush:" << endl;
	CommentEntry* curEntry = FirstLine;
	
	while(curEntry != nullptr) {
		//cout << curEntry->CompleteLine << std::endl;
		curEntry = curEntry->nextEntry;
	}

}

void IniFile::readFile()
{	
	if((SectionRoot = new SectionEntry("",0,0)) == nullptr)
    	    throw(std::bad_alloc());
	
	SectionEntry *curSectionEntry = SectionRoot;
		
	std::string completeLine;
	int lineNum = 0;
	bool SyntaxError = false;
	CommentEntry *curEntry = nullptr;
	CommentEntry *newCommentEntry;
	SectionEntry *newSectionEntry;
	KeyEntry *newKeyEntry;
	
	bool readfinished = false;
	
	while(!readfinished) {
		lineNum++;
		
		completeLine = "";
		uint8_t tmp;
		
		size_t size = _stream.sizeg();
		while(static_cast<uint32_t>(_stream.tellg()) < size-1) {
			tmp = _stream.get();
			if(static_cast<uint32_t>(_stream.tellg()) == size-1) {
				readfinished = true;
				break;
			} else if(tmp == '\n') {
				break;
			} else if(tmp != '\r') {
				completeLine += tmp;
			}
		}
		
		const uint8_t *line = reinterpret_cast<const uint8_t*>(completeLine.c_str());
		SyntaxError = false;
		int ret;

		ret = getNextChar(line,0);
		
		if(ret == -1) {
			// empty line or comment
			if((newCommentEntry = new CommentEntry(completeLine)) == nullptr)
		    	    throw(std::bad_alloc());
			
			if(curEntry == nullptr) {
				FirstLine = newCommentEntry;
				curEntry = newCommentEntry;
			} else {
				curEntry->nextEntry = newCommentEntry;
				newCommentEntry->prevEntry = curEntry;
				curEntry = newCommentEntry;
			}
		} else {
		
			if(line[ret] == '[') {
				// section line
				int sectionstart = ret+1;
				int sectionend = skipName(line,ret+1);
			
				if((line[sectionend] != ']') ||	(getNextChar(line,sectionend+1) != -1)) {
					SyntaxError = true;
				} else {
					// valid section line
					if((newSectionEntry = new SectionEntry(completeLine,sectionstart,sectionend-sectionstart)) == nullptr)
				    	    throw(std::bad_alloc());
			
					if(curEntry == nullptr) {
						FirstLine = newSectionEntry;
						curEntry = newSectionEntry;
					} else {
						curEntry->nextEntry = newSectionEntry;
						newSectionEntry->prevEntry = curEntry;
						curEntry = newSectionEntry;
					}
					
					InsertSection(newSectionEntry);
					curSectionEntry = newSectionEntry;
				}
			} else {
			
				// might be key/value line
				int keystart = ret;
				int keyend = skipKey(line,keystart);
				
				if(keystart == keyend) {
					SyntaxError = true;
				} else {
					ret = getNextChar(line,keyend);
					if((ret == -1) ||(line[ret] != '=')) {
						SyntaxError = true;
					} else {
						int valuestart = getNextChar(line,ret+1);
						int valueend;
						if(valuestart == -1) {
							SyntaxError = true;
						} else {
							if(line[valuestart] == '"') {
								// now get the next '"'
								
								valueend = getNextQuote(line,valuestart+1);
								
								if((valueend == -1) || (getNextChar(line,valueend+1) != -1)) {
									SyntaxError = true;
								} else {
									// valid key/value line
									if((newKeyEntry = new KeyEntry(completeLine,keystart,keyend-keystart,valuestart+1,valueend-valuestart-1)) == nullptr)
								    	    throw(std::bad_alloc());

									if(FirstLine == nullptr) {
										FirstLine = newKeyEntry;
										curEntry = newKeyEntry;
									} else {
										curEntry->nextEntry = newKeyEntry;
										newKeyEntry->prevEntry = curEntry;
										curEntry = newKeyEntry;
									}
					
									InsertKey(curSectionEntry,newKeyEntry);										
								}
								
							} else {
								valueend = skipValue(line,valuestart);
								
								if(getNextChar(line,valueend) != -1) {
									SyntaxError = true;
								} else {
									// valid key/value line
									if((newKeyEntry = new KeyEntry(completeLine,keystart,keyend-keystart,valuestart,valueend-valuestart)) == nullptr)
								    	    throw(std::bad_alloc());


									if(FirstLine == nullptr) {
										FirstLine = newKeyEntry;
										curEntry = newKeyEntry;
									} else {
										curEntry->nextEntry = newKeyEntry;
										newKeyEntry->prevEntry = curEntry;
										curEntry = newKeyEntry;
									}
					
									InsertKey(curSectionEntry,newKeyEntry);									
								}
							}
						}
					}
				}
				
			}
		}
		
		if(SyntaxError == true) {
			if(completeLine.size() < 100) {
				// there are some buggy ini-files which have a lot of waste at the end of the file
				// and it makes no sense to print all this stuff out. just skip it
			    std::cerr << "IniFile: Syntax-Error in line " << lineNum << ":" << completeLine << " !" << std::endl;
			}
			// save this line as a comment
			if((newCommentEntry = new CommentEntry(completeLine)) == nullptr)
		    	    throw(std::bad_alloc());
			
			if(curEntry == nullptr) {
				FirstLine = newCommentEntry;
				curEntry = newCommentEntry;
			} else {
				curEntry->nextEntry = newCommentEntry;
				newCommentEntry->prevEntry = curEntry;
				curEntry = newCommentEntry;
			}		
		}
		
		
		
	}
}

void IniFile::InsertSection(SectionEntry *newSection) {
	if(SectionRoot == nullptr) {
		// New root element
		SectionRoot = newSection;
	} else {
		// insert into list
		SectionEntry* curSection = SectionRoot;
		while(curSection->nextSection != nullptr) {
			curSection = curSection->nextSection;
		}
		
		curSection->nextSection = newSection;
		newSection->prevSection = curSection;
	}
}

void IniFile::InsertKey(SectionEntry *section, KeyEntry *newKeyEntry) {
	if(section->KeyRoot == nullptr) {
		// New root element
		section->KeyRoot = newKeyEntry;
	} else {
		// insert into list
		KeyEntry* curKey = section->KeyRoot;
		while(curKey->nextKey != nullptr) {
			curKey = curKey->nextKey;
		}
		
		curKey->nextKey = newKeyEntry;
		newKeyEntry->prevKey = curKey;
	}
}


IniFile::SectionEntry *IniFile::getSection(std::string sectionname) {
	SectionEntry *curSection = SectionRoot;
	int sectionnameSize = sectionname.size(); 
	
	while(curSection != nullptr) {
		if(curSection->SectionStringLength == sectionnameSize) {
				if(strncmp(sectionname.c_str(), curSection->CompleteLine.c_str()+curSection->SectionStringBegin, sectionnameSize) == 0) {
					return curSection;
				}
		}
		
		curSection = curSection->nextSection;
	}
	
	return nullptr;
}

IniFile::KeyEntry *IniFile::getKey(SectionEntry *sectionentry, std::string keyname) {
	KeyEntry *curKey = sectionentry->KeyRoot;
	int keynameSize = keyname.size(); 
	
	while(curKey != nullptr) {
		if(curKey->KeyStringLength == keynameSize) {
				if(strncmp(keyname.c_str(), curKey->CompleteLine.c_str()+curKey->KeyStringBegin, keynameSize) == 0) {
					return curKey;
				}
		}
		
		curKey = curKey->nextKey;
	}
	
	return nullptr;
}


int IniFile::getNextChar(const unsigned char *line, int startpos) {
	while(line[startpos] != '\0') {
		if((line[startpos] == ';') || (line[startpos] == '#')) {
			// comment
			return -1;
		} else if(!isWhitespace(line[startpos])) {
			return startpos;
		}
		startpos++;
	}
	return -1;
}

int IniFile::skipName(const unsigned char *line, int startpos) {
	while(line[startpos] != '\0') {
		if(isNormalChar(line[startpos])) {
			startpos++;
		} else {
			return startpos;
		}
	}
	return startpos;
}

int IniFile::skipValue(const unsigned char *line, int startpos) {
	int i = startpos;
	while(line[i] != '\0') {
		if(isNormalChar(line[i]) || isWhitespace(line[i])) {
			i++;
		} else if((line[i] == ';') || (line[i] == '#')) {
			// begin of a comment
			break;
		} else {
			// some invalid character
			return i;
		}
	}
	
	// now we go backwards
	while(i >= startpos) {
		if(isNormalChar(line[i])) {
			return i+1;
		}
		i--;
	}
	return startpos+1;
}

int IniFile::skipKey(const unsigned char *line,int startpos) {
	int i = startpos;
	while(line[i] != '\0') {
		if(isNormalChar(line[i]) || isWhitespace(line[i])) {
			i++;
		} else if((line[i] == ';') || (line[i] == '#') || (line[i] == '=')) {
			// begin of a comment or '='
			break;
		} else {
			// some invalid character
			return i;
		}
	}
	
	// now we go backwards
	while(i >= startpos) {
		if(isNormalChar(line[i])) {
			return i+1;
		}
		i--;
	}
	return startpos+1;
}

int IniFile::getNextQuote(const unsigned char *line,int startpos) {
	while(line[startpos] != '\0') {
		if(line[startpos] != '"') {
			startpos++;
		} else {
			return startpos;
		}
	}
	return -1;
}

bool IniFile::isWhitespace(unsigned char s) {
	if((s == ' ') || (s == '\t') || (s == '\n') || (s == '\r')) {
		return true;
	} else {
		return false;
	}
}

bool IniFile::isNormalChar(unsigned char s) {
	if((!isWhitespace(s)) && (s >= 33) && (s != '"') && (s != ';') && (s != '#') && (s != '[') && (s != ']') && (s != '=')) {
		return true;
	} else {
		return false;
	}
}

}
