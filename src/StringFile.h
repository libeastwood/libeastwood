#ifndef EASTWOOD_STRINGFILE_H
#define EASTWOOD_STRINGFILE_H

#include <string>
#include <vector>


#define MISSION_DESCRIPTION		0
#define MISSION_WIN			1
#define MISSION_LOSE			2
#define MISSION_ADVICE			3


/// A class for loading a encoded textfiles.
/*!
	This class can read encoded textfiles and return their content in decoded ANSI Code.
*/
class StringFile
{
public:
	StringFile(const unsigned char *bufFileData);
	~StringFile();

	/*!
	 * This method returns the briefing/debriefing text for the mission specified by the parameter mission. The second
	 *  parameter specifies the kind of briefing/debriefing. 
	 *  @param mission the mission number (0=House description; 1,2,...,9 = mission description).
	 *  @param texttype one of MISSION_DESCRIPTION, MISSION_WIN, MISSION_LOSE, MISSION_ADVICE
	 *  @return the text for this mission and of this type.
	*/
	inline std::string getString(unsigned int mission, unsigned int texttype) {
		return strings[mission*4+texttype];
	}

	inline std::string getString(int i){
		return strings[i];
	}

private:
    /*!
     *  This method decodes a string to ANSI Code
     *  The parameter text is decoded to ANSI Code and returned
     *  @param text	Text to decode
     *  @return The decoded text
     */
	std::string decodeString(std::string text);
	std::vector<std::string> strings;
};

#endif // EASTWOOD_STRINGFILE_H
