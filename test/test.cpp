#include "eastwood/IStream.h"
#include "eastwood/ArchiveManager.h"
#include "eastwood/Log.h"
#include "eastwood/IniFile.h"
#include "eastwood/StdDef.h"
#include "eastwood/Surface.h"
#include "eastwood/CpsFile.h"
#include "eastwood/FntFile.h"
#include "eastwood/StringFile.h"
#include "eastwood/OStream.h"

const char* mixes[] = {"tdtest.mix", "ratest.mix", "rasub.mix"};
const char* strfiles[] = {"sole.eng", "conquer.eng", "setup.dip", "redalert.eng"};
const char* inifile[] = { "redalert.ini" };

using namespace eastwood;

int main(int argc, char** argv)
{
    ArchiveManager arcman;
    arcman.indexDir(".");
    arcman.indexMix("ratest.mix", true);
    arcman.indexMix("rasub.mix", true);
    //IStream file(arcman.find("setup.dip"));
    IStream ini;
    ini.open("REDALERT.INI", std::ios_base::in | std::ios_base::binary);
    if(ini.is_open()){
        IniFile inifile(ini);
        LOG_INFO("Ini language %s.", inifile.getStringValue("Language", "Language").c_str());
        inifile.setStringValue("Language", "Language", "GER");
        OStream iniwrite;
        iniwrite.open("testing.ini");
        if(iniwrite.is_open()){
            LOG_INFO("Out stream is open");
            inifile.SaveChangesTo(iniwrite);
        }
    } else {
        LOG_DEBUG("File not open");
    }
    ini.close();
    
    
    //file.open(finfo);
    //StringFile strf(file2);
    //strf.list();
    
    return 0;
}
