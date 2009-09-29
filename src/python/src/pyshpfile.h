#ifndef EASTWOOD_PYTHON_PYSHPFILE_H
#define EASTWOOD_PYTHON_PYSHPFILE_H

#include <istream>

#include "eastwood/ShpFile.h"

extern PyTypeObject ShpFile_Type;

struct Py_ShpFile {
    PyObject_HEAD
    std::istream *stream;
    eastwood::ShpFile *shpFile;
    uint16_t size;
};

#endif // EASTWOOD_PYTHON_PYSHPFILE_H
