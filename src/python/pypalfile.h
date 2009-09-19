#ifndef EASTWOOD_PYTHON_PYPALFILE_H
#define EASTWOOD_PYTHON_PYPALFILE_H

#include <istream>
#include "PalFile.h"

struct Py_PalFile {
	PyObject_HEAD
	std::istream *stream;
	eastwood::PalFile *palFile;
};

extern PyTypeObject PalFile_Type;

#endif // EASTWOOD_PYTHON_PYPALFILE_H
