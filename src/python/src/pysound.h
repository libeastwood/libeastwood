#ifndef EASTWOOD_PYTHON_PYSOUND_H
#define EASTWOOD_PYTHON_PYSOUND_H

#include "eastwood/Sound.h"

struct Py_Sound {
    PyObject_HEAD
    eastwood::Sound *sound;
    uint8_t channels;
    uint32_t frequency;
    eastwood::AudioFormat format;
};

extern PyTypeObject Sound_Type;

#endif // EASTWOOD_PYTHON_PYSOUND_H
