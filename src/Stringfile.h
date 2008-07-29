#ifndef STRINGFILE_H_INCLUDED
#define STRINGFILE_H_INCLUDED

#include <string>


#define MISSION_DESCRIPTION		0
#define MISSION_WIN				1
#define MISSION_LOSE			2
#define MISSION_ADVICE			3


/// A class for loading a encoded textfiles.
/*!
	This class can read encoded textfiles and return their content in decoded ANSI Code.
*/
class Stringfile
{
public:
	Stringfile(std::string stringfileName);
	~Stringfile();

	/*!
	 * This method returns the briefing/debriefing text for the mission specified by the parameter mission. The second
	 *  parameter specifies the kind of briefing/debriefing. 
	 *  @param mission the mission number (0=House description; 1,2,...,9 = mission description).
	 *  @param texttype one of MISSION_DESCRIPTION, MISSION_WIN, MISSION_LOSE, MISSION_ADVICE
	 *  @return the text for this mission and of this type.
	*/
	std::string getString(unsigned int mission, unsigned int texttype) {
		int index = mission*4+texttype;
		
		if(index < numStrings) {
			return stringArray[index];
		} else {
			return "Stringfile::getString(): mission number or text type is invalid!\n";
		}
	}
	std::string getString(int i){
		return stringArray[i];
	}

private:
    /*!
     *  This method decodes a string to ANSI Code
     *  The parameter text is decoded to ANSI Code and returned
     *  @param text	Text to decode
     *  @return The decoded text
     */
	std::string decodeString(std::string text);
	std::string *stringArray;
	int numStrings;
};

#endif // STRINGFILE_H_INCLUDED
