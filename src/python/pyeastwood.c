#include "pyeastwood.h"
#include "cpsfile.h"
#include "pakfile.h"
#include "palfile.h"

static const char __author__[] =
"The eastwood python module was written by:\n\
\n\
    Per Øyvind Karlsen <peroyvind@mandriva.org>\n\
";

static PyMethodDef pyeastwood_methods[] = {
    {0, 0, 0, 0}
};

/* declare function before defining it to avoid compile warnings */
PyMODINIT_FUNC initpyeastwood(void);
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

    module = Py_InitModule("pyeastwood", pyeastwood_methods);
    if (module == NULL)
	return;

    Py_INCREF(&CpsFile_Type);
    PyModule_AddObject(module, "cpsfile", (PyObject *)&CpsFile_Type);

    Py_INCREF(&PakFile_Type);
    PyModule_AddObject(module, "pakfile", (PyObject *)&PakFile_Type);

    Py_INCREF(&PalFile_Type);
    PyModule_AddObject(module, "palfile", (PyObject *)&PalFile_Type);

    PyModule_AddObject(module, "__author__", PyString_FromString(__author__));

}

