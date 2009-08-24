#ifndef	EASTWOOD_EXCEPTION_H
#define	EASTWOOD_EXCEPTION_H

#include <sstream>
#include <string>

enum logLevel
{
    LOG_FATAL,
    LOG_ERROR,
    LOG_WARNING,
    LOG_INFO
};

namespace eastwood {
    class Exception {
        public:
            Exception(logLevel level, std::string location, std::string message);
            virtual ~Exception(){}
            inline logLevel getLogLevel() { return m_level; }
            inline std::string getLocation() { return m_location; }
            inline std::string getMessage() { return m_message; }

        protected:
            logLevel m_level;
            std::string m_location;
            std::string m_message;
    };

    class FileException : public Exception {
        public:
            FileException(logLevel level, std::string location, std::string filename, std::string message);
            inline std::string getFilename() { return m_filename; };

            inline std::string getMessage() {
                std::stringstream ss;
                ss << m_filename << ": " << m_message;
                return ss.str();
            }

        private:
            std::string m_filename;


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
