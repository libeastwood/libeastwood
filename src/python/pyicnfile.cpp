// TODO: This is *very* crude for now, need to figure out some neat way for
// 	 python to deal with C++ streams ...
#include <istream>
#include <sstream>
#include "pyeastwood.h"
#include <structmember.h>

#include "StdDef.h"
#include "IcnFile.h"
#include "PalFile.h"

#include "pyicnfile.h"
#include "pymapfile.h"
#include "pypalfile.h"
#include "pysurface.h"

using namespace eastwood;

static int
IcnFile_init(Py_IcnFile *self, PyObject *args)
{
    Py_buffer pdata;
    if (!PyArg_ParseTuple(args, "s*OO", &pdata, &self->mapFile, &self->palFile))
	return -1;

    self->stream = new std::istream(new std::stringbuf(std::string(reinterpret_cast<char*>(pdata.buf), pdata.len)));
    if(!self->stream->good()) {
	PyErr_SetFromErrno(PyExc_IOError);
	PyBuffer_Release(&pdata);
    	return -1;
    }
    if(!PyObject_TypeCheck(self->mapFile, &MapFile_Type)) {
	PyErr_SetString(PyExc_TypeError, "Second argument must be a MapFile object");
	return -1;
    }

    if(!PyObject_TypeCheck(self->palFile, &PalFile_Type)) {
	PyErr_SetString(PyExc_TypeError, "Third argument must be a PalFile object");
	return -1;
    }

    self->icnFile = new IcnFile(*self->stream, *((Py_MapFile*)self->mapFile)->mapFile, ((Py_PalFile*)self->palFile)->palette);
    self->size = self->icnFile->size();

    Py_INCREF(self->mapFile);
    Py_INCREF(self->palFile);

    PyBuffer_Release(&pdata);
    return 0;
}

static void
IcnFile_dealloc(Py_IcnFile *self)
{
    delete self->icnFile;
    delete self->stream;
    Py_XDECREF(self->palFile);
    Py_XDECREF(self->mapFile);
}

static PyObject *
IcnFile_getSurface(Py_IcnFile *self, PyObject *args)
{
    uint16_t index;
    if (!PyArg_ParseTuple(args, "H", &index))
	return NULL;

    Surface *surface = self->icnFile->getSurface(index);
    PyObject *pysurface = Surface_Type.tp_new(&Surface_Type, reinterpret_cast<PyObject*>(surface), NULL);
    return pysurface;
}

static PyObject *
IcnFile_getTiles(Py_IcnFile *self, PyObject *args)
{
    bool frameByFrame = false;
    uint16_t index = 0;
    PyObject *pysurface = NULL;
    Surface *tiles = NULL;
    if (!PyArg_ParseTuple(args, "HB", &index, &frameByFrame))
	return NULL;

    tiles = self->icnFile->getTiles(index, frameByFrame);
    pysurface = Surface_Type.tp_new(&Surface_Type, reinterpret_cast<PyObject*>(tiles), NULL);
    return pysurface;
}

static PyMethodDef IcnFile_methods[] = {
    {"getSurface", (PyCFunction)IcnFile_getSurface, METH_VARARGS, NULL},
    {"getTiles", (PyCFunction)IcnFile_getTiles, METH_VARARGS, NULL},
    {NULL, NULL, 0, NULL}		/* sentinel */
};

static PyMemberDef IcnFile_members[] = {
    {const_cast<char*>("size"), T_USHORT, offsetof(Py_IcnFile, size), RO, NULL},
    {NULL, 0, 0, 0, NULL}
};

PyTypeObject IcnFile_Type = {
    PyObject_HEAD_INIT(NULL)
    0,						/*ob_size*/
    "pyeastwood.IcnFile",			/*tp_name*/
    sizeof(Py_IcnFile),				/*tp_basicsize*/
    0,						/*tp_itemsize*/
    (destructor)IcnFile_dealloc,		/*tp_dealloc*/
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
    IcnFile_methods,				/*tp_methods*/
    IcnFile_members,				/*tp_members*/
    0,						/*tp_getset*/
    0,                      			/*tp_base*/
    0,                      			/*tp_dict*/
    0,                      			/*tp_descr_get*/
    0,                      			/*tp_descr_set*/
    0,                      			/*tp_dictoffset*/
    (initproc)IcnFile_init,			/*tp_init*/
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
