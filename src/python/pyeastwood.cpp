#include "pyeastwood.h"
#include "ShpFile.h"
#include "Surface.h"

#include "pycpsfile.h"
#include "pyicnfile.h"
#include "pymapfile.h"
#include "pypakfile.h"
#include "pypalfile.h"
#include "pyshpfile.h"
#include "pystringfile.h"
#include "pysurface.h"

using namespace eastwood;

static const char __author__[] =
"The eastwood python module was written by:\n\
\n\
    Per Øyvind Karlsen <peroyvind@mandriva.org>\n\
";

static PyMethodDef pyeastwood_methods[] = {
    {0, 0, 0, 0}
};

PyMODINIT_FUNC
initpyeastwood(void)
{
    PyObject *module;

    if (PyType_Ready(&CpsFile_Type) < 0)
	return;

    if (PyType_Ready(&IcnFile_Type) < 0)
	return;

    if (PyType_Ready(&MapFile_Type) < 0)
	return;

    if (PyType_Ready(&PakFile_Type) < 0)
	return;

    if (PyType_Ready(&PalFile_Type) < 0)
	return;

    if (PyType_Ready(&ShpFile_Type) < 0)
	return;

    if (PyType_Ready(&StringFile_Type) < 0)
	return;

    if (PyType_Ready(&Surface_Type) < 0)
	return;

    module = Py_InitModule("pyeastwood", pyeastwood_methods);
    if (module == NULL)
	return;

    Py_INCREF(&CpsFile_Type);
    PyModule_AddObject(module, "CpsFile", (PyObject *)&CpsFile_Type);

    Py_INCREF(&IcnFile_Type);
    PyModule_AddObject(module, "IcnFile", (PyObject *)&IcnFile_Type);

    Py_INCREF(&MapFile_Type);
    PyModule_AddObject(module, "MapFile", (PyObject *)&MapFile_Type);

    Py_INCREF(&PakFile_Type);
    PyModule_AddObject(module, "PakFile", (PyObject *)&PakFile_Type);

    Py_INCREF(&PalFile_Type);
    PyModule_AddObject(module, "PalFile", (PyObject *)&PalFile_Type);

    Py_INCREF(&ShpFile_Type);
    PyModule_AddObject(module, "ShpFile", (PyObject *)&ShpFile_Type);

    Py_INCREF(&StringFile_Type);
    PyModule_AddObject(module, "StringFile", (PyObject *)&StringFile_Type);

    Py_INCREF(&Surface_Type);
    PyModule_AddObject(module, "Surface", (PyObject *)&Surface_Type);

    PyModule_AddObject(module, "__author__", PyString_FromString(__author__));


    PyModule_AddIntConstant(module, "Scale2X", Scale2X);
    PyModule_AddIntConstant(module, "Scale2X3", Scale2X3);
    PyModule_AddIntConstant(module, "Scale2X4", Scale2X4);
    PyModule_AddIntConstant(module, "Scale3X", Scale3X);
    PyModule_AddIntConstant(module, "Scale4X", Scale4X);

    PyModule_AddIntConstant(module, "TILE_NORMAL", TILE_NORMAL);
    PyModule_AddIntConstant(module, "TILE_FLIPH", TILE_FLIPH);
    PyModule_AddIntConstant(module, "TILE_FLIPV", TILE_FLIPV);
    PyModule_AddIntConstant(module, "TILE_ROTATE", TILE_ROTATE);

}

