#ifndef EASTWOOD_PYTHON_PYPAKFILE_H
#define EASTWOOD_PYTHON_PYPAKFILE_H

#include <istream>
#include "PakFile.h"

extern PyTypeObject PakFile_Type;

enum file_mode {
    MODE_CLOSED		= 0,
    MODE_READ		= 1,
    MODE_READ_EOF	= 2
};

struct Py_PakFile {
    PyObject_HEAD
    std::ifstream *stream;
    eastwood::PakFile *pakFile;
    uint32_t fileSize;
    file_mode mode;
};

#endif // EASTWOOD_PYTHON_PYPAKFILE_H
