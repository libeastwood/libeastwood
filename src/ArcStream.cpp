#include "eastwood/ArcStream.h"
#include <cstring>

namespace eastwood {

ArcStream::ArcStream(FILE* fd, int size, int offset, int bufsize) :
_mode(std::ios_base::in), _descriptor(fd), _soffset(offset), _eoffset(offset + size), 
_coffset(offset), _bufsize(bufsize), _buffer(bufsize)
{
    fseek(_descriptor, _soffset, SEEK_SET);
    setg(&_buffer.front(), &_buffer.front() + _bufsize, &_buffer.front() + _bufsize);
}

ArcStream::ArcStream(std::string filename, int mode, int bufsize) :
_mode(mode), _descriptor(NULL), _soffset(0), _eoffset(0), _coffset(0),
_bufsize(bufsize), _buffer(bufsize)
{
    open(filename);
}

ArcStream::~ArcStream()
{
    close();
}

void ArcStream::open(std::string& filename)
{
    std::string accmode = "rb";
    if((_mode & std::ios_base::out) == std::ios_base::out)
        accmode = "w+b";
    
    //open the file and get its size
    _descriptor = fopen(filename.c_str(), accmode.c_str());
    if(_descriptor == NULL) throw("error");

    fseek(_descriptor, 0, SEEK_END);
    _eoffset = ftell(_descriptor);
    fseek(_descriptor, _soffset, SEEK_SET);
    
    setg(&_buffer.front(), &_buffer.front() + _bufsize, &_buffer.front() + _bufsize);
}

void ArcStream::close()
{
    _mode = 0;
    _soffset = 0;
    _eoffset = 0;
    _coffset = 0;
    _bufsize = 0;
    _buffer.erase(_buffer.begin(), _buffer.end());
    fclose(_descriptor);
    _descriptor = NULL;
}

int ArcStream::underflow()
{
    if (gptr() < egptr())
        return *gptr();
    
    //am I near end of virtual file?
    int readsize = _coffset + _bufsize > _eoffset ? _eoffset - _coffset : _bufsize;
    if (readsize <= 0)
        return EOF;
    
    int nread = fread(&_buffer.front(), readsize, 1, _descriptor);

    if (nread <= 0)
        return EOF;
    
    _coffset += nread;
    setg(&_buffer.front(), &_buffer.front(), &_buffer.front() + nread);
    return *gptr();
}

std::streamsize ArcStream::xsgetn(char* dest, std::streamsize n)
{
    unsigned int nread = 0;

    while (n)
    {
        if (!in_avail())
        {
            if (underflow() == EOF)
                break;
        }

        unsigned int avail = in_avail();

        if (avail > n)
            avail = n;

        std::memcpy(dest + nread, gptr(), avail);
        gbump(avail);

        nread += avail;
        n -= avail;
    }

    return nread;
}

std::streambuf::pos_type ArcStream::seekoff(off_type offset, seekdir dir, openmode mode)
{
    //coffset is at end of our current buffer, so work out where we really think we are
    off_type current = _coffset - (egptr() - gptr());
    
    //decide where in the file we really want to be
    off_type truepos = (dir == std::ios_base::beg) ? _soffset + offset :
                       (dir == std::ios_base::cur) ? current + offset :
                                                _eoffset + offset;
    
    truepos = truepos > _eoffset ? _eoffset : truepos;
    
    pos_type pos = truepos - _soffset;
    
    if(pos < 0) return -1;
    
    //decide if we need to move the actual file pointer and refill the buffer
    if(truepos > _coffset || truepos < _coffset - _bufsize){
        fseek(_descriptor, truepos, SEEK_SET);
        setg(&_buffer.front(), &_buffer.front() + _bufsize, &_buffer.front() + _bufsize);
    } else {
        setg(&_buffer.front(), gptr() + offset, egptr());
    }
    
    return pos;
}

std::streambuf::pos_type ArcStream::seekpos(pos_type offset, openmode mode)
{
    return seekoff(offset, std::ios_base::beg, mode);
}

}//eastwood
