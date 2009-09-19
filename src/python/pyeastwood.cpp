#include "pyeastwood.h"
#include "Surface.h"

#include "cpsfile.h"
#include "pakfile.h"
#include "palfile.h"
#include "surface.h"

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

    if (PyType_Ready(&PakFile_Type) < 0)
	return;

    if (PyType_Ready(&PalFile_Type) < 0)
	return;

    if (PyType_Ready(&Surface_Type) < 0)
	return;

    module = Py_InitModule("pyeastwood", pyeastwood_methods);
    if (module == NULL)
	return;

    Py_INCREF(&CpsFile_Type);
    PyModule_AddObject(module, "CpsFile", (PyObject *)&CpsFile_Type);

    Py_INCREF(&PakFile_Type);
    PyModule_AddObject(module, "PakFile", (PyObject *)&PakFile_Type);

    Py_INCREF(&PalFile_Type);
    PyModule_AddObject(module, "PalFile", (PyObject *)&PalFile_Type);

    Py_INCREF(&Surface_Type);
    PyModule_AddObject(module, "Surface", (PyObject *)&Surface_Type);

    PyModule_AddObject(module, "__author__", PyString_FromString(__author__));


    PyModule_AddIntConstant(module, "Scale2X", Scale2X);
    PyModule_AddIntConstant(module, "Scale2X3", Scale2X3);
    PyModule_AddIntConstant(module, "Scale2X4", Scale2X4);
    PyModule_AddIntConstant(module, "Scale3X", Scale3X);
    PyModule_AddIntConstant(module, "Scale4X", Scale4X);

}
