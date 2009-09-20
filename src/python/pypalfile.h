#ifndef EASTWOOD_PYTHON_PYPALFILE_H
#define EASTWOOD_PYTHON_PYPALFILE_H

#include "PalFile.h"

struct Py_PalFile {
	PyObject_HEAD
	eastwood::PalFile *palFile;
	eastwood::Palette *palette;
};

extern PyTypeObject PalFile_Type;

#endif // EASTWOOD_PYTHON_PYPALFILE_H
