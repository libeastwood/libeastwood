#include "pyeastwood.h"
#include "eastwood/StdDef.h"
#include "eastwood/ShpFile.h"
#include "eastwood/Surface.h"

#include "pycpsfile.h"
#include "pyemcfile.h"
#include "pyicnfile.h"
#include "pymapfile.h"
#include "pypakfile.h"
#include "pypalfile.h"
#include "pyshpfile.h"
#include "pysound.h"
#include "pystringfile.h"
#include "pysurface.h"
#include "pyvocfile.h"
#include "pywsafile.h"

using namespace eastwood;

static const char __author__[] =
"The eastwood python module was written by:\n\
\n\
    Per Øyvind Karlsen <proyvind@moondrake.org>\n\
";

PyMODINIT_FUNC
initpyeastwood(void)
{
    PyObject *module;

    if((PyType_Ready(&CpsFile_Type) < 0) || (PyType_Ready(&EmcFile_Type) < 0)
	    || (PyType_Ready(&IcnFile_Type) < 0)
	    || (PyType_Ready(&MapFile_Type) < 0) || (PyType_Ready(&PakFile_Type) < 0)
	    || (PyType_Ready(&PalFile_Type) < 0) || (PyType_Ready(&ShpFile_Type) < 0)
	    || (PyType_Ready(&Sound_Type) < 0) || (PyType_Ready(&StringFile_Type) < 0)
	    || (PyType_Ready(&Surface_Type) < 0) || (PyType_Ready(&VocFile_Type) < 0)
	    || (PyType_Ready(&WsaFile_Type) < 0))

	return;    

    module = Py_InitModule("pyeastwood", nullptr);
    if (module == nullptr)
	return;

    Py_INCREF(&CpsFile_Type);
    PyModule_AddObject(module, "CpsFile", (PyObject *)&CpsFile_Type);

    Py_INCREF(&EmcFile_Type);
    PyModule_AddObject(module, "EmcFile", (PyObject *)&EmcFile_Type);

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

    Py_INCREF(&Sound_Type);
    PyModule_AddObject(module, "Sound", (PyObject *)&Sound_Type);
    
    Py_INCREF(&StringFile_Type);
    PyModule_AddObject(module, "StringFile", (PyObject *)&StringFile_Type);

    Py_INCREF(&Surface_Type);
    PyModule_AddObject(module, "Surface", (PyObject *)&Surface_Type);

    Py_INCREF(&VocFile_Type);
    PyModule_AddObject(module, "VocFile", (PyObject *)&VocFile_Type);

    Py_INCREF(&WsaFile_Type);
    PyModule_AddObject(module, "WsaFile", (PyObject *)&WsaFile_Type);

    PyModule_AddObject(module, "__author__", PyString_FromString(__author__));
    PyModule_AddStringConstant(module, "__version__", VERSION);

    PyModule_AddIntConstant(module, "Scale2X", Scale2X);
    PyModule_AddIntConstant(module, "Scale2X3", Scale2X3);
    PyModule_AddIntConstant(module, "Scale2X4", Scale2X4);
    PyModule_AddIntConstant(module, "Scale3X", Scale3X);
    PyModule_AddIntConstant(module, "Scale4X", Scale4X);

    PyModule_AddIntConstant(module, "TILE_NORMAL", TILE_NORMAL);
    PyModule_AddIntConstant(module, "TILE_FLIPH", TILE_FLIPH);
    PyModule_AddIntConstant(module, "TILE_FLIPV", TILE_FLIPV);
    PyModule_AddIntConstant(module, "TILE_ROTATE", TILE_ROTATE);

    PyModule_AddIntConstant(module, "MISSION_DESCRIPTION", MISSION_DESCRIPTION);
    PyModule_AddIntConstant(module, "MISSION_WIN", MISSION_WIN);
    PyModule_AddIntConstant(module, "MISSION_LOSE", MISSION_LOSE);
    PyModule_AddIntConstant(module, "MISSION_ADVICE", MISSION_ADVICE);

    PyModule_AddIntConstant(module, "I_SINC_BEST_QUALITY", I_SINC_BEST_QUALITY);
    PyModule_AddIntConstant(module, "I_SINC_MEDIUM_QUALITY", I_SINC_MEDIUM_QUALITY);
    PyModule_AddIntConstant(module, "I_SINC_FASTEST", I_SINC_FASTEST);
    PyModule_AddIntConstant(module, "I_ZERO_ORDER_HOLD", I_ZERO_ORDER_HOLD);
    PyModule_AddIntConstant(module, "I_LINEAR", I_LINEAR);    

    PyModule_AddIntConstant(module, "FMT_U8", FMT_U8);
    PyModule_AddIntConstant(module, "FMT_S8", FMT_S8);
    PyModule_AddIntConstant(module, "FMT_U16LE", FMT_U16LE);
    PyModule_AddIntConstant(module, "FMT_S16LE", FMT_S16LE);
    PyModule_AddIntConstant(module, "FMT_U16BE", FMT_U16BE);
    PyModule_AddIntConstant(module, "FMT_S16BE", FMT_S16BE);

}
