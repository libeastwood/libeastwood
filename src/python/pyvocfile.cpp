// TODO: This is *very* crude for now, need to figure out some neat way for
// 	 python to deal with C++ streams ...
#include <istream>
#include <sstream>
#include "pyeastwood.h"

#include "StdDef.h"

#include "pysound.h"
#include "pyvocfile.h"

using namespace eastwood;

static int
VocFile_init(Py_VocFile *self, PyObject *args)
{
    Py_buffer pdata;
    if (!PyArg_ParseTuple(args, "s*", &pdata))
	return -1;

    self->stream = new std::istream(new std::stringbuf(std::string(reinterpret_cast<char*>(pdata.buf), pdata.len)));
    if(!self->stream->good()) {
	PyErr_SetFromErrno(PyExc_IOError);
    	PyBuffer_Release(&pdata);
    	return -1;
    }

    self->vocFile = new VocFile(*self->stream);

    PyBuffer_Release(&pdata);
    return 0;
}

static void
VocFile_dealloc(Py_VocFile *self)
{
    delete self->vocFile;
    delete self->stream;
}

static PyObject *
VocFile_getSound(Py_VocFile *self)
{
    Sound *sound = self->vocFile->getSound();
    PyObject *pysound = Sound_Type.tp_new(&Sound_Type, reinterpret_cast<PyObject*>(sound), NULL);
    return pysound;
}


static PyMethodDef VocFile_methods[] = {
    {"getSound", (PyCFunction)VocFile_getSound, METH_NOARGS, NULL},
    {NULL, NULL, 0, NULL}		/* sentinel */
};


PyTypeObject VocFile_Type = {
    PyObject_HEAD_INIT(NULL)
    0,						/*ob_size*/
    "pyeastwood.VocFile",			/*tp_name*/
    sizeof(Py_VocFile),				/*tp_basicsize*/
    0,						/*tp_itemsize*/
    (destructor)VocFile_dealloc,		/*tp_dealloc*/
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
    VocFile_methods,				/*tp_methods*/
    0,						/*tp_members*/
    0,						/*tp_getset*/
    0,                      			/*tp_base*/
    0,                      			/*tp_dict*/
    0,                      			/*tp_descr_get*/
    0,                      			/*tp_descr_set*/
    0,                      			/*tp_dictoffset*/
    (initproc)VocFile_init,			/*tp_init*/
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
