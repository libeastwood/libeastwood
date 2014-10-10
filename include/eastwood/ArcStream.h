#ifndef EASTWOOD_ARCSTREAM_H
#define	EASTWOOD_ARCSTREAM_H

#include "eastwood/ArcFileInfo.h"

#include <streambuf>
#include <vector>
#include <string>
#include <cstdio>
#include <iostream>

namespace eastwood {
    
class ArcStream : public std::streambuf
{
protected:
    typedef std::streambuf::pos_type        pos_type;
    typedef std::streambuf::off_type        off_type;
    typedef std::ios_base::seekdir               seekdir;
    typedef std::ios_base::openmode              openmode;
    
public:
    ArcStream(FILE* fp = NULL);
    ArcStream(ArcFileInfo& fileinfo);
    ~ArcStream() { this->close(); }
    
    ArcStream* open(const char* filename, openmode mode);
    ArcStream* close();
    bool is_open() { return _fp != NULL; }
    FILE* getiofile() const { return _fp; }
    
protected:
    virtual int_type underflow();
    virtual int_type uflow();
    virtual int_type pbackfail(int_type c = traits_type::eof());
    virtual int_type overflow(int_type c = traits_type::eof());
    virtual ArcStream* setbuf(char* s, std::streamsize n);
    virtual std::streamsize xsgetn(char* dest, std::streamsize n);
    virtual std::streamsize xsputn(char* src, std::streamsize n);
    virtual pos_type seekoff(off_type offset, seekdir direction, openmode mode = std::ios_base::in | std::ios_base::out);
    virtual pos_type seekpos(pos_type offset, openmode mode = std::ios_base::in | std::ios_base::out);
    virtual int sync();
    FILE* _fp;
    int _soffset;
    int _eoffset;
    std::vector <int_type> _pushbacks;
    
private:
    ArcStream(const ArcStream&);
    ArcStream& operator=(const ArcStream&);
};
    
}//eastwood

#endif	/* EASTWOOD_ARCSTREAM_H */

