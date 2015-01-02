#include "eastwood/ArchiveManager.h"
#include "eastwood/Exception.h"
#include "eastwood/Log.h"
#include "eastwood/BlowFish.h"
#include "eastwood/WsKey.h"

namespace eastwood {

const uint32_t ENCRYPTED = 0x00020000;
    
size_t ArchiveManager::indexDir(std::string& path)
{
    struct dirent* dir;
    DIR *dp = opendir(path.c_str());
    struct stat st;
    t_arc_entry info;
    std::pair<t_arc_index_iter,bool> rv;
    
    //check we managed to open a directory path
    if (dp == NULL) throw(Exception(LOG_ERROR, "ArchiveManager", "Could not open directory"));
    
    //initialise a fresh map object
    _archives.push_back(t_arc_index());
    
    while ((dir = readdir(dp)) != NULL) {
        std::string path;
        
        //ignore standard dirs
        if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0)
            continue;
        
        //check we can stat the current file
        path = path + DIR_SEP + dir->d_name;
        if (stat(path.c_str(), &st) < 0) {
            //TODO error and warnings
            LOG_WARNING("Couldn't stat %s", path.c_str());
            continue;
        }
        
        //ignore if a directory of any kind, sub dirs not supported
        if (S_ISDIR(st.st_mode)) continue;
        
        info.first = idGen(dir->d_name);
        info.second.archivepath = path;
        info.second.size = st.st_size;
        info.second.start = 0;
        
        rv = _archives.back().insert(info);
        
        //if insertion failed, assume it was due to id collision
        if(!rv.second) {
            LOG_WARNING("Could not index %s, likely a hash collision", dir->d_name);
        }
    }
    
    closedir(dp);
    
    return _archives.size() - 1;
}

size_t ArchiveManager::indexPak(std::string& pakfile, bool usefind)
{
    return _archives.size() - 1;
}

size_t ArchiveManager::indexMix(std::string& mixfile, bool usefind)
{
    uint32_t flags;
    uint16_t filecount;
    std::string archivename;
    ArcFileInfo info;
    std::pair<t_arc_index_iter,bool> rv;
    
    if(usefind) {
        info = find(mixfile);
        _stream.open(info);
    } else {
        _stream.open(mixfile.c_str(), std::ios_base::binary | std::ios_base::in);
        info.archivepath = mixfile;
        info.start = 0;
        info.size = _stream.sizeg();
    }
    
    if(!_stream.is_open())
        throw(Exception(LOG_ERROR, "ArchiveManager", "Could not open Mix"));
    
    flags = _stream.getU32LE();
    filecount = *reinterpret_cast<uint16_t*>(&flags);
    
    if(filecount || !(flags & ENCRYPTED)){
        handleUnEncrypted(info, filecount);
    } else {
        handleEncrypted(info);
    }
    
    return _archives.size() - 1;
}

void ArchiveManager::handleUnEncrypted(ArcFileInfo& archive, uint16_t filecount)
{
    uint32_t offset = 6;  //at least 6 at this point
    t_arc_entry info;
    std::pair<t_arc_index_iter,bool> rv;
    
    _archives.push_back(t_arc_index());
    
    //This ignores the body size and does no validation as the games do.
    if(filecount){
        _stream.seekg(6, std::ios_base::beg);
        LOG_DEBUG("Indexing TD format mix");
    } else {
        filecount = _stream.getU16LE();
        offset += 4;
        _stream.seekg(10, std::ios_base::beg);
        LOG_DEBUG("Indexing RA format mix");
    }
    
    //add total size of index to offset
    offset += 12 * filecount;
    
    for(uint32_t i = 0; i < filecount; i++) {
        info.first = _stream.getU32LE();
        info.second.start = _stream.getU32LE() + offset + archive.start;
        info.second.size = _stream.getU32LE();
        info.second.archivepath = archive.archivepath;
        rv = _archives.back().insert(info);
        
        //if insertion failed, assume bad format.
        if(!rv.second)
            throw(Exception(LOG_ERROR, "ArchiveManager", "Invalid Mix format"));
    }
    
}

void ArchiveManager::handleEncrypted(ArcFileInfo& archive)
{
    uint32_t offset = 84 + archive.start;     //at least 84 at this point
    uint32_t filecount;
    Cblowfish bfish;
    int8_t bfishbuf[8];
    uint8_t keysource[80];
    uint8_t key[56];
    uint32_t bcount;
    t_arc_entry info;
    std::pair<t_arc_index_iter,bool> rv;
    
    
    //setup blowfish to handle header
    _stream.seekg(4, std::ios_base::beg);
    _stream.read(reinterpret_cast<char*>(keysource), 80);
    get_blowfish_key(keysource, key);
    bfish.set_key(key, 56);
    
    //read first block to get file count, needed to calculate header size
    _stream.read(reinterpret_cast<char*>(bfishbuf), 8);
    bfish.decipher(reinterpret_cast<void*>(bfishbuf), 
                    reinterpret_cast<void*>(bfishbuf), 8);
    memcpy(reinterpret_cast<char*>(&filecount), bfishbuf, 2);
    
    //caculate header size and prepare buffer for it
    bcount = ((filecount * 12) - 2) / 8;
    if (((filecount * 12) - 2) % 8) bcount++;
    //add 8 to compensate for block we already decrypted
    offset += bcount * 8 + 8;
    
    //prepare buffer for index
    char pindbuf[bcount * 8 + 2];
    memcpy(pindbuf, bfishbuf + 6 , 2);
    
    //loop to decrypt index into index buffer
    for(int i = 0; i < bcount; i++) {
        _stream.read(reinterpret_cast<char*>(bfishbuf), 8);
        bfish.decipher(reinterpret_cast<void*>(bfishbuf), 
                    reinterpret_cast<void*>(bfishbuf), 8);
        memcpy(pindbuf + 2 + 8 * i, bfishbuf, 8);
    }
    
    //get ready to create index
    _archives.push_back(t_arc_index());
    
    for(uint32_t i = 0; i < filecount; i++){
        memcpy(reinterpret_cast<char*>(&info.first), pindbuf + i * 12,
               sizeof(int32_t));
        memcpy(reinterpret_cast<char*>(&info.second.start), 
               pindbuf + 4 + i * 12, sizeof(int32_t));
        memcpy(reinterpret_cast<char*>(&info.second.size), 
               pindbuf + 8 + i * 12, sizeof(int32_t));
        
        info.second.start += offset;
        info.second.archivepath = archive.archivepath;
        rv = _archives.back().insert(info);
        
        //if insertion failed, assume bad format.
        if(!rv.second)
            throw(Exception(LOG_ERROR, "ArchiveManager", "Invalid Mix format"));
    }
}
    
//Generates the ID's found in mix files.
int32_t ArchiveManager::idGen(std::string filename)
{
    std::transform(filename.begin(), filename.end(), filename.begin(),
            (int(*)(int)) toupper); // convert to uppercase
    int i = 0;
    uint32_t id = 0;
    int l = filename.length(); // length of the filename
    while (i < l) {
        uint32_t a = 0;
        for (int j = 0; j < 4; j++) {
            a >>= 8;
            if (i < l)
                a += static_cast<uint32_t> (filename[i]) << 24;
            i++;
        }
        id = (id << 1 | id >> 31) + a;
    }
    return id;
}

}//eastwood