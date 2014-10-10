#include "eastwood/CnCStrFile.h"
#include "eastwood/Log.h"
//#include <sys/stat.h>
//#include <sys/types.h>

namespace eastwood {

CCStringFile::CCStringFile(std::istream &stream) :
_strings(0)
{
    IStream &_stream = reinterpret_cast<IStream&>(stream);
    int count;
    std::vector<uint16_t> offsets;
    offsets.push_back(_stream.getU16LE());
    count = (offsets.back() / 2) - 1;
    
    while(count--) {
        offsets.push_back(_stream.getU16LE());
    }
    
    for(uint32_t i = 0; i < offsets.size(); i++){
        _strings.push_back(readString(_stream, offsets[i]));
    }
}

void CCStringFile::list()
{
    for(uint32_t i = 0; i < _strings.size(); i++){
        std::string str = getString(i);
        printf("%d: %s\n", i, str.c_str());
    }
}

std::string CCStringFile::readString(IStream& _stream, int offset)
{
    std::string buf;
    char c;
    
    while(c != '\0') {
        c = _stream.get();
        buf + c;
    }
    
    LOG_DEBUG("Read %s as string", buf.c_str());
    
    return std::string(buf);
}

}//eastwood