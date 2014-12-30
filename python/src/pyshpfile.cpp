// TODO: This is *very* crude for now, need to figure out some neat way for
// 	 python to deal with C++ streams ...
#include <istream>
#include <sstream>
#include <stdexcept>
#include "pyeastwood.h"
#include <structmember.h>

#include "eastwood/StdDef.h"
#include "eastwood/Exception.h"
#include "eastwood/ShpFile.h"
#include "eastwood/Palette.h"

#include "pyshpfile.h"
#include "pypalette.h"
#include "pysurface.h"

using namespace eastwood;

PyDoc_STRVAR(ShpFile_init__doc__,
"ShpFile(data, palette) -> ShpFile object\n\
\n\
Creates a ShpFile from data using palette specified.\n\
");

static int
ShpFile_init(Py_ShpFile *self, PyObject *args)
{
    Py_buffer pdata;
    PyObject *palObject = nullptr;
    if (!PyArg_ParseTuple(args, "s*O", &pdata, &palObject))
	return -1;

    self->stream = new std::istream(new std::stringbuf(std::string(reinterpret_cast<char*>(pdata.buf), pdata.len)));
    if(!self->stream->good()) {
	PyErr_SetFromErrno(PyExc_IOError);
	goto error;
    }
    if(!PyObject_TypeCheck(palObject, &Palette_Type)) {
	PyErr_SetString(PyExc_TypeError, "Second argument must be a Palette object");
	goto error;
    }

    try {
    	self->shpFile = new ShpFile(*self->stream, *((Py_Palette*)palObject)->palette);
    } catch(Exception e) {
	PyErr_Format(PyExc_Exception, "%s: %s", e.getLocation().c_str(), e.getMessage().c_str());
	goto error;
    }
    self->size = self->shpFile->size();

    PyBuffer_Release(&pdata);
    return 0;

error:
    PyBuffer_Release(&pdata);
    return -1;
}

static PyObject *
ShpFile_alloc(PyTypeObject *type, Py_ssize_t nitems)
{
    Py_ShpFile *self = (Py_ShpFile *)PyType_GenericAlloc(type, nitems);
    self->shpFile = nullptr;
    self->stream = nullptr;

    return (PyObject *)self;
}

static void
ShpFile_dealloc(Py_ShpFile *self)
{
    if(self->shpFile)
    	delete self->shpFile;
    if(self->stream) {
	delete self->stream->rdbuf();
    	delete self->stream;
    }
    PyObject_Del((PyObject*)self);
}

PyDoc_STRVAR(ShpFile_getSurface__doc__,
"getSurface(index) -> Surface object\n\
\n\
Returns a Surface object from index.\n\
");

static PyObject *
ShpFile_getSurface(Py_ShpFile *self, PyObject *args)
{
    uint16_t index;
    Surface *surface;
    if (!PyArg_ParseTuple(args, "H", &index))
	return nullptr;

    try {
    	surface = new Surface(self->shpFile->getSurface(index));
    } catch(Exception e) {
	PyErr_Format(PyExc_Exception, "%s: %s", e.getLocation().c_str(), e.getMessage().c_str());
	return nullptr;
    } catch(std::out_of_range e) {
	PyErr_SetString(PyExc_IndexError, "ShpFile index out of range");
	return nullptr;
    }

    PyObject *pysurface = Surface_Type.tp_new(&Surface_Type, reinterpret_cast<PyObject*>(surface), nullptr);
    return pysurface;
}

PyDoc_STRVAR(ShpFile_getSurfaceArray__doc__,
"getSurface(w,h, tiles) -> Surface object\n\
\n\
Returns a Surface of w*h surfaces from the indexes in the tiles tuple.\n\
");

static PyObject *
ShpFile_getSurfaceArray(Py_ShpFile *self, PyObject *args)
{
    PyObject *array = nullptr;
    uint8_t x = 0,
	    y = 0;
    Surface *surface;
    if (!PyArg_ParseTuple(args, "BBO", &x, &y, &array))
	return nullptr;
    if(!PyTuple_Check(array)) {
	PyErr_SetString(PyExc_TypeError, "Third argument must be a tuple!");
	return nullptr;
    }
    std::vector<uint32_t> tiles(PyTuple_Size(array));
    for(uint16_t i = 0; i < tiles.size(); i++) {
	PyObject *item = PyTuple_GetItem(array, i);
	if(!item)
	    return nullptr;
	tiles[i] = PyInt_AsLong(item);
    }

    try {
    	surface = new Surface(self->shpFile->getSurfaceArray(x,y, &tiles.front()));
    } catch(Exception e) {
	PyErr_Format(PyExc_Exception, "%s: %s", e.getLocation().c_str(), e.getMessage().c_str());
	return nullptr;
    } catch(std::out_of_range e) {
	PyErr_SetString(PyExc_IndexError, "ShpFile index out of range");
	return nullptr;
    }

    PyObject *pysurface = Surface_Type.tp_new(&Surface_Type, reinterpret_cast<PyObject*>(surface), nullptr);
    return pysurface;
}

static PyMethodDef ShpFile_methods[] = {
    {"getSurface", (PyCFunction)ShpFile_getSurface, METH_VARARGS, ShpFile_getSurface__doc__},
    {"getSurfaceArray", (PyCFunction)ShpFile_getSurfaceArray, METH_VARARGS, ShpFile_getSurfaceArray__doc__},
    {nullptr, nullptr, 0, nullptr}		/* sentinel */
};

static PyMemberDef ShpFile_members[] = {
    {const_cast<char*>("size"), T_USHORT, offsetof(Py_ShpFile, size), RO, nullptr},
    {nullptr, 0, 0, 0, nullptr}
};

PyTypeObject ShpFile_Type = {
    PyObject_HEAD_INIT(nullptr)
    0,						/*ob_size*/
    "pyeastwood.ShpFile",			/*tp_name*/
    sizeof(Py_ShpFile),				/*tp_basicsize*/
    0,						/*tp_itemsize*/
    (destructor)ShpFile_dealloc,		/*tp_dealloc*/
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
    ShpFile_init__doc__,			/*tp_doc*/
    0,						/*tp_traverse*/
    0,						/*tp_clear*/
    0,						/*tp_richcompare*/
    0,						/*tp_weaklistoffset*/
    0,						/*tp_iter*/
    0,						/*tp_iternext*/
    ShpFile_methods,				/*tp_methods*/
    ShpFile_members,				/*tp_members*/
    0,						/*tp_getset*/
    0,                      			/*tp_base*/
    0,                      			/*tp_dict*/
    0,                      			/*tp_descr_get*/
    0,                      			/*tp_descr_set*/
    0,                      			/*tp_dictoffset*/
    (initproc)ShpFile_init,			/*tp_init*/
    ShpFile_alloc,	    			/*tp_alloc*/
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
