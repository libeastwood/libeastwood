#include "eastwood/ArcStream.h"
#include "eastwood/Log.h"
#include <cstring>

namespace eastwood {

ArcStream::ArcStream(FILE* fp) :
_fp(fp), _soffset(0), _eoffset(0), _pushbacks(0)
{
    if(_fp != NULL){
        fseek(_fp, 0, SEEK_END);
        _eoffset = ftell(_fp);
        fseek(_fp, _soffset, SEEK_SET);
    }
}

ArcStream::ArcStream(ArcFileInfo& fileinfo) :
 _fp(NULL), _soffset(fileinfo.start), _eoffset(fileinfo.start + fileinfo.size)
{
    _fp = fopen(fileinfo.archivepath.c_str(), "rb");
    if(_fp != NULL) {
        fseek(_fp, _soffset, SEEK_SET);
    } else {
        _soffset = _eoffset = 0;
    }
}

ArcStream* ArcStream::open(const char* filename, std::ios_base::openmode mode)
{
    if (is_open()) return NULL;

    // Figure out the open mode flags  . . . . . . . . . . . . . . . . . .
    std::string fmode;

    bool is_ate = mode & std::ios_base::ate;
    bool is_bin = mode & std::ios_base::binary;
    mode &= ~(std::ios_base::ate | std::ios_base::binary);

    #define _(flag) std::ios_base::flag
    if      (mode == (         _(in)                    )) fmode = "r";
    else if (mode == (                 _(out) & _(trunc))) fmode = "w";
    else if (mode == (_(app)         & _(out)           )) fmode = "a";
    else if (mode == (         _(in) & _(out)           )) fmode = "r+";
    else if (mode == (         _(in) & _(out) & _(trunc))) fmode = "w+";
    else if (mode == (_(app) & _(in) & _(out)           )) fmode = "a+";
    else return NULL;
    #undef _
    if (is_bin) fmode.insert( 1, 1, 'b' );
    
    //open the file and get its size
    _fp = fopen(filename, fmode.c_str());
    if(_fp == NULL) return NULL;

    fseek(_fp, 0, SEEK_END);
    _eoffset = ftell(_fp);
    fseek(_fp, _soffset, SEEK_SET);
    
    //reposition to end if needed
    if(is_ate) fseek(_fp, 0, SEEK_END);
    
    return this;
}

ArcStream* ArcStream::close()
{
    _soffset = 0;
    _eoffset = 0;
    if(_fp != NULL)
    {
        fclose(_fp);
        _fp = NULL;
    }
    _pushbacks.clear();
    return this;
}

std::streambuf::traits_type::int_type ArcStream::underflow()
{
    // Return anything previously pushed-back
    if (_pushbacks.size())
      return _pushbacks.back();

    //check if our file position is past where EOF is
    if(ftell(_fp) >= _eoffset) return traits_type::eof();
    
    // Else do the right thing
    fpos_t pos;
    if (std::fgetpos( _fp, &pos ) != 0)
      return traits_type::eof();

    int c = std::fgetc( _fp );
    std::fsetpos( _fp, &pos );

    return c;
}

std::streambuf::traits_type::int_type ArcStream::uflow()
{
    // Return anything previously pushed-back
    if (_pushbacks.size()) {
        int_type c = _pushbacks.back();
        _pushbacks.pop_back();
        return c;
    }
    
    //check if our file position is past where EOF is
    if(ftell(_fp) >= _eoffset) return traits_type::eof();
    
    // Else do the right thing
    return fgetc(_fp);
}

std::streambuf::traits_type::int_type ArcStream::pbackfail(int_type c)
{
    if (!is_open())
      return traits_type::eof();

    // If the argument c is EOF and the file pointer is not at the
    // beginning of the character sequence, it is decremented by one.
    if (traits_type::eq_int_type(c, traits_type::eof())) {
        _pushbacks.clear();
        return fseek(_fp, -1L, SEEK_CUR) ? traits_type::eof() : 0;
    }

    // Otherwise, make the argument the next value to be returned by
    // underflow() or uflow()
    _pushbacks.push_back(c);
    return c;
}

std::streambuf::traits_type::int_type ArcStream::overflow(int_type c)
{
    _pushbacks.clear();

    // Do nothing
    if (traits_type::eq_int_type(c, traits_type::eof()))
      return 0;

    // Else write a character
    return fputc(c, _fp);
}

std::streamsize ArcStream::xsgetn(char* dest, std::streamsize n)
{
    unsigned int nread = 0;
    
    if(ftell(_fp) >= _eoffset) return 0;
    
    if((ftell(_fp) + n) < _eoffset) {
        nread = fread(dest, n, 1, _fp);
    } else {
        nread = fread(dest, (ftell(_fp) + n) - _eoffset, 1, _fp);
    }

    return nread;
}

std::streamsize ArcStream::xsputn(char* src, std::streamsize n)
{
    return fwrite(src, n, 1, _fp);
}

ArcStream* ArcStream::setbuf(char* s, std::streamsize n)
{
    return setvbuf(_fp, s, (s and n) ? _IOLBF : _IONBF, (size_t)n) ? NULL : this;
}

std::streambuf::pos_type ArcStream::seekoff(
    off_type offset, 
    seekdir direction, 
    openmode mode)
{
    _pushbacks.clear();
    int pos = (direction == std::ios_base::beg) ? _soffset + offset :
                   (direction == std::ios_base::cur) ? ftell(_fp) + offset :
                                                       _eoffset + offset;
    
    return std::fseek(_fp, pos, SEEK_SET) ? (-1) : pos;
}

std::streambuf::pos_type ArcStream::seekpos(pos_type offset, openmode mode)
{
    return seekoff(offset, std::ios_base::beg, mode);
}

int ArcStream::sync()
{
    _pushbacks.clear();
    return fflush(_fp) ? traits_type::eof() : 0;
}

}//eastwood
