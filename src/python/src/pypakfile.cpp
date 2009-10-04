// TODO: This is *very* crude for now, need to figure out some neat way for
// 	 python to deal with C++ streams ...
#include <istream>
#include <fstream>
#include <sstream>

#include "eastwood/StdDef.h"
#include "eastwood/Exception.h"

#include "pyeastwood.h"
#include "pypakfile.h"


using namespace eastwood;

static int
PakFile_init(Py_PakFile *self, PyObject *args)
{
    char *fileName;
    if (!PyArg_ParseTuple(args, "s", &fileName))
	return -1;
    self->pakFileName = PyString_FromString(fileName);

#ifdef WITH_THREAD
    self->lock = PyThread_allocate_lock();
    if (!self->lock) {
	PyErr_SetString(PyExc_MemoryError, "unable to allocate lock");
	goto error;
    }
#endif

    self->stream = new std::fstream(fileName, std::ios::in | std::ios::out | std::ios::binary);
    if(!self->stream->good()) {
	PyErr_SetFromErrno(PyExc_IOError);
	goto error;
    }

    self->pakFile = new PakFile(*self->stream);

    return 0;

error:
#ifdef WITH_THREAD
    if (self->lock) {
	PyThread_free_lock(self->lock);
	self->lock = NULL;
    }
#endif
    return -1;
}

static PyObject *
PakFile_alloc(PyTypeObject *type, Py_ssize_t nitems)
{
    Py_PakFile *self = (Py_PakFile *)PyType_GenericAlloc(type, nitems);
    self->pakFile = NULL;
    self->stream = NULL;
    self->pakFileName = NULL;
    self->mode = std::ios_base::binary;
    self->lock = NULL;

    return (PyObject *)self;
}

static PyObject *
PakFile_close(Py_PakFile *self)
{
    ACQUIRE_LOCK(self);

    self->pakFile->close();
    self->mode = std::ios_base::binary;

    RELEASE_LOCK(self);

    Py_RETURN_TRUE;
}

static void
PakFile_dealloc(Py_PakFile *self)
{
    if(self->pakFile) {
    	int32_t sizediff = self->pakFile->sizediff();
    	if(sizediff < 0)
    	    truncateFile(PyString_AsString(self->pakFileName), std::abs(sizediff));
    }

    Py_XDECREF(self->pakFileName);
#ifdef WITH_THREAD
    if (self->lock)
	PyThread_free_lock(self->lock);
#endif

    if(self->pakFile)
	delete self->pakFile;
    if(self->stream) {
	self->stream->close();
	delete self->stream;
    }
    PyObject_Del((PyObject*)self);
}

static PyObject *
PakFile_listfiles(Py_PakFile *self)
{
    PyObject *fileList = PyTuple_New(self->pakFile->entries());
    for(uint32_t i = 0; i < self->pakFile->entries(); i++)
	PyTuple_SetItem(fileList, i, PyString_FromString(self->pakFile->getFileName(i).c_str()));
    return fileList;
}

static PyObject *
PakFile_open(Py_PakFile *self, PyObject *args, PyObject *kwargs)
{
    PyObject *name = NULL;
    char mode = 'r';
    static char *kwlist[] = {const_cast<char*>("name"), const_cast<char*>("mode"), NULL};

    PakFile_close(self);

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|c:open", kwlist, &name, &mode))
	return NULL;

    switch (mode) {
	case 'a':
	    self->mode |= std::ios_base::out | std::ios_base::ate;
	    break;
	case 'r':
	    self->mode |= std::ios_base::in;
	    break;
	case 'w':
	    self->mode |= std::ios_base::out | std::ios_base::trunc;
	    break;
	default:
	    PyErr_Format(PyExc_ValueError, "invalid mode char %c", mode);
	    return NULL;
	    break;
    }

    try {
	self->pakFile->open(PyString_AsString(name), self->mode);
    } catch(FileException e) {
	PyErr_Format(PyExc_IOError, "%s: %s", e.getLocation().c_str(), e.getMessage().c_str());
	return NULL;
    }

    Py_RETURN_TRUE;
}

static PyObject *
PakFile_delete(Py_PakFile *self, PyObject *args)
{
    PyObject *name = NULL;
    PakFile_close(self);

    if (!PyArg_ParseTuple(args, "O:delete", &name))
	return NULL;

    self->pakFile->erase(PyString_AsString(name));

    Py_RETURN_TRUE;
}

static PyObject *
PakFile_read(Py_PakFile *self, PyObject *args)
{
    std::streamoff offset = static_cast<std::streamoff>(self->pakFile->tellg());
    size_t bytesrequested = -1,
	   left = self->pakFile->size() - offset;
    PyObject *v = NULL;

    if (!PyArg_ParseTuple(args, "|l:read", &bytesrequested))
	return NULL;

    ACQUIRE_LOCK(self);
    if (bytesrequested > left)
	bytesrequested = left;
    if (bytesrequested > UINT_MAX) {
	PyErr_SetString(PyExc_OverflowError,
		"requested number of bytes is more than a Python string can hold");
	goto cleanup;
    }
    v = PyString_FromStringAndSize(NULL, bytesrequested);
    if (v == NULL)
	goto cleanup;

    Py_BEGIN_ALLOW_THREADS
    self->pakFile->read(PyString_AS_STRING(v), bytesrequested);
    Py_END_ALLOW_THREADS


cleanup:
    RELEASE_LOCK(self);
    return v;
}

static PyObject *
PakFile_write(Py_PakFile *self, PyObject *args)
{
    PyObject *ret = NULL;
    Py_buffer pbuf;
    char *buf;
    Py_ssize_t len;

    if (!PyArg_ParseTuple(args, "s*:write", &pbuf))
	return NULL;
    buf = (char*)pbuf.buf;
    len = pbuf.len;

    ACQUIRE_LOCK(self);
    if(!self->pakFile->is_open()) {
	PyErr_SetString(PyExc_ValueError,
		"I/O operation on closed file");
	goto cleanup;
    } else if(!(self->mode & std::ios_base::out) || self->pakFile->eof() || !self->pakFile->good()) {
	PyErr_SetString(PyExc_IOError,
		"file is not ready for writing");
	goto cleanup;
    }

    Py_BEGIN_ALLOW_THREADS
    self->pakFile->write(buf, len);
    Py_END_ALLOW_THREADS

    Py_INCREF(Py_None);
    ret = Py_None;

cleanup:
    PyBuffer_Release(&pbuf);
    RELEASE_LOCK(self);
    return ret;
}

static PyObject *
PakFile_seek(Py_PakFile *self, PyObject *args)
{
    int where = 0;
    PyObject *offobj;
    std::streamoff offset;
    PyObject *ret = NULL;

    if (!PyArg_ParseTuple(args, "O|i:seek", &offobj, &where))
	return NULL;

    if(self->pakFile->good()) {
	    self->pakFile->seekg(where, std::ios::beg);
	    self->pakFile->seekg(PyInt_AsLong(offobj), std::ios::cur);
	    offset = static_cast<std::streamoff>(self->pakFile->tellg());
    } else if(!self->pakFile->is_open()) {
	    PyErr_SetString(PyExc_ValueError,
		    "I/O operation on closed file");
	    goto cleanup;
    } else if(!self->pakFile->eof()) {
	    PyErr_SetString(PyExc_IOError, "seek works only while reading");
	    goto cleanup;
    }	

    Py_INCREF(Py_None);
    ret = Py_None;

cleanup:
    return ret;
}

static PyMethodDef PakFile_methods[] = {
    {"listfiles", (PyCFunction)PakFile_listfiles, METH_NOARGS, NULL},
    {"open", (PyCFunction)PakFile_open, METH_VARARGS|METH_KEYWORDS, NULL},
    {"close", (PyCFunction)PakFile_close, METH_NOARGS, NULL},
    {"delete", (PyCFunction)PakFile_delete, METH_VARARGS, NULL},
    {"read", (PyCFunction)PakFile_read, METH_VARARGS, NULL},
    {"write", (PyCFunction)PakFile_write, METH_VARARGS, NULL},
    {"seek", (PyCFunction)PakFile_seek, METH_VARARGS, NULL},
    {0, 0, 0, 0}
};


PyTypeObject PakFile_Type = {
    PyObject_HEAD_INIT(NULL)
    0,						/*ob_size*/
    "pyeastwood.PakFile",			/*tp_name*/
    sizeof(Py_PakFile),				/*tp_basicsize*/
    0,						/*tp_itemsize*/
    (destructor)PakFile_dealloc,		/*tp_dealloc*/
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
    PakFile_methods,				/*tp_methods*/
    0,						/*tp_members*/
    0,						/*tp_getset*/
    0,                      			/*tp_base*/
    0,                      			/*tp_dict*/
    0,                      			/*tp_descr_get*/
    0,                      			/*tp_descr_set*/
    0,                      			/*tp_dictoffset*/
    (initproc)PakFile_init,			/*tp_init*/
    PakFile_alloc,	    			/*tp_alloc*/
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
