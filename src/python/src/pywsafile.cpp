// TODO: This is *very* crude for now, need to figure out some neat way for
// 	 python to deal with C++ streams ...
#include <istream>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include "pyeastwood.h"
#include <structmember.h>

#include "eastwood/StdDef.h"
#include "eastwood/WsaFile.h"
#include "eastwood/Palette.h"

#include "pywsafile.h"
#include "pypalette.h"
#include "pysurface.h"

using namespace eastwood;

PyDoc_STRVAR(WsaFile_init__doc__,
"WsaFile(data, palette [, firstframe]) -> WsaFile object\n\
\n\
Creates a WsaFile from data using palette specified.\n\
If lastframe argument is given, it will act as the first frame\n\
");

static int
WsaFile_init(Py_WsaFile *self, PyObject *args)
{
    Py_buffer pdata;
    PyObject *palObject = NULL, *frameObject = NULL;
    Surface firstFrame;
    if (!PyArg_ParseTuple(args, "s*O|O", &pdata, &palObject, &frameObject))
	return -1;

    self->stream = new std::istream(new std::stringbuf(std::string(reinterpret_cast<char*>(pdata.buf), pdata.len)));
    if(!self->stream->good()) {
	PyErr_SetFromErrno(PyExc_IOError);
	goto error;
    }
    if(!PyObject_TypeCheck(palObject, &Palette_Type)) {
	PyErr_SetString(PyExc_TypeError, "Second argument must be a Palette object");
	return -1;
    }
    if(frameObject) {
	if(!PyObject_TypeCheck(frameObject, &Surface_Type)) {
    	    PyErr_SetString(PyExc_TypeError, "If given, third argument must be a Surface object");
    	    return -1;
	}
	firstFrame = *((Py_Surface*)frameObject)->surface;
    }	

    self->wsaFile = new WsaFile(*self->stream, *((Py_Palette*)palObject)->palette, firstFrame);
    self->size = self->wsaFile->size();
    self->fpms = self->wsaFile->getFramesPer1024ms();

    PyBuffer_Release(&pdata);
    return 0;

error:
    PyBuffer_Release(&pdata);
    return -1;
}

static PyObject *
WsaFile_alloc(PyTypeObject *type, Py_ssize_t nitems)
{
    Py_WsaFile *self = (Py_WsaFile *)PyType_GenericAlloc(type, nitems);
    self->wsaFile = NULL;
    self->stream = NULL;
    self->size = 0;
    self->fpms = 0;

    return (PyObject *)self;
}

static void
WsaFile_dealloc(Py_WsaFile *self)
{
    if(self->wsaFile)
    	delete self->wsaFile;
    if(self->stream) {
	delete self->stream->rdbuf();
    	delete self->stream;
    }
    PyObject_Del((PyObject*)self);
}

PyDoc_STRVAR(WsaFile_getSurface__doc__,
"getSurface(index) -> Surface object\n\
\n\
Returns a Surface object at index.\n\
");

static PyObject *
WsaFile_getSurface(Py_WsaFile *self, PyObject *args)
{
    uint16_t index;
    Surface *surface = NULL;
    if (!PyArg_ParseTuple(args, "H", &index))
	return NULL;

    try {
    	surface = new Surface(self->wsaFile->getSurface(index));
    } catch(std::out_of_range e) {
	PyErr_SetString(PyExc_IndexError, "WsaFile index out of range");
	return NULL;
    }
    PyObject *pysurface = Surface_Type.tp_new(&Surface_Type, reinterpret_cast<PyObject*>(surface), NULL);
    return pysurface;
}

static PyMethodDef WsaFile_methods[] = {
    {"getSurface", (PyCFunction)WsaFile_getSurface, METH_VARARGS, WsaFile_getSurface__doc__},
    {NULL, NULL, 0, NULL}		/* sentinel */
};

static PyMemberDef WsaFile_members[] = {
    {const_cast<char*>("size"), T_USHORT, offsetof(Py_WsaFile, size), RO, NULL},
    {const_cast<char*>("fpms"), T_UINT, offsetof(Py_WsaFile, size), RO, NULL},    
    {NULL, 0, 0, 0, NULL}
};

PyTypeObject WsaFile_Type = {
    PyObject_HEAD_INIT(NULL)
    0,						/*ob_size*/
    "pyeastwood.WsaFile",			/*tp_name*/
    sizeof(Py_WsaFile),				/*tp_basicsize*/
    0,						/*tp_itemsize*/
    (destructor)WsaFile_dealloc,		/*tp_dealloc*/
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
    WsaFile_init__doc__,			/*tp_doc*/
    0,						/*tp_traverse*/
    0,						/*tp_clear*/
    0,						/*tp_richcompare*/
    0,						/*tp_weaklistoffset*/
    0,						/*tp_iter*/
    0,						/*tp_iternext*/
    WsaFile_methods,				/*tp_methods*/
    WsaFile_members,				/*tp_members*/
    0,						/*tp_getset*/
    0,                      			/*tp_base*/
    0,                      			/*tp_dict*/
    0,                      			/*tp_descr_get*/
    0,                      			/*tp_descr_set*/
    0,                      			/*tp_dictoffset*/
    (initproc)WsaFile_init,			/*tp_init*/
    WsaFile_alloc,	    			/*tp_alloc*/
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
