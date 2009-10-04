#ifndef EASTWOOD_PYTHON_PYPAKFILE_H
#define EASTWOOD_PYTHON_PYPAKFILE_H

#include <fstream>

#include "eastwood/PakFile.h"
#include "pyeastwood.h"

extern PyTypeObject PakFile_Type;

struct Py_PakFile {
    PyObject_HEAD
    std::fstream *stream;
    eastwood::PakFile *pakFile;
    PyObject *pakFileName;
    std::ios_base::openmode mode;
#ifdef WITH_THREAD
    PyThread_type_lock lock;
#endif
};

#endif // EASTWOOD_PYTHON_PYPAKFILE_H
