#ifndef EASTWOOD_OSTREAM_H
#define EASTWOOD_OSTREAM_H

//#include <ostream>
#include "ArcStreamBuf.h"

#include "eastwood/StdDef.h"

namespace eastwood {

template < typename CharType, typename CharTraits = std::char_traits <CharType> >

class basic_arcostream : public std::basic_ostream <CharType, CharTraits>
{
public:
    typedef CharType                                      char_type;
    typedef CharTraits                                    traits_type;

    typedef basic_arcstream         <char_type, traits_type>  sbuf_type;
    typedef std::basic_streambuf    <char_type, traits_type>  bbuf_type;
    typedef basic_arcostream        <char_type, traits_type>  this_type;
    typedef std::basic_ostream      <char_type, traits_type>  base_type;
    
    basic_arcostream(FILE* fp = NULL) :
    base_type(new sbuf_type(fp))
    {}
    
    basic_arcostream(bbuf_type* sb) :
    base_type(reinterpret_cast<sbuf_type*>(sb))
    {}
    
    basic_arcostream(const base_type& sb) :
    base_type(reinterpret_cast<sbuf_type*>(sb.rdbuf()))
    {}
    
    basic_arcostream(const char* filename, std::ios_base::openmode mode = std::ios_base::out) :
    base_type((new sbuf_type)->open(filename, mode))
    {}
    
    basic_arcostream(ArcFileInfo& fileinfo) :
    base_type(new sbuf_type(fileinfo))
    {}
    
    void open(const char* filename, std::ios_base::openmode mode = std::ios_base::out) 
    {
        sbuf_type* buf = static_cast<sbuf_type*>(this->rdbuf());
        if (!(buf->open(filename, mode)));
    }
    
    void open(ArcFileInfo& fileinfo) 
    {
        sbuf_type* buf = static_cast<sbuf_type*>(this->rdbuf());
        if (!(buf->open(fileinfo)))
        this->setstate(std::ios_base::badbit);
    }
    
    void close()
    {
        sbuf_type* buf = static_cast<sbuf_type*>(this->rdbuf());
        buf->close();
    }
    
    bool is_open()
    {
        sbuf_type* buf = static_cast<sbuf_type*>(this->rdbuf());
        return buf->is_open();
    }
    
    std::streamsize sizep()
    {
        sbuf_type* buf = static_cast<sbuf_type*>(this->rdbuf());
        return static_cast<std::streamsize>(buf->size());
    }
    
private:
    template <typename T> inline
    this_type& writeT(T &value)
    {
        return reinterpret_cast<this_type&>(this->write(reinterpret_cast<char*>(&value), sizeof(value)));
    }

    template <typename T> inline
    this_type& putT(T value)
    {
        return writeT<T>(value);
    }
    
public:
    this_type& putU16BE(uint16_t value)
    {
        return putT<uint16_t>(htobe16(value));
    }

    this_type& putU16LE(uint16_t value)
    {
        return putT<uint16_t>(htole16(value));
    }

    this_type& putU32BE(uint32_t value)
    {
        return putT<uint32_t>(htobe32(value));
    }

    this_type& putU32LE(uint32_t value)
    {
        return putT<uint32_t>(htole32(value));
    }

#if __BYTE_ORDER == __BIG_ENDIAN
    this_type& writeU16BE(uint16_t *buf, size_t n)
    {
        return reinterpret_cast<this_type&>(this->write((char*)buf, n*sizeof(buf[0])));
    }

    this_type& writeU32BE(uint32_t *buf, size_t n)
    {
        return reinterpret_cast<this_type&>(this->write((char*)buf, n*sizeof(buf[0])));
    }

    this_type& writeU16LE(uint16_t *buf, size_t n)
    {
        for(size_t i = 0; i < n; i++)
            putU16LE(buf[i]);
        return *this;
    }

    this_type& writeU32LE(uint32_t *buf, size_t n)
    {
        for(size_t i = 0; i < n; i++)
            putU32LE(buf[i]);
        return *this;
    }
#else
    this_type& writeU16BE(uint16_t *buf, size_t n)
    {
        for(size_t i = 0; i < n; i++)
            putU16BE(buf[i]);
        return *this;
    }

    this_type& writeU32BE(uint32_t *buf, size_t n)
    {
        for(size_t i = 0; i < n; i++)
            putU32BE(buf[i]);
        return *this;
    }

    this_type& writeU16LE(uint16_t *buf, size_t n)
    {
        return reinterpret_cast<this_type&>(this->write(reinterpret_cast<char*>(buf), n*sizeof(buf[0])));
    }

    this_type& writeU32LE(uint32_t *buf, size_t n)
    {
        return reinterpret_cast<this_type&>(this->write(reinterpret_cast<char*>(buf), n*sizeof(buf[0])));
    }
#endif
};

typedef basic_arcostream<char>  arcostream;
typedef basic_arcostream<char>  OStream;

}

#endif // EASTWOOD_OSTREAM_H

