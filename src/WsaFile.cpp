#include <vector>
#include <stdexcept>
#include <iostream>

#include "eastwood/StdDef.h"

#include "eastwood/WsaFile.h"
#include "eastwood/PalFile.h"

#include "eastwood/Exception.h"
#include "eastwood/Log.h"

namespace eastwood {

WsaFile::WsaFile(std::istream &stream, Palette palette,
	Surface firstFrame) :
    Decode(stream, 0, 0, palette), _frameOffsTable(0),
    _decodedFrames(0), _deltaBufferSize(0), _framesPer1024ms(0)
{
    uint32_t frameDataOffs = 0;
    bool newformat;

    _decodedFrames.resize(_stream.getU16LE());
    LOG_DEBUG("WsaFile numframes = %d", _decodedFrames.size());

    _width = _stream.getU16LE();
    _height = _stream.getU16LE();
    
    //These shorts will be 0 if a new format wsa for x and y pos
    if(!_width && !_height){
        //following shorts are actual dimensions
        _width = _stream.getU16LE();
        _height = _stream.getU16LE();
        newformat = true;
    }
    
    LOG_DEBUG("WsaFile size %d x %d", _width, _height);
    
    if(newformat){
        _deltaBufferSize = _stream.getU32LE();
        
        _frameOffsTable.resize(_decodedFrames.size()+2);
        for (uint32_t i = 0; i < _frameOffsTable.size(); ++i) {
            _frameOffsTable[i] = _stream.getU32LE();
            if (_frameOffsTable[i])
                _frameOffsTable[i] += 768; //offset to account for palette;
        }
        
    } else {
        
        _deltaBufferSize = _stream.getU16LE();
        
        // "Regular" WSA files shipped with the Dune 2 demo version does not have
        // 2 bytes padding here...
        if(_stream.getU16LE())
            _stream.seekg(-2, std::ios::cur);

        frameDataOffs = _stream.getU16LE();
        // "Continue" WSA files shipped with the Dune 2 demo version does not have
        // 2 bytes padding here...
        if(_stream.getU16LE())
            _stream.seekg(-2, std::ios::cur);
        
        if (frameDataOffs == 0) {
            frameDataOffs = _stream.getU32LE();
            _decodedFrames.pop_back();
        }
    _frameOffsTable.resize(_decodedFrames.size()+2);
    for (auto i = 1; i < _frameOffsTable.size(); ++i) {
	_frameOffsTable[i] = _stream.getU32LE() + offsetadjust;
	if (_frameOffsTable[i])
	    _frameOffsTable[i] -= frameDataOffs;
        }
    }
    
    _framesPer1024ms = _deltaBufferSize / 1024.0f;

    LOG_DEBUG("WsaFile _framesPer1024ms = %d", _framesPer1024ms);

    if(newformat){
        PalFile pal(_stream);
        _palette = pal.getPalette();
    }

    _decodedFrames.front() = firstFrame ? firstFrame : Surface(_width, _height, 8, _palette);

    decodeFrames();
}

WsaFile::~WsaFile()
{
}

void WsaFile::decodeFrames()
{
    std::vector<uint8_t> dec80(_decodedFrames.front().size());
    Surface *pic = nullptr;

    for(std::vector<Surface>::iterator it = _decodedFrames.begin();
	    it != _decodedFrames.end(); pic = &(*it), ++it) {
	if(pic)
	    *it = Surface(*pic);
	decode80(&dec80.front(), 0);
	decode40(&dec80.front(), *it);

    }
}

}
