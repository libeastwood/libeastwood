#ifndef EASTWOOD_PYTHON_PYPALFILE_H
#define EASTWOOD_PYTHON_PYPALFILE_H

#include "eastwood/PalFile.h"

struct Py_PalFile {
    PyObject_HEAD
    eastwood::PalFile *palFile;
};

extern PyTypeObject PalFile_Type;

#endif // EASTWOOD_PYTHON_PYPALFILE_H
