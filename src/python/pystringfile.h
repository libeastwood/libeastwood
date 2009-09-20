#ifndef EASTWOOD_PYTHON_PYSTRINGFILE_H
#define EASTWOOD_PYTHON_PYSTRINGFILE_H

#include <istream>
#include "StringFile.h"

extern PyTypeObject StringFile_Type;

struct Py_StringFile {
    PyObject_HEAD
    eastwood::StringFile *stringFile;
    uint16_t size;
};

#endif // EASTWOOD_PYTHON_PYSTRINGFILE_H
