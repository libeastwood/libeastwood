#include <istream>
#include <fstream>
#include <sstream>
#include "pyeastwood.h"
#include <structmember.h>

#include "eastwood/StdDef.h"
#include "eastwood/Sound.h"

#include "pysound.h"

using namespace eastwood;

static PyObject *
Sound_new(PyTypeObject *type, PyObject *args, __attribute__((unused)) PyObject *kwargs)
{
    Py_Sound *self = NULL;
    self = (Py_Sound *)type->tp_alloc(type, 0);
    if (self != NULL)
	self->sound = reinterpret_cast<Sound*>(args);

    return (PyObject *)self;
}

static void
Sound_dealloc(Py_Sound *self)
{
    if(self->sound)
    	delete self->sound;
    PyObject_Del((PyObject*)self);
}

PyDoc_STRVAR(Sound_getBuffer__doc__,
"getBuffer() -> buffer\n\
\n\
Returns the sound buffer from memory (mainly for debugging).\n\
");

static PyObject *
Sound_getBuffer(Py_Sound *self)
{
    return PyBuffer_FromMemory((((uint8_t*)(*self->sound))), self->sound->size());
}

PyDoc_STRVAR(Sound_getResampled__doc__,
"getResampled(channels, frequency, format, interpolator) -> Sound object\n\
\n\
Returns a resampled Sound object using format & interpolator.\n\
Available formats and interpolators are FMT_* & I_* constants.\n\
");

static PyObject *
Sound_getResampled(Py_Sound *self, PyObject *args)
{
    uint8_t channels = 0;
    uint32_t frequency = 0;
    AudioFormat audioFormat = FMT_INVALID;
    Interpolator interpolator = I_INVALID;
    Sound *resampled = NULL;
    if (!PyArg_ParseTuple(args, "BIHI", &channels, &frequency, &audioFormat, &interpolator))
	return NULL;

    resampled = new Sound(self->sound->getResampled(channels, frequency, audioFormat, interpolator));

    return Sound_Type.tp_new(&Sound_Type, reinterpret_cast<PyObject*>(resampled), NULL);
}

PyDoc_STRVAR(Sound_saveWAV__doc__,
"saveWAV() -> string\n\
\n\
Returns sound as a WAV file in the form of a string.\n\
");

static PyObject *
Sound_saveWAV(Py_Sound *self)
{
    std::stringbuf rdbuf;
    std::ostream output(&rdbuf);
    if(!output.good()) {
	PyErr_SetFromErrno(PyExc_IOError);
	return NULL;
    }

    self->sound->saveWAV(output);
    std::string buf = rdbuf.str();
    PyObject *ret = PyString_FromStringAndSize(buf.c_str(), buf.size());
    return ret;
}

static PyMethodDef Sound_methods[] = {
    {"getBuffer", (PyCFunction)Sound_getBuffer, METH_NOARGS, Sound_getBuffer__doc__},
    {"getResampled", (PyCFunction)Sound_getResampled, METH_VARARGS, Sound_getResampled__doc__},
    {"saveWAV", (PyCFunction)Sound_saveWAV, METH_NOARGS, Sound_saveWAV__doc__},
    {NULL, NULL, 0, NULL}		/* sentinel */
};

PyTypeObject Sound_Type = {
    PyObject_HEAD_INIT(NULL)
    0,						/*ob_size*/
    "pyeastwood.Sound",				/*tp_name*/
    sizeof(Py_Sound),				/*tp_basicsize*/
    0,						/*tp_itemsize*/
    (destructor)Sound_dealloc,			/*tp_dealloc*/
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
    Sound_methods,				/*tp_methods*/
    0,						/*tp_members*/
    0,						/*tp_getset*/
    0,                      			/*tp_base*/
    0,                      			/*tp_dict*/
    0,                      			/*tp_descr_get*/
    0,                      			/*tp_descr_set*/
    0,                      			/*tp_dictoffset*/
    0,						/*tp_init*/
    PyType_GenericAlloc,    			/*tp_alloc*/
    Sound_new,	      				/*tp_new*/
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
