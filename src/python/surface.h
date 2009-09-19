#ifndef EASTWOOD_PYTHON_SURFACE_H
#define EASTWOOD_PYTHON_SURFACE_H

#include <istream>
#include "PalFile.h"

struct Py_Surface {
    PyObject_HEAD
    eastwood::Surface *surface;
};

extern PyTypeObject Surface_Type;

#endif // EASTWOOD_PYTHON_SURFACE_H
