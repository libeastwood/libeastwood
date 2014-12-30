// TODO: This is *very* crude for now, need to figure out some neat way for
// 	 python to deal with C++ streams ...
#include <istream>
#include <sstream>
#include <stdexcept>
#include "pyeastwood.h"
#include <structmember.h>

#include "eastwood/StdDef.h"
#include "eastwood/IcnFile.h"
#include "eastwood/Exception.h"
#include "eastwood/Palette.h"

#include "pyicnfile.h"
#include "pymapfile.h"
#include "pypalette.h"
#include "pysurface.h"

using namespace eastwood;

PyDoc_STRVAR(IcnFile_init__doc__,
"IcnFile(data, map, palette) -> IcnFile object\n\
\n\
Creates a IcnFile from data using map & palette specified.\n\
");

static int
IcnFile_init(Py_IcnFile *self, PyObject *args)
{
    Py_buffer pdata;
    PyObject *palObject = nullptr;
    if (!PyArg_ParseTuple(args, "s*|OO", &pdata, &palObject, &self->mapFile))
	return -1;

    self->stream = new std::istream(new std::stringbuf(std::string(reinterpret_cast<char*>(pdata.buf), pdata.len)));
    if(!self->stream->good()) {
	PyErr_SetFromErrno(PyExc_IOError);
	goto error;
    }

    if(palObject && !PyObject_TypeCheck(palObject, &Palette_Type)) {
	PyErr_SetString(PyExc_TypeError, "Second argument must be a Palette object");
	goto error;
    }

    if(self->mapFile && !PyObject_TypeCheck(self->mapFile, &MapFile_Type)) {
	PyErr_SetString(PyExc_TypeError, "Third argument must be a MapFile object");
	goto error;
    }

    try {
	if(self->mapFile)
    	    self->icnFile = new IcnFile(*self->stream, *((Py_Palette*)palObject)->palette, *((Py_MapFile*)self->mapFile)->mapFile);
	else if(palObject)
    	    self->icnFile = new IcnFile(*self->stream, *((Py_Palette*)palObject)->palette);
	else
    	    self->icnFile = new IcnFile(*self->stream);
    } catch(Exception e) {
	PyErr_Format(PyExc_Exception, "%s: %s", e.getLocation().c_str(), e.getMessage().c_str());
	goto error;
    }

    self->size = self->icnFile->size();

    if(self->mapFile)
    	Py_INCREF(self->mapFile);

    PyBuffer_Release(&pdata);
    return 0;
    
error:
    PyBuffer_Release(&pdata);
    return -1;
}

static PyObject *
IcnFile_alloc(PyTypeObject *type, Py_ssize_t nitems)
{
    Py_IcnFile *self = (Py_IcnFile *)PyType_GenericAlloc(type, nitems);
    self->stream = nullptr;
    self->icnFile = nullptr;
    self->mapFile = nullptr;

    return (PyObject *)self;
}

static void
IcnFile_dealloc(Py_IcnFile *self)
{
    if(self->icnFile)
	delete self->icnFile;
    if(self->stream) {
	delete self->stream->rdbuf();
    	delete self->stream;
    }
    Py_XDECREF(self->mapFile);
    PyObject_Del((PyObject*)self);
}

PyDoc_STRVAR(IcnFile_getSurface__doc__,
"getSurface(index) -> Surface object\n\
\n\
Returns a Surface object from index.\n\
");

static PyObject *
IcnFile_getSurface(Py_IcnFile *self, PyObject *args)
{
    uint16_t index;
    Surface *surface;
    if (!PyArg_ParseTuple(args, "H", &index))
	return nullptr;

    try {
	surface = new Surface(self->icnFile->getSurface(index));
    } catch(Exception e) {
	PyErr_Format(PyExc_Exception, "%s: %s", e.getLocation().c_str(), e.getMessage().c_str());
	return nullptr;
    } catch(std::out_of_range e) {
	PyErr_SetString(PyExc_IndexError, "IcnFile index out of range");
	return nullptr;
    }


    PyObject *pysurface = Surface_Type.tp_new(&Surface_Type, reinterpret_cast<PyObject*>(surface), nullptr);
    return pysurface;
}

PyDoc_STRVAR(IcnFile_getTiles__doc__,
"getTiles(mapindex [, frameByFrame=False]) -> Surface object\n\
\n\
Returns a tiled Surface object from map index n.\n\
If frameByFrame is True, all different frames made out of the tiles will be made.\n\
");

static PyObject *
IcnFile_getTiles(Py_IcnFile *self, PyObject *args)
{
    bool frameByFrame = false;
    uint16_t index = 0;
    PyObject *pysurface = nullptr;
    Surface *tiles = nullptr;
    if (!PyArg_ParseTuple(args, "H|B", &index, &frameByFrame))
	return nullptr;

    try {
	tiles = new Surface(self->icnFile->getTiles(index, frameByFrame));
    } catch(Exception e) {
	PyErr_Format(PyExc_Exception, "%s: %s", e.getLocation().c_str(), e.getMessage().c_str());
	return nullptr;
    } catch(std::out_of_range e) {
	PyErr_SetString(PyExc_IndexError, "IcnFile index out of range");
	return nullptr;
    }

    pysurface = Surface_Type.tp_new(&Surface_Type, reinterpret_cast<PyObject*>(tiles), nullptr);
    return pysurface;
}

static PyMethodDef IcnFile_methods[] = {
    {"getSurface", (PyCFunction)IcnFile_getSurface, METH_VARARGS, IcnFile_getSurface__doc__},
    {"getTiles", (PyCFunction)IcnFile_getTiles, METH_VARARGS, IcnFile_getTiles__doc__},
    {nullptr, nullptr, 0, nullptr}		/* sentinel */
};

static PyMemberDef IcnFile_members[] = {
    {const_cast<char*>("size"), T_USHORT, offsetof(Py_IcnFile, size), RO, nullptr},
    {nullptr, 0, 0, 0, nullptr}
};

PyTypeObject IcnFile_Type = {
    PyObject_HEAD_INIT(nullptr)
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
    IcnFile_init__doc__,			/*tp_doc*/
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
    IcnFile_alloc,	    			/*tp_alloc*/
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
