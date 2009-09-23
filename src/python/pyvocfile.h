#ifndef EASTWOOD_PYTHON_PYVOCFILE_H
#define EASTWOOD_PYTHON_PYVOCFILE_H

#include <istream>
#include "eastwood/VocFile.h"

extern PyTypeObject VocFile_Type;

struct Py_VocFile {
    PyObject_HEAD
    std::istream *stream;
    eastwood::VocFile *vocFile;
    PyObject *palFile;
};

#endif // EASTWOOD_PYTHON_PYVOCFILE_H
