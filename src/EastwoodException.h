#ifndef	EASTWOOD_EASTWOODEXCEPTION_H
#define	EASTWOOD_EASTWOODEXCEPTION_H

#include <sstream>
#include <string>

class EastwoodException {
    public:
        EastwoodException(std::string message);
        inline std::string getMessage() { return message; };
        inline int getErrorCode() { return errorCode; };
        inline bool isFatal() { return fatal; };

    private:
        std::string message;
        int errorCode;
        bool fatal;
};

class FileNotFoundException : public EastwoodException {
    public:
        FileNotFoundException( std::string filename );
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
        NullSizeException(std::string message);
};
#endif // EASTWOOD_EASTWOODEXCEPTION_H
// vim:ts=8:sw=4:et

