#include <iostream>
#include "eastwood/StdDef.h"
#include "eastwood/Exception.h"
#include "eastwood/StringFile.h"

namespace eastwood {

StringFile::StringFile(std::istream& stream)
    : _stream(reinterpret_cast<IStream&>(stream)),
    _strings(0), _compressed(true)
{
    //LOG_DEBUG("We are at pos %d", _stream.tellg());
    //FIXME: This is a "bit" poor detection...
    if(_stream.peek() == 0x2)
        _compressed = false;
    else
        _strings.resize((_stream.getU16LE()/2)-1);
    
    readHeader();
}

StringFile::~StringFile()
{
}

void StringFile::readHeader()
{
    if(_compressed) {
        std::vector<uint16_t> offsets(_strings.size());

        for(auto &offset : offsets)
            offset = _stream.getU16LE();

        auto i = 0;
        for(auto &string : _strings)
            string = std::move(decodeString(offsets[i++]));
    }
    else {
        _strings.emplace_back("");
        while(!_stream.eof()) {
            auto byte = _stream.get();
            //TODO: Figure out what these non-alpha numeric characters are for,
            //      some sort of formatting perhaps? Let's just treat them as
            //      separators for now... 
            if(byte >= 0x20 && byte <= 0x7e)
                _strings.back() += byte;
            else if(_strings.back().size() != 0)
                    _strings.emplace_back("");
        }
    }
}

std::string StringFile::decodeString(uint16_t offset)
{
    std::string out = "";
    uint8_t databyte;

    while(static_cast<uint32_t>(_stream.tellg()) < offset) {
        databyte = _stream.get();

        switch(databyte) {
            case 0x00: break;
            case 0x04: break;
            case 0x03: break;
            case 0x0C: out += "\n"; break;
            case 0x0D: out += "\n"; break;

            case 0x80: out += " t"; break;
            case 0x81: out += " a"; break;
            case 0x82: out += " s"; break;
            case 0x83: out += " i"; break;
            case 0x84: out += " o"; break;
            case 0x85: out += "  "; break; // might be something else
            case 0x86: out += " w"; break;
            case 0x87: out += " b"; break;
            case 0x88: out += "e "; break;
            case 0x89: out += "er"; break;
            case 0x8A: out += "en"; break;
            case 0x8B: out += "es"; break;
            case 0x8C: out += "ed"; break;
            case 0x8D: out += "ea"; break;
            case 0x8E: out += "el"; break;
            case 0x8F: out += "em"; break;

            case 0x90: out += "th"; break;
            case 0x91: out += "t "; break;
            case 0x92: out += "ti"; break;
            case 0x93: out += "te"; break;
            case 0x94: out += "to"; break;
            case 0x95: out += "tr"; break;
            case 0x96: out += "ta"; break;
            case 0x97: out += "ts"; break;
            case 0x98: out += "an"; break;
            case 0x99: out += "ar"; break;
            case 0x9A: out += "at"; break;
            case 0x9B: out += "al"; break;
            case 0x9C: out += "ac"; break;
            case 0x9D: out += "a "; break;
            case 0x9E: out += "as"; break;
            case 0x9F: out += "ay"; break;

            case 0xA0: out += "in"; break;
            case 0xA1: out += "is"; break;
            case 0xA2: out += "it"; break;
            case 0xA3: out += "ic"; break;
            case 0xA4: out += "il"; break;
            case 0xA5: out += "io"; break;
            case 0xA6: out += "ie"; break;
            case 0xA7: out += "ir"; break;
            case 0xA8: out += "n "; break;
            case 0xA9: out += "nd"; break;
            case 0xAA: out += "nt"; break;
            case 0xAB: out += "ng"; break;
            case 0xAC: out += "ne"; break;
            case 0xAD: out += "ns"; break;
            case 0xAE: out += "ni"; break;
            case 0xAF: out += "no"; break;

            case 0xB0: out += "on"; break;
            case 0xB1: out += "or"; break;
            case 0xB2: out += "o "; break;
            case 0xB3: out += "ou"; break;
            case 0xB4: out += "of"; break;
            case 0xB5: out += "om"; break;
            case 0xB6: out += "os"; break;
            case 0xB7: out += "ow"; break;
            case 0xB8: out += "s "; break;
            case 0xB9: out += "st"; break;
            case 0xBA: out += "se"; break;
            case 0xBB: out += "sp"; break;
            case 0xBC: out += "s."; break;
            case 0xBD: out += "si"; break;
            case 0xBE: out += "sc"; break;
            case 0xBF: out += "sa"; break;

            case 0xC0: out += "re"; break;
            case 0xC1: out += "r "; break;
            case 0xC2: out += "ro"; break;
            case 0xC3: out += "ri"; break;
            case 0xC4: out += "ra"; break;
            case 0xC5: out += "rd"; break;
            case 0xC6: out += "ru"; break;
            case 0xC7: out += "rr"; break;
            case 0xC8: out += "l "; break;
            case 0xC9: out += "ll"; break;
            case 0xCA: out += "la"; break;
            case 0xCB: out += "le"; break;
            case 0xCC: out += "li"; break;
            case 0xCD: out += "le"; break;
            case 0xCE: out += "lo"; break;
            case 0xCF: out += "ld"; break;

            case 0xD0: out += "he"; break;
            case 0xD1: out += "hi"; break;
            case 0xD2: out += "ha"; break;
            case 0xD3: out += "h "; break;
            case 0xD4: out += "ho"; break;
            case 0xD5: out += "ht"; break;
            case 0xD6: out += "hr"; break;
            case 0xD7: out += "hu"; break;
            case 0xD8: out += "ce"; break;
            case 0xD9: out += "ct"; break;
            case 0xDA: out += "co"; break;
            case 0xDB: out += "ca"; break;
            case 0xDC: out += "ck"; break;
            case 0xDD: out += "ch"; break;
            case 0xDE: out += "cl"; break;
            case 0xDF: out += "cr"; break;

            case 0xE0: out += "d "; break;
            case 0xE1: out += "de"; break;
            case 0xE2: out += "di"; break;
            case 0xE3: out += "du"; break;
            case 0xE4: out += "d,"; break;
            case 0xE5: out += "d."; break;
            case 0xE6: out += "do"; break;
            case 0xE7: out += "da"; break;
            case 0xE8: out += "un"; break;
            case 0xE9: out += "us"; break;
            case 0xEA: out += "ur"; break;
            case 0xEB: out += "uc"; break;
            case 0xEC: out += "ut"; break;
            case 0xED: out += "ul"; break;
            case 0xEE: out += "ua"; break;
            case 0xEF: out += "ui"; break;

            case 0xF0: out += "pl"; break;
            case 0xF1: out += "pe"; break;
            case 0xF2: out += "po"; break;
            case 0xF3: out += "pi"; break;
            case 0xF4: out += "pr"; break;
            case 0xF5: out += "pa"; break;
            case 0xF6: out += "pt"; break;
            case 0xF7: out += "pp"; break;
            case 0xF8: out += "me"; break;
            case 0xF9: out += "ma"; break;
            case 0xFA: out += "mo"; break;
            case 0xFB: out += "mi"; break;
            case 0xFC: out += "mp"; break;
            case 0xFD: out += "m "; break;
            case 0xFE: out += "mb"; break;
            case 0xFF: out += "mm"; break;

            case 0x1B: {
                           // special character
                           if(static_cast<uint32_t>(_stream.tellg()) == offset)
                               throw (Exception(LOG_ERROR, "StringFile", "decodeString: Special character escape sequence at end of string!"));

                           uint8_t special = _stream.get();
                           switch(special) {
                               // e.g. german "umlaute"
                               case 0x02: out += static_cast<uint8_t>(252) /*"ue"*/; break;
                               case 0x05: out += static_cast<uint8_t>(228) /*"ae"*/; break;
                               case 0x0F: out += static_cast<uint8_t>(197) /*"Ae"*/; break;
                               case 0x15: out += static_cast<uint8_t>(246) /*"oe"*/; break;
                               case 0x1B: out += static_cast<uint8_t>(220) /*"Ue"*/; break;
                               case 0x62: out += static_cast<uint8_t>(223) /*"ss"*/; break;
                               default: {
                                            char tmp[20];
                                            sprintf(tmp,"---Unknown char:%X---",special);
                                            out += tmp;
                                        } break;
                           };

                       } break;

            case 0x1F: out += "."; break;

            default: {
                         if((databyte & 0x80) == 0x80) {
                             char tmp[20];
                             sprintf(tmp,"---Unknown char:%X---",databyte);
                             out += tmp;
                         } else {
                             out += databyte;
                         }
                     } break;
        }
    }

    return out;
}

void StringFile::list()
{
    auto i = 0;
    for(auto &text : _strings)
        std::cout << i++ << ":" << text << std::endl;
}

}
// vim:ts=8:sw=4:et
