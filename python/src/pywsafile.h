#ifndef EASTWOOD_PYTHON_PYWSAFILE_H
#define EASTWOOD_PYTHON_PYWSAFILE_H

#include <istream>

#include "eastwood/WsaFile.h"

extern PyTypeObject WsaFile_Type;

struct Py_WsaFile {
    PyObject_HEAD
    std::istream *stream;
    eastwood::WsaFile *wsaFile;
    uint16_t size;
    uint32_t fpms;
};

#endif // EASTWOOD_PYTHON_PYWSAFILE_H
