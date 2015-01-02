#ifndef EASTWOOD_ARCHIVEMANAGER_H
#define	EASTWOOD_ARCHIVEMANAGER_H

#include "StdDef.h"
#include "IOStream.h"
#include "DirEnt.h"
#include <vector>
#include <map>

typedef std::map<int32_t, ArcFileInfo> t_arc_index;
typedef std::pair<int32_t, ArcFileInfo> t_arc_entry;
typedef std::map<int32_t, ArcFileInfo>::iterator t_arc_index_iter;

namespace eastwood {

class ArchiveManager
{
public:
    ArchiveManager() : _archives(0), _stream(), _nullinfo() {}
    size_t indexDir(std::string path);
    size_t indexPak(std::string pakfile, bool usefind = false);
    size_t indexMix(std::string mixfile, bool usefind = false);
    ArcFileInfo& find(std::string filename);
    int size() { return _archives.size(); }
    bool empty() { return _archives.empty(); }
    
private:
    int32_t idGen(std::string filename);
    void handleEncrypted(ArcFileInfo& archive);
    void handleUnEncrypted(ArcFileInfo& archive, uint16_t filecount);
    std::vector<t_arc_index> _archives;
    IOStream _stream;
    ArcFileInfo _nullinfo;
};

}//eastwood

#endif	/* EASTWOOD_ARCHIVEMANAGER_H */

