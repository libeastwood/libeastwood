#ifndef EASTWOOD_STRINGFILE_H
#define EASTWOOD_STRINGFILE_H

#include <string>
#include <vector>

#include "eastwood/IStream.h"

namespace eastwood {

enum MissionType {
    MISSION_DESCRIPTION	= 0,
    MISSION_WIN		= 1,
    MISSION_LOSE	= 2,
    MISSION_ADVICE	= 3,
    MISSION_INVALID
};

/// A class for loading a encoded textfiles.
/*!
	This class can read encoded textfiles and return their content in decoded ANSI Code.
*/
class StringFile
{
    public:
	StringFile(const std::istream &stream);
	virtual ~StringFile();

	/*!
	 * This method returns the briefing/debriefing text for the mission specified by the parameter mission. The second
	 *  parameter specifies the kind of briefing/debriefing. 
	 *  @param mission the mission number (0=House description; 1,2,...,9 = mission description).
	 *  @param texttype one of MISSION_DESCRIPTION, MISSION_WIN, MISSION_LOSE, MISSION_ADVICE
	 *  @return the text for this mission and of this type.
	 */
	inline std::string getString(uint16_t mission, MissionType texttype) {
	    return _strings.at(mission*4+texttype);
	}

	inline std::string getString(uint16_t i) {
	    return _strings.at(i);
	}

	uint16_t size() const throw() {
	    return _strings.size();
	}

    private:
	/*!
	 *  This method decodes a string to ANSI Code
	 *  The parameter offset is the end offset of the text to decode to ANSI Code and returned
	 *  @param offset	End offset of text to decode
	 *  @return The decoded text
	 */
	void readHeader();
	std::string decodeString(uint16_t offset);
	IStream &_stream;
	std::vector<std::string> _strings;
	bool _compressed;
};

}
#endif // EASTWOOD_STRINGFILE_H
