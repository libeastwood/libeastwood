#include <fstream>
#include "StdDef.h"

#include "Dune2File.h"

namespace eastwood {

Dune2File::Dune2File(std::istream &stream) : _stream(stream), _version(D2_VERSIONS)
{
    detectDune2Version();
}

Dune2Version Dune2File::detectDune2Version()
{
    int i;
    for(i = D2_DEMO; i < D2_VERSIONS; i++) {
	_stream.seekg(versionOffset[i], std::ios::beg);
	uint8_t tmp[4];
	readLE(_stream, tmp, sizeof(tmp));
	if(memcmp(tmp, "DUNE", sizeof(tmp)) == 0)
	    break;
    }
    return (Dune2Version)i;
}

}
