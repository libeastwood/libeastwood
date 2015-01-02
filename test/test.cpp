#include "eastwood/ArcStream.h"
#include "eastwood/Log.h"
#include "eastwood/IniFile.h"
#include "eastwood/StdDef.h"
#include "eastwood/Surface.h"
#include "eastwood/CpsFile.h"
#include "eastwood/FntFile.h"
#include "eastwood/StringFile.h"
#include <istream>
#include <fstream>

const char* mixes[] = {"tdtest.mix", "ratest.mix", "rasub.mix"};
const char* strfiles[] = {"conquer.eng", "setup.dip", "redalert.eng"};
const char* inifile[] = { "redalert.ini" };

using namespace eastwood;

int main(int argc, char** argv)
{
    ArcStream file("sole.eng");
    std::istream stream(&file);
    StringFile strf(stream);
    LOG_INFO("String %d is %s.", 120, strf.getString(120).c_str());
    
    return 0;
}
