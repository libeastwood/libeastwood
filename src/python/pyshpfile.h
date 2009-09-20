#ifndef EASTWOOD_PYTHON_PYSHPFILE_H
#define EASTWOOD_PYTHON_PYSHPFILE_H

#include <istream>
#include "ShpFile.h"

extern PyTypeObject ShpFile_Type;

struct Py_ShpFile {
    PyObject_HEAD
    std::istream *stream;
    eastwood::ShpFile *shpFile;
    PyObject *palFile;
    uint16_t size;
};

#endif // EASTWOOD_PYTHON_PYSHPFILE_H
