#ifndef EASTWOOD_PYTHON_PALFILE_H
#define EASTWOOD_PYTHON_PALFILE_H

#ifdef __cplusplus
#include <istream>
#include "PalFile.h"

struct Py_PalFile {
	PyObject_HEAD
	std::istream *stream;
	eastwood::PalFile *palFile;
};

#endif

extern PyTypeObject PalFile_Type;

#endif // EASTWOOD_PYTHON_PALFILE_H
