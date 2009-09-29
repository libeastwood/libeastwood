#ifndef EASTWOOD_PYTHON_PYEMCFILE_H
#define EASTWOOD_PYTHON_PYEMCFILE_H

#include <istream>
#include <ostream>
#include "eastwood/EmcFileBase.h"

extern PyTypeObject EmcFile_Type;

enum EmcMode {
    EMC_ASSEMBLE,
    EMC_DISASSEMBLE,
    EMC_INVALID    
};

struct Py_EmcFile {
    PyObject_HEAD
    std::istream *input;
    std::ostream *output;
    eastwood::EmcFileBase *emcFile;
    size_t size;
    EmcMode mode;
    PyObject *type;
};

#endif // EASTWOOD_PYTHON_PYEMCFILE_H
