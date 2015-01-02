#ifndef EASTWOOD_ISTREAM_H
#define EASTWOOD_ISTREAM_H

//#include <istream>
#include "ArcStreamBuf.h"

#include "eastwood/StdDef.h"

namespace eastwood {

template < typename CharType, typename CharTraits = std::char_traits <CharType> >

class basic_arcistream : public std::basic_istream <CharType, CharTraits>
{
public:
    typedef CharType                                      char_type;
    typedef CharTraits                                    traits_type;

    typedef basic_arcstream         <char_type, traits_type>  sbuf_type;
    typedef std::basic_streambuf    <char_type, traits_type>  bbuf_type;
    typedef basic_arcistream        <char_type, traits_type>  this_type;
    typedef std::basic_istream      <char_type, traits_type>  base_type;
    
    basic_arcistream(FILE* fp = NULL) :
    base_type(new sbuf_type(fp))
    {}
    
    basic_arcistream(bbuf_type* sb) :
    base_type(reinterpret_cast<sbuf_type*>(sb))
    {}
    
    basic_arcistream(const base_type& sb) :
    base_type(reinterpret_cast<sbuf_type*>(sb.rdbuf()))
    {}
    
    basic_arcistream(const char* filename, std::ios_base::openmode mode) :
    base_type((new sbuf_type)->open(filename, mode))
    {}
    
    basic_arcistream(ArcFileInfo& fileinfo) :
    base_type(new sbuf_type(fileinfo))
    {}
    
    void open(const char* filename, std::ios_base::openmode mode = std::ios_base::in) 
    {
        sbuf_type* buf = static_cast<sbuf_type*>(this->rdbuf());
        if (!(buf->open(filename, mode)))
        this->setstate(std::ios_base::badbit);
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
    
    std::streamsize sizeg()
    {
        sbuf_type* buf = static_cast<sbuf_type*>(this->rdbuf());
        return static_cast<std::streamsize>(buf->size());
    }
    
private:
    template <typename T> inline
    this_type& readT(T &value) 
    {
        return reinterpret_cast<this_type&>(this->read(reinterpret_cast<char*>(&value), sizeof(value)));
    }

    template <typename T> inline
    T getT() 
    {
        T value;
        readT<T>(value);
        return value;
    }
public:
    uint16_t getU16BE() 
    {
        return htobe16(getT<uint16_t>());
    }

    uint16_t getU16LE() 
    {
        return htole16(getT<uint16_t>());
    }

    uint32_t getU32BE() 
    {
        return htobe32(getT<uint32_t>());
    }

    uint32_t getU32LE() 
    {
        return htole32(getT<uint32_t>());
    }
    
#if __BYTE_ORDER == __BIG_ENDIAN
    this_type& readU16BE(uint16_t *buf, size_t n)
    {
        return reinterpret_cast<this_type&>(this->read((char*)buf, n*sizeof(buf[0])));
    }

    this_type& readU32BE(uint32_t *buf, size_t n)
    {
        return reinterpret_cast<this_type&>(this->read((char*)buf, n*sizeof(buf[0])));
    }

    this_type& readU16LE(uint16_t *buf, size_t n)
    {
        for(size_t i = 0; i < n; i++)
            buf[i] = getU16LE();
        return *this;
    }

    this_type& readU32LE(uint32_t *buf, size_t n)
    {
        for(size_t i = 0; i < n; i++)
            buf[i] = getU32LE();
        return *this;
    }
#else
    this_type& readU16BE(uint16_t *buf, size_t n)
    {
        for(size_t i = 0; i < n; i++)
            buf[i] = getU16BE();
        return *this;
    }

    this_type& readU32BE(uint32_t *buf, size_t n)
    {
        for(size_t i = 0; i < n; i++)
            buf[i] = getU32BE();
        return *this;
    }

    this_type& readU16LE(uint16_t *buf, size_t n)
    {
        return reinterpret_cast<this_type&>(this->read(reinterpret_cast<char*>(buf), n*sizeof(buf[0])));
    }

    this_type& readU32LE(uint32_t *buf, size_t n)
    {
        return reinterpret_cast<this_type&>(this->read(reinterpret_cast<char*>(buf), n*sizeof(buf[0])));
    }
#endif
};

typedef basic_arcistream<char>  arcistream;
typedef basic_arcistream<char>  IStream;

}

#endif // EASTWOOD_ISTREAM_H
