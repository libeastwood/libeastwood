#ifndef EASTWOOD_PYTHON_PYMAPFILE_H
#define EASTWOOD_PYTHON_PYMAPFILE_H

#include <istream>

#include "eastwood/MapFile.h"

extern PyTypeObject MapFile_Type;

struct Py_MapFile {
    PyObject_HEAD
    std::istream *stream;
    eastwood::MapFile *mapFile;
    uint16_t size;
};

#endif // EASTWOOD_PYTHON_PYMAPFILE_H
