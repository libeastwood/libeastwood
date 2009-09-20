#ifndef EASTWOOD_PYTHON_PYSOUND_H
#define EASTWOOD_PYTHON_PYSOUND_H

#include "Sound.h"

struct Py_Sound {
    PyObject_HEAD
    eastwood::Sound *sound;
    PyObject *size;
};

extern PyTypeObject Sound_Type;

#endif // EASTWOOD_PYTHON_PYSOUND_H
