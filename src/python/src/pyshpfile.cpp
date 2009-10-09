// TODO: This is *very* crude for now, need to figure out some neat way for
// 	 python to deal with C++ streams ...
#include <istream>
#include <sstream>
#include "pyeastwood.h"
#include <structmember.h>

#include "eastwood/StdDef.h"
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
    PyObject *palObject = NULL;
    if (!PyArg_ParseTuple(args, "s*O", &pdata, &palObject))
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

    self->shpFile = new ShpFile(*self->stream, *((Py_Palette*)palObject)->palette);
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
    self->shpFile = NULL;
    self->stream = NULL;

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
    if (!PyArg_ParseTuple(args, "H", &index))
	return NULL;

    Surface *surface = new Surface(self->shpFile->getSurface(index));
    PyObject *pysurface = Surface_Type.tp_new(&Surface_Type, reinterpret_cast<PyObject*>(surface), NULL);
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
    PyObject *array = NULL;
    uint8_t x = 0,
	    y = 0;
    if (!PyArg_ParseTuple(args, "BBO", &x, &y, &array))
	return NULL;
    if(!PyTuple_Check(array)) {
	PyErr_SetString(PyExc_TypeError, "Third argument must be a tuple!");
	return NULL;
    }
    std::vector<uint32_t> tiles(PyTuple_Size(array));
    for(uint16_t i = 0; i < tiles.size(); i++) {
	PyObject *item = PyTuple_GetItem(array, i);
	if(!item)
	    return NULL;
	tiles[i] = PyInt_AsLong(item);
    }

    Surface *surface = new Surface(self->shpFile->getSurfaceArray(x,y, &tiles.front()));
    PyObject *pysurface = Surface_Type.tp_new(&Surface_Type, reinterpret_cast<PyObject*>(surface), NULL);
    return pysurface;
}

static PyMethodDef ShpFile_methods[] = {
    {"getSurface", (PyCFunction)ShpFile_getSurface, METH_VARARGS, ShpFile_getSurface__doc__},
    {"getSurfaceArray", (PyCFunction)ShpFile_getSurfaceArray, METH_VARARGS, ShpFile_getSurfaceArray__doc__},
    {NULL, NULL, 0, NULL}		/* sentinel */
};

static PyMemberDef ShpFile_members[] = {
    {const_cast<char*>("size"), T_USHORT, offsetof(Py_ShpFile, size), RO, NULL},
    {NULL, 0, 0, 0, NULL}
};

PyTypeObject ShpFile_Type = {
    PyObject_HEAD_INIT(NULL)
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
