#include <istream>
#include <fstream>
#include <sstream>
#include "pyeastwood.h"
#include "PakFile.h"

using namespace eastwood;

struct Py_PakFile {
	PyObject_HEAD
	std::ifstream *stream;
	PakFile *pakFile;
};

static int
PakFile_init(Py_PakFile *self, PyObject *args)
{
    size_t fileSize;
    char *fileName = NULL;
    if (!PyArg_ParseTuple(args, "s#", &fileName, &fileSize))
	return -1;

    self->stream = new std::ifstream(fileName);

    self->pakFile = new PakFile(*self->stream);

    return 0;
}

static void
PakFile_dealloc(Py_PakFile *self)
{
    delete self->pakFile;
    self->stream->close();
    delete self->stream;
}

static PyObject *
PakFile_listfiles(Py_PakFile *self)
{
    PyObject *fileList = PyTuple_New(self->pakFile->size());
    for(uint32_t i = 0; i < self->pakFile->size(); i++)
	PyTuple_SetItem(fileList, i, PyString_FromString(self->pakFile->getFileName(i).c_str()));
    return fileList;
}

static PyObject *
PakFile_open(Py_PakFile *self, PyObject *args)
{
    size_t fileSize;
    char *fileName = NULL;
    char *fileString = NULL;
    PyObject *pyFile = NULL;
    if (!PyArg_ParseTuple(args, "s#", &fileName, &fileSize))
	return NULL;

    self->pakFile->open(fileName);
    return pyFile;
}

static PyMethodDef PakFile_methods[] = {
	{"listfiles", (PyCFunction)PakFile_listfiles, METH_NOARGS, NULL},
	{"open", (PyCFunction)PakFile_open, METH_VARARGS, NULL},
	{NULL, NULL, 0, NULL}		/* sentinel */
};


PyTypeObject PakFile_Type = {
	PyObject_HEAD_INIT(NULL)
	0,					/*ob_size*/
	"eastwood.PakFile",			/*tp_name*/
	sizeof(Py_PakFile),			/*tp_basicsize*/
	0,					/*tp_itemsize*/
	(destructor)PakFile_dealloc,		/*tp_dealloc*/
	0,					/*tp_print*/
	0,					/*tp_getattr*/
	0,					/*tp_setattr*/
	0,					/*tp_compare*/
	0,					/*tp_repr*/
	0,					/*tp_as_number*/
	0,					/*tp_as_sequence*/
	0,					/*tp_as_mapping*/
	0,					/*tp_hash*/
        0,					/*tp_call*/
        0,					/*tp_str*/
        PyObject_GenericGetAttr,		/*tp_getattro*/
        PyObject_GenericSetAttr,		/*tp_setattro*/
        0,					/*tp_as_buffer*/
        Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE,	/*tp_flags*/
        0,					/*tp_doc*/
        0,					/*tp_traverse*/
        0,					/*tp_clear*/
        0,					/*tp_richcompare*/
        0,					/*tp_weaklistoffset*/
        0,					/*tp_iter*/
        0,					/*tp_iternext*/
        PakFile_methods,			/*tp_methods*/
        0,					/*tp_members*/
        0,					/*tp_getset*/
        0,                      		/*tp_base*/
        0,                      		/*tp_dict*/
        0,                      		/*tp_descr_get*/
        0,                      		/*tp_descr_set*/
        0,                      		/*tp_dictoffset*/
        (initproc)PakFile_init,			/*tp_init*/
        PyType_GenericAlloc,    		/*tp_alloc*/
        PyType_GenericNew,	      		/*tp_new*/
      	0,		          		/*tp_free*/
        0,                      		/*tp_is_gc*/
	0,					/*tp_bases*/
	0,					/*tp_mro*/
	0,					/*tp_cache*/
	0,					/*tp_subclasses*/
	0,					/*tp_weaklist*/
	0,					/*tp_del*/
	0					/*tp_version_tag*/
};
