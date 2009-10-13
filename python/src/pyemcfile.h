#ifndef EASTWOOD_PYTHON_PYEMCFILE_H
#define EASTWOOD_PYTHON_PYEMCFILE_H

#include <istream>
#include <ostream>
#include "eastwood/EmcFileBase.h"

extern PyTypeObject EmcFile_Type;

struct Py_EmcFile {
    PyObject_HEAD
    std::istream *input;
    std::ostream *output;
    eastwood::EmcFileBase *emcFile;
    char mode;
    PyObject *type;
};

#endif // EASTWOOD_PYTHON_PYEMCFILE_H
