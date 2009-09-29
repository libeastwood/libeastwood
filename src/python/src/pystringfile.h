#ifndef EASTWOOD_PYTHON_PYSTRINGFILE_H
#define EASTWOOD_PYTHON_PYSTRINGFILE_H

#include <istream>
#include "eastwood/StringFile.h"

extern PyTypeObject StringFile_Type;

struct Py_StringFile {
    PyObject_HEAD
    std::istream *stream;
    eastwood::StringFile *stringFile;
    uint16_t size;
};

#endif // EASTWOOD_PYTHON_PYSTRINGFILE_H
