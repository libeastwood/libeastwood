#include <fstream>
#include "StdDef.h"

#include "Dune2File.h"

namespace eastwood {

Dune2Version detectDune2Version(std::istream &stream)
{
    int i;
    for(i = D2_DEMO; i < D2_VERSIONS; i++) {
	stream.seekg(versionOffset[i], std::ios::beg);
	uint8_t tmp[4];
	readLE(stream, tmp, sizeof(tmp));
	if(memcmp(tmp, "DUNE", sizeof(tmp)) == 0)
	    break;
    }
    return (Dune2Version)i;
}

}
