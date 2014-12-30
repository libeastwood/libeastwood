// TODO: This is *very* crude for now, need to figure out some neat way for
// 	 python to deal with C++ streams ...
#include <istream>
#include <sstream>
#include "pyeastwood.h"

#include "eastwood/StdDef.h"
#include "eastwood/CpsFile.h"
#include "eastwood/Exception.h"
#include "eastwood/Palette.h"

#include "pycpsfile.h"
#include "pypalette.h"
#include "pysurface.h"

using namespace eastwood;

PyDoc_STRVAR(CpsFile_init__doc__,
"CpsFile(data [, palette]) -> CpsFile object\n\
\n\
Creates a CpsFile from data using palette specified.\n\
If no palette specified, try using embedded palette if present.\n\
");

static int
CpsFile_init(Py_CpsFile *self, PyObject *args)
{
    Py_buffer pdata;
    PyObject *palObject = nullptr;
    Palette palette(0);
    if (!PyArg_ParseTuple(args, "s*|O", &pdata, &palObject))
	return -1;

    self->stream = new std::istream(new std::stringbuf(std::string(reinterpret_cast<char*>(pdata.buf), pdata.len)));
    if(!self->stream->good()) {
	PyErr_SetFromErrno(PyExc_IOError);
	goto error;
    }
    if(palObject) {
    	if(!PyObject_TypeCheck(palObject, &Palette_Type)) {
	    PyErr_SetString(PyExc_TypeError, "If given, second argument must be a Palette object");
	    goto error;
 	}
	palette = *((Py_Palette*)palObject)->palette;
    }

    try {
    	self->cpsFile = new CpsFile(*self->stream, palette);
    } catch(Exception e) {
	PyErr_Format(PyExc_Exception, "%s: %s", e.getLocation().c_str(), e.getMessage().c_str());
	goto error;
    }

    PyBuffer_Release(&pdata);
    return 0;

error:
    PyBuffer_Release(&pdata);
    return -1;
}

static PyObject *
CpsFile_alloc(PyTypeObject *type, Py_ssize_t nitems)
{
    Py_CpsFile *self = (Py_CpsFile *)PyType_GenericAlloc(type, nitems);
    self->cpsFile = nullptr;
    self->stream = nullptr;

    return (PyObject *)self;
}

static void
CpsFile_dealloc(Py_CpsFile *self)
{
    if(self->cpsFile)
    	delete self->cpsFile;
    if(self->stream) {
	delete self->stream->rdbuf();
    	delete self->stream;
    }
    PyObject_Del((PyObject*)self);
}

PyDoc_STRVAR(CpsFile_getSurface__doc__,
"getSurface() -> Surface object\n\
\n\
Returns a Surface object.\n\
");

static PyObject *
CpsFile_getSurface(Py_CpsFile *self)
{
    Surface *surface;
    try {
       surface = new Surface(self->cpsFile->getSurface());
    } catch(Exception e) {
	PyErr_Format(PyExc_Exception, "%s: %s", e.getLocation().c_str(), e.getMessage().c_str());
	return nullptr;
    }
    PyObject *pysurface = Surface_Type.tp_new(&Surface_Type, reinterpret_cast<PyObject*>(surface), nullptr);
    return pysurface;
}

static PyMethodDef CpsFile_methods[] = {
    {"getSurface", (PyCFunction)CpsFile_getSurface, METH_NOARGS, CpsFile_getSurface__doc__},
    {nullptr, nullptr, 0, nullptr}		/* sentinel */
};


PyTypeObject CpsFile_Type = {
    PyObject_HEAD_INIT(nullptr)
    0,						/*ob_size*/
    "pyeastwood.CpsFile",			/*tp_name*/
    sizeof(Py_CpsFile),				/*tp_basicsize*/
    0,						/*tp_itemsize*/
    (destructor)CpsFile_dealloc,		/*tp_dealloc*/
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
    CpsFile_init__doc__,			/*tp_doc*/
    0,						/*tp_traverse*/
    0,						/*tp_clear*/
    0,						/*tp_richcompare*/
    0,						/*tp_weaklistoffset*/
    0,						/*tp_iter*/
    0,						/*tp_iternext*/
    CpsFile_methods,				/*tp_methods*/
    0,						/*tp_members*/
    0,						/*tp_getset*/
    0,                      			/*tp_base*/
    0,                      			/*tp_dict*/
    0,                      			/*tp_descr_get*/
    0,                      			/*tp_descr_set*/
    0,                      			/*tp_dictoffset*/
    (initproc)CpsFile_init,			/*tp_init*/
    CpsFile_alloc,    				/*tp_alloc*/
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
