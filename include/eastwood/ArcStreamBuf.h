#ifndef EASTWOOD_ARCSTREAM_H
#define	EASTWOOD_ARCSTREAM_H

#include "eastwood/ArcFileInfo.h"
#include "eastwood/Log.h"
#include "eastwood/StdDef.h"

#include <streambuf>
#include <vector>
#include <string>
#include <cstdio>
#include <iostream>

namespace eastwood {

/*
 stdio stream buffer for using files within a solid archive (uncompressed)
*/
    
template < typename CharType, typename CharTraits = std::char_traits <CharType> >
    
class basic_arcstream : public std::basic_streambuf <CharType, CharTraits>
{
public:
    typedef CharType                                char_type;
    typedef CharTraits                              traits_type;
    typedef typename traits_type::int_type          int_type;
    typedef typename traits_type::pos_type          pos_type;
    typedef typename traits_type::off_type          off_type;
    typedef std::ios_base::seekdir                  seekdir;
    typedef std::ios_base::openmode                 openmode;
    
    typedef basic_arcstream <char_type, traits_type> this_type;
    
    basic_arcstream(FILE* fp = NULL) :
    _fp(fp), _soffset(0), _eoffset(0), _pushbacks(0)
    {
        if(_fp != NULL){
            fseek(_fp, 0, SEEK_END);
            _eoffset = ftell(_fp);
            fseek(_fp, _soffset, SEEK_SET);
        } else {
            LOG_DEBUG("%s", "constructed stream with null file pointer");
        }
    }
    
    basic_arcstream(ArcFileInfo& fileinfo) :
         _fp(NULL), _soffset(fileinfo.start), _eoffset(fileinfo.start + fileinfo.size)
        {
            _fp = fopen(fileinfo.archivepath.c_str(), "rb");
            if(_fp != NULL) {
                fseek(_fp, _soffset, SEEK_SET);
            } else {
                _soffset = _eoffset = 0;
            }
        }
    
    ~basic_arcstream() { this->close(); }
    
    this_type* open(const char* filename, std::ios_base::openmode mode)
    {
        if (is_open()) return NULL;

        // Figure out the open mode flags  . . . . . . . . . . . . . . . . . .
        std::string fmode;

        bool is_ate = mode & std::ios_base::ate;
        bool is_bin = mode & std::ios_base::binary;
        mode &= ~(std::ios_base::ate | std::ios_base::binary);
        
        #define _(M) std::ios_base::M
        if      (mode == (         _(in)                    )) fmode = "r";
        else if (mode == (                 _(out) & _(trunc))) fmode = "w";
        else if (mode == (                 _(out)           )) fmode = "w";
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
    
    this_type* open(ArcFileInfo& fileinfo)
    {
        if(!fileinfo.size) return NULL;
        _soffset = fileinfo.start;
        _eoffset = fileinfo.start + fileinfo.size;
                
        _fp = fopen(fileinfo.archivepath.c_str(), "rb");
        
        if(_fp != NULL) {
            fseek(_fp, _soffset, SEEK_SET);
        } else {
            _soffset = _eoffset = 0;
        }
        
        return this;
    }
    
    this_type* close()
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
    
    bool is_open() { return _fp != NULL; }
    
    FILE* getiofile() const { return _fp; }
    
    int size() { return _eoffset; }
    
protected:
    virtual int_type underflow()
    {
        // Return anything previously pushed-back
        if (_pushbacks.size())
          return _pushbacks.back();

        //check if our file position is past where EOF is
        if(ftell(_fp) >= _eoffset) return traits_type::eof();

        // Else do the right thing
        fpos_t pos;
        if (fgetpos(_fp, &pos) != 0)
          return traits_type::eof();

        int c = fgetc(_fp);
        std::fsetpos(_fp, &pos);

        return c;
    }
    
    virtual int_type uflow()
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
    
    virtual int_type pbackfail(int_type c)
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

    virtual int_type overflow(int_type c)
    {
        _pushbacks.clear();
        int_type rv;
        // Do nothing
        if (traits_type::eq_int_type(c, traits_type::eof()))
          return 0;

        // Else write a character
        rv = fputc(c, _fp);
        if(rv != EOF) _eoffset++;
        return rv;
    }
    
    virtual this_type* setbuf(char* s, std::streamsize n)
    {
        return setvbuf(_fp, s, (s and n) ? _IOLBF : _IONBF, (size_t)n) ? NULL : this;
    }
    
    virtual std::streamsize xsgetn(char* dest, std::streamsize n)
    {
        unsigned int nread = 0;

        if(ftell(_fp) >= _eoffset) return 0;

        if((ftell(_fp) + n) < _eoffset) {
            nread = fread(dest, 1, n, _fp);
        } else {
            nread = fread(dest, 1, (ftell(_fp) + n) - _eoffset, _fp);
        }

        return nread;
    }
    
    virtual std::streamsize xsputn(const char_type* src, std::streamsize n)
    {
        std::streamsize rv = fwrite(src, 1, n, _fp);
        _eoffset += rv;
        return rv;
    }
    
    virtual pos_type seekoff(off_type offset, seekdir direction, openmode mode = std::ios_base::in | std::ios_base::out)
    {
        _pushbacks.clear();
        int pos = (direction == std::ios_base::beg) ? _soffset + offset :
                       (direction == std::ios_base::cur) ? ftell(_fp) + offset :
                                                           _eoffset + offset;

        return std::fseek(_fp, pos, SEEK_SET) ? (-1) : pos - _soffset;
    }
    
    virtual pos_type seekpos(pos_type offset, openmode mode = std::ios_base::in | std::ios_base::out)
    {
        return seekoff(offset, std::ios_base::beg, mode);
    }
    
    virtual int sync()
    {
        _pushbacks.clear();
        return fflush(_fp) ? traits_type::eof() : 0;
    }
    
    virtual std::streamsize showmanyc()
    {
        return ftell(_fp) < _eoffset? ftell(_fp) - _eoffset : -1;
    }
    
    FILE* _fp;
    int _soffset;
    int _eoffset;
    std::vector<int_type> _pushbacks;
    
private:
    basic_arcstream(const basic_arcstream&);
    basic_arcstream& operator=(const basic_arcstream&);
};
    
}//eastwood

#endif	/* EASTWOOD_ARCSTREAM_H */

