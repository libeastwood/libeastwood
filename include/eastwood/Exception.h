#ifndef	EASTWOOD_EXCEPTION_H
#define	EASTWOOD_EXCEPTION_H

#include <sstream>
#include <string>

namespace eastwood {

enum logLevel
{
    LOG_FATAL,
    LOG_ERROR,
    LOG_WARNING,
    LOG_INFO
};

class Exception {
    public:
        Exception(logLevel level, std::string location, std::string message);
        virtual ~Exception(){}
        inline logLevel getLogLevel() { return _level; }
        inline std::string getLocation() { return _location; }
        inline std::string getMessage() { return _message; }

    protected:
        logLevel _level;
        std::string _location;
        std::string _message;
};

class FileException : public Exception {
    public:
        FileException(logLevel level, std::string location, std::string filename, std::string message);
        inline std::string getFilename() { return _filename; };

        inline std::string getMessage() {
            std::stringstream ss;
            ss << _filename << ": " << _message;
            return ss.str();
        }

    private:
        std::string _filename;


};

class FileNotFoundException : public FileException {
    public:
        FileNotFoundException(logLevel level, std::string location, std::string filename);
};

class NullSizeException : public Exception {
    public:
        NullSizeException(logLevel level, std::string location, std::string message);
};

}
#endif // EASTWOOD_EASTWOODEXCEPTION_H

// vim:ts=8:sw=4:et
