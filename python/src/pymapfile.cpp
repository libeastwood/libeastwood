// TODO: This is *very* crude for now, need to figure out some neat way for
// 	 python to deal with C++ streams ...
#include <istream>
#include <sstream>
#include "pyeastwood.h"
#include <structmember.h>

#include "eastwood/StdDef.h"
#include "eastwood/MapFile.h"

#include "pymapfile.h"
#include "pysurface.h"

using namespace eastwood;

PyDoc_STRVAR(MapFile_init__doc__,
"MapFile(data) -> MapFile object\n\
\n\
Creates a MapFile from data.\n\
");

static int
MapFile_init(Py_MapFile *self, PyObject *args)
{
    Py_buffer pdata;
    uint16_t size;
    if (!PyArg_ParseTuple(args, "s*", &pdata))
	return -1;

    self->stream = new std::istream(new std::stringbuf(std::string(reinterpret_cast<char*>(pdata.buf), pdata.len)));
    if(!self->stream->good()) {
	PyErr_SetFromErrno(PyExc_IOError);
	PyBuffer_Release(&pdata);
    	return -1;
    }

    self->mapFile = new MapFile(*self->stream);
    size = self->mapFile->size();
    self->tuple = PyTuple_New(size);
    for(uint16_t i = 0; i < size; i++) {
    	const std::vector<uint16_t> &mapRow = (*self->mapFile)[i];
	PyObject *row = PyTuple_New(mapRow.size());
	for(uint16_t j = 0; j < mapRow.size(); j++)
	    if(PyTuple_SetItem(row, j, Py_BuildValue("H", mapRow[j])))
		return -1;
	if(PyTuple_SetItem(self->tuple, i, row))
	    return -1;
    }

    PyBuffer_Release(&pdata);
    return 0;
}

static PyObject *
MapFile_alloc(PyTypeObject *type, Py_ssize_t nitems)
{
    Py_MapFile *self = (Py_MapFile *)PyType_GenericAlloc(type, nitems);
    self->stream = NULL;
    self->mapFile = NULL;
    self->tuple = NULL;

    return (PyObject *)self;
}

static void
MapFile_dealloc(Py_MapFile *self)
{
    if(self->mapFile)
    	delete self->mapFile;
    if(self->stream) {
	delete self->stream->rdbuf();
    	delete self->stream;
    }
    Py_XDECREF(self->tuple);
    Py_TYPE(self)->tp_free((PyObject *)self);
}

static PyObject*
MapFile_subscript(Py_MapFile *self, PyObject *item)
{
    return PyTuple_Type.tp_as_mapping->mp_subscript(self->tuple, item);
}

static Py_ssize_t
MapFile_length(Py_MapFile *a)
{
    return PyTuple_Type.tp_as_mapping->mp_length(a->tuple);
}

static PyMappingMethods MapFile_as_mapping = {
    (lenfunc)MapFile_length,
    (binaryfunc)MapFile_subscript,
    0
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
    &MapFile_as_mapping,			/*tp_as_mapping*/
    0,						/*tp_hash*/
    0,						/*tp_call*/
    0,						/*tp_str*/
    PyObject_GenericGetAttr,			/*tp_getattro*/
    PyObject_GenericSetAttr,			/*tp_setattro*/
    0,						/*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE,	/*tp_flags*/
    MapFile_init__doc__,			/*tp_doc*/
    0,						/*tp_traverse*/
    0,						/*tp_clear*/
    0,						/*tp_richcompare*/
    0,						/*tp_weaklistoffset*/
    0,						/*tp_iter*/
    0,						/*tp_iternext*/
    0,						/*tp_methods*/
    0,						/*tp_members*/
    0,						/*tp_getset*/
    0,                      			/*tp_base*/
    0,                      			/*tp_dict*/
    0,                      			/*tp_descr_get*/
    0,                      			/*tp_descr_set*/
    0,                      			/*tp_dictoffset*/
    (initproc)MapFile_init,			/*tp_init*/
    MapFile_alloc,	    			/*tp_alloc*/
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
