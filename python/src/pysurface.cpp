#include <istream>
#include <fstream>
#include <sstream>
#include "pyeastwood.h"
#include <structmember.h>

#include "eastwood/StdDef.h"
#include "eastwood/Exception.h"
#include "eastwood/Surface.h"

#include "pysurface.h"

using namespace eastwood;

static PyObject *
Surface_new(PyTypeObject *type, PyObject *args, __attribute__((unused)) PyObject *kwargs)
{
    Py_Surface *self = nullptr;
    self = (Py_Surface *)type->tp_alloc(type, 0);
    if (self != nullptr) {
	self->surface = reinterpret_cast<Surface*>(args);
	self->width = self->surface->width();
	self->height = self->surface->height();
	self->bpp = self->surface->bpp();
    }

    return (PyObject *)self;
}

static void
Surface_dealloc(Py_Surface *self)
{
    delete self->surface;
    PyObject_Del((PyObject*)self);
}

PyDoc_STRVAR(Surface_getPixels__doc__,
"getPixels() -> buffer\n\
\n\
Returns the pixel buffer from memory (mainly for debugging).\n\
");

static PyObject *
Surface_getPixels(Py_Surface *self)
{
    return PyBuffer_FromMemory((((uint8_t*)(*self->surface))), self->surface->size());
}

PyDoc_STRVAR(Surface_getScaled__doc__,
"getScaled(scaler) -> Surface object\n\
\n\
Returns a scaled Surface object using scaler.\n\
Available scalers to use are Scale* constants.\n\
");

static PyObject *
Surface_getScaled(Py_Surface *self, PyObject *args)
{
    Scaler scaler;
    Surface *scaled;
    if (!PyArg_ParseTuple(args, "I", &scaler))
	return nullptr;

    //TODO: throw exception
    if(!self->surface->scalePrecondition(scaler))
	return nullptr;
    try {
    	scaled = new Surface(self->surface->getScaled(scaler));
    } catch(Exception e) {
	PyErr_Format(PyExc_Exception, "%s: %s", e.getLocation().c_str(), e.getMessage().c_str());
	return nullptr;
    }

    return Surface_Type.tp_new(&Surface_Type, reinterpret_cast<PyObject*>(scaled), nullptr);
}

PyDoc_STRVAR(Surface_saveBMP__doc__,
"saveBMP() -> string\n\
\n\
Returns surface as a BMP file in the form of a string.\n\
");

static PyObject *
Surface_saveBMP(Py_Surface *self)
{
    std::stringbuf rdbuf;
    std::ostream output(&rdbuf);
    if(!output.good()) {
	PyErr_SetFromErrno(PyExc_IOError);
	return nullptr;
    }

    try {
    	self->surface->saveBMP(output);
    } catch(Exception e) {
	PyErr_Format(PyExc_Exception, "%s: %s", e.getLocation().c_str(), e.getMessage().c_str());
	return nullptr;
    }

    std::string buf = rdbuf.str();
    PyObject *ret = PyString_FromStringAndSize(buf.c_str(), buf.size());
    return ret;
}

static PyMethodDef Surface_methods[] = {
    {"getPixels", (PyCFunction)Surface_getPixels, METH_NOARGS, Surface_getPixels__doc__},
    {"getScaled", (PyCFunction)Surface_getScaled, METH_VARARGS, Surface_getScaled__doc__},
    {"saveBMP", (PyCFunction)Surface_saveBMP, METH_NOARGS, Surface_saveBMP__doc__},
    {nullptr, nullptr, 0, nullptr}		/* sentinel */
};

static PyMemberDef Surface_members[] = {
    {const_cast<char*>("width"), T_USHORT, offsetof(Py_Surface, width), RO, nullptr},
    {const_cast<char*>("height"), T_USHORT, offsetof(Py_Surface, height), RO, nullptr},
    {const_cast<char*>("bpp"), T_UBYTE, offsetof(Py_Surface, bpp), RO, nullptr},
    {nullptr, 0, 0, 0, nullptr}
};

PyTypeObject Surface_Type = {
    PyObject_HEAD_INIT(nullptr)
    0,						/*ob_size*/
    "pyeastwood.Surface",			/*tp_name*/
    sizeof(Py_Surface),				/*tp_basicsize*/
    0,						/*tp_itemsize*/
    (destructor)Surface_dealloc,		/*tp_dealloc*/
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
    Surface_methods,				/*tp_methods*/
    Surface_members,				/*tp_members*/
    0,						/*tp_getset*/
    0,                      			/*tp_base*/
    0,                      			/*tp_dict*/
    0,                      			/*tp_descr_get*/
    0,                      			/*tp_descr_set*/
    0,                      			/*tp_dictoffset*/
    0,						/*tp_init*/
    PyType_GenericAlloc,    			/*tp_alloc*/
    Surface_new,	      			/*tp_new*/
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
