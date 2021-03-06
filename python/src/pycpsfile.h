#ifndef EASTWOOD_PYTHON_PYCPSFILE_H
#define EASTWOOD_PYTHON_PYCPSFILE_H

#include <istream>

#include "eastwood/CpsFile.h"

extern PyTypeObject CpsFile_Type;

struct Py_CpsFile {
    PyObject_HEAD
    std::istream *stream;
    eastwood::CpsFile *cpsFile;
};

#endif // EASTWOOD_PYTHON_PYCPSFILE_H
