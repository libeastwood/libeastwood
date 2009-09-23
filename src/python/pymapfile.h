#ifndef EASTWOOD_PYTHON_PYMAPFILE_H
#define EASTWOOD_PYTHON_PYMAPFILE_H

#include "eastwood/MapFile.h"

extern PyTypeObject MapFile_Type;

struct Py_MapFile {
    PyObject_HEAD
    eastwood::MapFile *mapFile;
    uint16_t size;
};

#endif // EASTWOOD_PYTHON_PYMAPFILE_H
