#ifndef EASTWOOD_PYTHON_PYSURFACE_H
#define EASTWOOD_PYTHON_PYSURFACE_H

#include "eastwood/Surface.h"

struct Py_Surface {
    PyObject_HEAD
    eastwood::Surface *surface;
    uint16_t width,
	     height;
};

extern PyTypeObject Surface_Type;

#endif // EASTWOOD_PYTHON_PYSURFACE_H
