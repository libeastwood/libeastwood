#include "eastwood/ArcStream.h"
#include "eastwood/Log.h"
#include "eastwood/IniFile.h"
#include "eastwood/StdDef.h"
#include "eastwood/Surface.h"
#include "eastwood/CpsFile.h"
#include "eastwood/FntFile.h"
#include "eastwood/CnCStrFile.h"
#include <istream>
#include <fstream>

const char* mixes[] = {"tdtest.mix", "ratest.mix", "rasub.mix"};
const char* strfiles[] = {"conquer.eng", "setup.dip", "redalert.eng"};
const char* inifile[] = { "redalert.ini" };

using namespace eastwood;

int main(int argc, char** argv)
{
    ArcStream file;
    file.open("sole.eng", std::ios_base::in | std::ios_base::binary);
    std::istream stream(&file);
    CCStringFile strf(stream);
    LOG_INFO("String %d is %s.", 121, strf.getString(120).c_str());
    
    return 0;
}
