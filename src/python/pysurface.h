#ifndef EASTWOOD_PYTHON_PYSURFACE_H
#define EASTWOOD_PYTHON_PYSURFACE_H

#include "PalFile.h"

struct Py_Surface {
    PyObject_HEAD
    eastwood::Surface *surface;
    PyObject *size;
};

extern PyTypeObject Surface_Type;

#endif // EASTWOOD_PYTHON_PYSURFACE_H
