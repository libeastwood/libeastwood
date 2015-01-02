#ifndef EASTWOOD_ARCSTREAM_H
#define	EASTWOOD_ARCSTREAM_H

#include <streambuf>
#include <vector>
#include <string>
#include <stdio.h>

namespace eastwood {
    
class ArcStream : public std::streambuf
{
protected:
    typedef std::streambuf::pos_type        pos_type;
    typedef std::streambuf::off_type        off_type;
    typedef std::ios_base::seekdir               seekdir;
    typedef std::ios_base::openmode              openmode;
    
public:
    ArcStream() : _mode(0), _descriptor(0), _soffset(0), _eoffset(0), _coffset(0),
                  _bufsize(0), _buffer(0){}
    ArcStream(FILE* fd, int size, int offset = 0, int bufsize = 256);
    ArcStream(std::string filename, int mode = std::ios_base::in, 
              int bufsize = 256);
    ~ArcStream();
    
    void open(std::string& filename);
    void close();
    
protected:
    int_type underflow();
    std::streamsize xsgetn(char *dest, std::streamsize n);
    pos_type seekoff(off_type offset, seekdir dir, openmode mode);
    pos_type seekpos(pos_type offset, openmode mode);
    int _mode;
    FILE* _descriptor;
    int _soffset;
    int _eoffset;
    int _coffset;
    int _bufsize;
    std::vector<char> _buffer;
};
    
}//eastwood

#endif	/* EASTWOOD_ARCSTREAM_H */

