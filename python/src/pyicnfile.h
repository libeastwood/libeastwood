#ifndef EASTWOOD_PYTHON_PYICNFILE_H
#define EASTWOOD_PYTHON_PYICNFILE_H

#include <istream>

#include "eastwood/IcnFile.h"

extern PyTypeObject IcnFile_Type;

struct Py_IcnFile {
    PyObject_HEAD
    std::istream *stream;
    eastwood::IcnFile *icnFile;
    PyObject *mapFile;
    uint16_t size;
};

#endif // EASTWOOD_PYTHON_PYICNFILE_H
