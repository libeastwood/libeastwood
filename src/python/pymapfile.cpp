// TODO: This is *very* crude for now, need to figure out some neat way for
// 	 python to deal with C++ streams ...
#include <istream>
#include <sstream>
#include "pyeastwood.h"
#include <structmember.h>

#include "StdDef.h"
#include "MapFile.h"
#include "PalFile.h"

#include "pymapfile.h"
#include "pypalfile.h"
#include "pysurface.h"

using namespace eastwood;

struct Py_MapFile {
    PyObject_HEAD
    MapFile *mapFile;
    uint16_t size;
};

static int
MapFile_init(Py_MapFile *self, PyObject *args)
{
    Py_buffer pdata;
    if (!PyArg_ParseTuple(args, "s*", &pdata))
	return -1;

    std::istream stream(new std::stringbuf(std::string(reinterpret_cast<char*>(pdata.buf), pdata.len)));
    if(!stream.good()) {
	PyErr_SetFromErrno(PyExc_IOError);
	PyBuffer_Release(&pdata);
    	return -1;
    }

    self->mapFile = new MapFile(stream);
    self->size = self->mapFile->size();

    PyBuffer_Release(&pdata);
    return 0;
}

static void
MapFile_dealloc(Py_MapFile *self)
{
    delete self->mapFile;
}

static PyMemberDef MapFile_members[] = {
    {const_cast<char*>("size"), T_USHORT, offsetof(Py_MapFile, size), RO, NULL},
    {NULL, 0, 0, 0, NULL}
};

PyTypeObject MapFile_Type = {
    PyObject_HEAD_INIT(NULL)
    0,						/*ob_size*/
    "pyeastwood.MapFile",			/*tp_name*/
    sizeof(Py_MapFile),				/*tp_basicsize*/
    0,						/*tp_itemsize*/
    (destructor)MapFile_dealloc,		/*tp_dealloc*/
    0,						/*tp_print*/
    0,						/*tp_getattr*/
    0,						/*tp_setattr*/
    0,						/*tp_compare*/
    0,						/*tp_repr*/
    0,						/*tp_as_number*/
    0,						/*tp_as_sequence*/
    0,						/*tp_as_mapping*/
    0,						/*tp_hash*/
    0,						/*tp_call*/
    0,						/*tp_str*/
    PyObject_GenericGetAttr,			/*tp_getattro*/
    PyObject_GenericSetAttr,			/*tp_setattro*/
    0,						/*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE,	/*tp_flags*/
    0,						/*tp_doc*/
    0,						/*tp_traverse*/
    0,						/*tp_clear*/
    0,						/*tp_richcompare*/
    0,						/*tp_weaklistoffset*/
    0,						/*tp_iter*/
    0,						/*tp_iternext*/
    0,						/*tp_methods*/
    MapFile_members,				/*tp_members*/
    0,						/*tp_getset*/
    0,                      			/*tp_base*/
    0,                      			/*tp_dict*/
    0,                      			/*tp_descr_get*/
    0,                      			/*tp_descr_set*/
    0,                      			/*tp_dictoffset*/
    (initproc)MapFile_init,			/*tp_init*/
    PyType_GenericAlloc,    			/*tp_alloc*/
    PyType_GenericNew,	      			/*tp_new*/
    0,		          			/*tp_free*/
    0,                      			/*tp_is_gc*/
    0,						/*tp_bases*/
    0,						/*tp_mro*/
    0,						/*tp_cache*/
    0,						/*tp_subclasses*/
    0,						/*tp_weaklist*/
    0,						/*tp_del*/
    0						/*tp_version_tag*/
};
