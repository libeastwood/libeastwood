#ifndef EASTWOOD_ARCSTREAM_H
#define	EASTWOOD_ARCSTREAM_H

#include <streambuf>
#include <vector>

namespace eastwood {
    
class ArcStream : public std::streambuf
{
public:
    ArcStream(int fd, int size, int offset = 0, std::size_t buff_sz = 256, std::size_t put_back = 8);
    ~ArcStream();
protected:
    underflow();
    int _descriptor;
    int _soffset;
    int _eoffset;
    const std::size_t put_back_;
    std::vector<char> buffer_;
};
    
}//eastwood

#endif	/* EASTWOOD_ARCSTREAM_H */

