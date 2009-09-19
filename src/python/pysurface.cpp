#include <istream>
#include <fstream>
#include <sstream>
#include "pyeastwood.h"
#include <structmember.h>

#include "StdDef.h"
#include "Surface.h"
#include "PalFile.h"

#include "pysurface.h"

using namespace eastwood;

static PyObject *
Surface_new(PyTypeObject *type, PyObject *args, __attribute__((unused)) PyObject *kwargs)
{
    Py_Surface *self = NULL;
    self = (Py_Surface *)type->tp_alloc(type, 0);
    if (self != NULL) {
	self->surface = reinterpret_cast<Surface*>(args);
	self->size = Py_BuildValue("(HH)", self->surface->size().x, self->surface->size().y);
    }

    return (PyObject *)self;
}

static void
Surface_dealloc(Py_Surface *self)
{
    delete self->surface;
}

static PyObject *
Surface_getScaled(Py_Surface *self, PyObject *args)
{
    Scaler scaler;
    Surface *scaled;
    if (!PyArg_ParseTuple(args, "i", &scaler))
	return NULL;

    //TODO: throw exception
    if(!self->surface->scalePrecondition(scaler))
	return Py_None;
    scaled = self->surface->getScaled(scaler);

    return Surface_Type.tp_new(&Surface_Type, reinterpret_cast<PyObject*>(scaled), NULL);
}


static PyObject *
Surface_saveBMP(Py_Surface *self, PyObject *args)
{
    Py_ssize_t size;
    char *fileName = NULL;
    if (!PyArg_ParseTuple(args, "s#", &fileName, &size))
	return NULL;

    std::ofstream out(fileName);
    self->surface->SaveBMP(out);
    out.close();
 
    return Py_True;
}


static PyMethodDef Surface_methods[] = {
    {"getScaled", (PyCFunction)Surface_getScaled, METH_VARARGS, NULL},
    {"saveBMP", (PyCFunction)Surface_saveBMP, METH_VARARGS, NULL},
    {NULL, NULL, 0, NULL}		/* sentinel */
};

static PyMemberDef Surface_members[] = {
    {const_cast<char*>("size"), T_OBJECT, offsetof(Py_Surface, size), RO, NULL},
    {NULL, 0, 0, 0, NULL}
};

PyTypeObject Surface_Type = {
    PyObject_HEAD_INIT(NULL)
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
