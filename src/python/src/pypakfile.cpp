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

PyDoc_STRVAR(PakFile_init__doc__,
"PakFile(name [, create=False]) -> file object.\n\
\n\
Open a PAK archive file. If create is True, it will create a new, empty\n\
archive.\n\
");

static int
PakFile_init(Py_PakFile *self, PyObject *args)
{
    char *fileName;
    PyObject *create = Py_False;
    std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out | std::ios_base::binary;
    if (!PyArg_ParseTuple(args, "s|O:PakFile", &fileName, &create))
	return -1;
    if(!PyBool_Check(create)) {
	PyErr_SetString(PyExc_TypeError, "If given, second argument must be True or False");
	goto error;
    }
    self->pakFileName = PyString_FromString(fileName);
    if(create == Py_True)
	mode |= std::ios_base::trunc;

#ifdef WITH_THREAD
    self->lock = PyThread_allocate_lock();
    if (!self->lock) {
	PyErr_SetString(PyExc_MemoryError, "unable to allocate lock");
	goto error;
    }
#endif

    self->stream = new std::fstream(fileName, mode);
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

PyDoc_STRVAR(PakFile_close__doc__,
"close() -> None\n\
\n\
Close the file. Sets data attribute .closed to true. A closed file\n\
cannot be used for further I/O operations. close() may be called more\n\
than once without error.\n\
");

static PyObject *
PakFile_close(Py_PakFile *self)
{
    ACQUIRE_LOCK(self);

    self->pakFile->close();
    self->mode = std::ios_base::binary;

    RELEASE_LOCK(self);

    Py_RETURN_NONE;
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

PyDoc_STRVAR(PakFile_listfiles__doc__,
"listfiles() -> tuple.\n\
\n\
Returns the names of the files available in the archive.\n\
");

static PyObject *
PakFile_listfiles(Py_PakFile *self)
{
    PyObject *fileList = PyTuple_New(self->pakFile->entries());
    for(uint32_t i = 0; i < self->pakFile->entries(); i++)
	PyTuple_SetItem(fileList, i, PyString_FromString(self->pakFile->getFileName(i).c_str()));
    return fileList;
}

PyDoc_STRVAR(PakFile_open__doc__,
"open(name [, mode='r']) -> bool.\n\
\n\
Open a file in the archive. The mode can be the following:\n\
'r'\tOpen file for reading. The stream is positioned at the beginning of\n\
\tthe file.\n\
'r+'\tOpen for reading and writing. The stream is positioned at the\n\
\tbeginning of the file.\n\
'w'\tTruncate file to zero length or create file for writing. The stream\n\
\tis positioned at the beginning of the file.\n\
'w+'\tOpen for reading and writing. The file is created if it does not\n\
\texist, otherwise it is truncated. The stream is positioned at the\n\
\tbeginning of the file.\n\
'a'\tOpen for appending (writing at end of file). The file is created if it\n\
\tdoes not exist. The stream is positioned at the end of the file.\n\
'a+'\tOpen for reading and appending (writing at end of file). The file is\n\
\tcreated if it does not exist. The initial file position for reading is\n\
\tat the  beginning of the file, but output is always appended to the end\n\
\tof the file.\n\
");

static PyObject *
PakFile_open(Py_PakFile *self, PyObject *args, PyObject *kwargs)
{
    const char *name = NULL;
    const char *mode = "r";
    bool error = false;
    static char *kwlist[] = {const_cast<char*>("name"), const_cast<char*>("mode"), NULL};

    PakFile_close(self);

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "s|s:open", kwlist, &name, &mode))
	return NULL;

    if(mode[0] == 'r') {
	self->mode |= std::ios_base::in; if(mode[1]){if(mode[1] == '+')
	self->mode |= std::ios_base::out; else error = true;}
    } else if(mode[0] == 'a') {
	self->mode |= std::ios_base::in | std::ios_base::out | std::ios_base::ate; if(mode[1]){if(mode[1] == '+')
	self->mode |= std::ios_base::app; else error = true;}
    } else if(mode[0] == 'w') {
	self->mode |= std::ios_base::out | std::ios_base::trunc; if(mode[1]){if(mode[1] == '+')
	self->mode |= std::ios_base::in; else error = true;}
    } else error = true;

    if(error) {
	PyErr_Format(PyExc_ValueError, "invalid mode char %s", mode);
	return NULL;
    }

    try {
	self->pakFile->open(name, self->mode);
    } catch(FileException e) {
	PyErr_Format(PyExc_IOError, "%s: %s", e.getLocation().c_str(), e.getMessage().c_str());
	return NULL;
    }

    Py_RETURN_TRUE;
}

PyDoc_STRVAR(PakFile_delete__doc__,
"delete(name) -> None.\n\
\n\
Deletes file from the archive.\n\
");

static PyObject *
PakFile_delete(Py_PakFile *self, PyObject *args)
{
    const char *name = NULL;
    PakFile_close(self);

    if (!PyArg_ParseTuple(args, "s:delete", &name))
	return NULL;

    self->pakFile->erase(name);

    Py_RETURN_NONE;
}

PyDoc_STRVAR(PakFile_read__doc__,
"read([size]) -> string\n\
\n\
Read at most size uncompressed bytes, returned as a string. If the size\n\
argument is negative or omitted, read until EOF is reached.\n\
");

static PyObject *
PakFile_read(Py_PakFile *self, PyObject *args)
{
    std::streamoff offset = static_cast<std::streamoff>(self->pakFile->tellg());
    size_t bytesrequested = -1,
	   left = self->pakFile->sizeg() - offset;
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


PyDoc_STRVAR(PakFile_write__doc__,
"write(data) -> None\n\
\n\
Write the 'data' string to file. Note that due to buffering, close() is\n\
needed before the file on disk reflects the data written.\n\
");

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
PakFile_seek(Py_PakFile *self, PyObject *args, bool in)
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
	    offset = static_cast<std::streamoff>(in ? self->pakFile->tellg() : self->pakFile->tellp());
    } else if(!self->pakFile->is_open()) {
	    PyErr_SetString(PyExc_ValueError,
		    "I/O operation on closed file");
	    goto cleanup;
    }

    Py_INCREF(Py_None);
    ret = Py_None;

cleanup:
    return ret;
}

PyDoc_STRVAR(PakFile_seekg__doc__,
"seekg(offset [, whence]) -> None\n\
\n\
Move to new read position. Argument offset is a byte count. Optional\n\
argument whence defaults to 0 (offset from start of file, offset\n\
should be >= 0); other values are 1 (move relative to current position,\n\
positive or negative), and 2 (move relative to end of file, usually\n\
negative).\n\
");

static PyObject *
PakFile_seekg(Py_PakFile *self, PyObject *args)
{
    return PakFile_seek(self, args, true);
}

PyDoc_STRVAR(PakFile_seekp__doc__,
"seekp(offset [, whence]) -> None\n\
\n\
Move to new write position. Argument offset is a byte count. Optional\n\
argument whence defaults to 0 (offset from start of file, offset\n\
should be >= 0); other values are 1 (move relative to current position,\n\
positive or negative), and 2 (move relative to end of file, usually\n\
negative).\n\
");

static PyObject *
PakFile_seekp(Py_PakFile *self, PyObject *args)
{
    return PakFile_seek(self, args, false);
}

static PyObject *
PakFile_tell(Py_PakFile *self, bool in)
{
    PyObject *ret = NULL;

    if(!self->pakFile->is_open()) {
	PyErr_SetString(PyExc_ValueError,
		"I/O operation on closed file");
	goto cleanup;
    } else if(!(self->mode & (in ? std::ios_base::in : std::ios_base::out))) {
	PyErr_SetString(PyExc_IOError, in ?
		"file not opened for reading" :
		"file not opened for writing");
	goto cleanup;
    }

    ret = PyInt_FromLong(static_cast<uint32_t>(in ? self->pakFile->tellg() : self->pakFile->tellp()));

cleanup:
    return ret;
}

PyDoc_STRVAR(PakFile_tellg__doc__,
"tellg() -> int\n\
\n\
Return the current read position, an integer.\n\
");

static PyObject *
PakFile_tellg(Py_PakFile *self, __attribute__((unused)) PyObject *args)
{
    return PakFile_tell(self, true);
}

PyDoc_STRVAR(PakFile_tellp__doc__,
"tellp() -> int\n\
\n\
Return the current write position, an integer.\n\
");

static PyObject *
PakFile_tellp(Py_PakFile *self, __attribute__((unused)) PyObject *args)
{
    return PakFile_tell(self, false);
}

static PyMethodDef PakFile_methods[] = {
    {"listfiles", (PyCFunction)PakFile_listfiles, METH_NOARGS, PakFile_listfiles__doc__},
    {"open", (PyCFunction)PakFile_open, METH_VARARGS|METH_KEYWORDS, PakFile_open__doc__},
    {"close", (PyCFunction)PakFile_close, METH_NOARGS, PakFile_close__doc__},
    {"delete", (PyCFunction)PakFile_delete, METH_VARARGS, PakFile_delete__doc__},
    {"read", (PyCFunction)PakFile_read, METH_VARARGS, PakFile_read__doc__},
    {"write", (PyCFunction)PakFile_write, METH_VARARGS, PakFile_write__doc__},
    {"seekg", (PyCFunction)PakFile_seekg, METH_VARARGS, PakFile_seekg__doc__},
    {"seekp", (PyCFunction)PakFile_seekp, METH_VARARGS, PakFile_seekp__doc__},
    {"tellg", (PyCFunction)PakFile_tellg, METH_NOARGS, PakFile_tellg__doc__},
    {"tellp", (PyCFunction)PakFile_tellp, METH_NOARGS, PakFile_tellp__doc__},
    {0, 0, 0, 0}
};

static PyObject *
PakFile_get_closed(Py_PakFile *self, __attribute__((unused)) void *closure)
{
    return PyBool_FromLong(!self->pakFile->is_open());
}

static PyGetSetDef PakFile_getset[] = {
    {const_cast<char*>("closed"), (getter)PakFile_get_closed, NULL,
	const_cast<char*>("True if the file is closed"), NULL},
    {NULL, NULL, NULL, NULL, NULL}	/* Sentinel */
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
    PakFile_init__doc__,			/*tp_doc*/
    0,						/*tp_traverse*/
    0,						/*tp_clear*/
    0,						/*tp_richcompare*/
    0,						/*tp_weaklistoffset*/
    0,						/*tp_iter*/
    0,						/*tp_iternext*/
    PakFile_methods,				/*tp_methods*/
    0,						/*tp_members*/
    PakFile_getset,				/*tp_getset*/
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
