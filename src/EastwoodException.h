#ifndef	EASTWOOD_EASTWOODEXCEPTION_H
#define	EASTWOOD_EASTWOODEXCEPTION_H

#include <sstream>
#include <string>

#include "Log.h"

class EastwoodException {
    public:
        EastwoodException(logLevel level, std::string location, std::string message);
        inline logLevel getLogLevel() { return level; }
        inline std::string getLocation() { return location; }
        inline std::string getMessage() { return message; }

    private:
        logLevel level;
        std::string location;
        std::string message;
};

class FileNotFoundException : public EastwoodException {
    public:
        FileNotFoundException(logLevel level, std::string location, std::string message);
        inline std::string getFilename() { return filename; };

        inline std::string getMessage() {
            std::stringstream ss;
            ss << "File not found: " << filename;
            return ss.str();
        }

    private:
        std::string filename;


};

class NullSizeException : public EastwoodException {
    public:
        NullSizeException(logLevel level, std::string location, std::string message);
};
#endif // EASTWOOD_EASTWOODEXCEPTION_H
// vim:ts=8:sw=4:et

