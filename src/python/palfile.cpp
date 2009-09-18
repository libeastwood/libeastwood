// TODO: This is *very* crude for now, need to figure out some neat way for
// 	 python to deal with C++ streams ...
#include <istream>
#include <fstream>
#include <sstream>
#include "pyeastwood.h"

#include "PalFile.h"
#include "StdDef.h"

#include "palfile.h"

using namespace eastwood;

static int
PalFile_init(Py_PalFile *self, PyObject *args)
{
    Py_ssize_t size;
    char *buffer = NULL;
    if (!PyArg_ParseTuple(args, "s#", &buffer, &size))
	return -1;

    self->stream = new std::istream(new std::stringbuf(std::string(buffer, size)));
    if(!self->stream->good())
    {
	PyErr_SetFromErrno(PyExc_IOError);
    	return -1;
    }

    self->palFile = new PalFile(*self->stream);

    return 0;
}

static void
PalFile_dealloc(Py_PalFile *self)
{
    delete self->palFile;
    delete self->stream;
}

PyTypeObject PalFile_Type = {
	PyObject_HEAD_INIT(NULL)
	0,					/*ob_size*/
	"eastwood.palfile",			/*tp_name*/
	sizeof(Py_PalFile),			/*tp_basicsize*/
	0,					/*tp_itemsize*/
	(destructor)PalFile_dealloc,		/*tp_dealloc*/
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
        0,					/*tp_methods*/
        0,					/*tp_members*/
        0,					/*tp_getset*/
        0,                      		/*tp_base*/
        0,                      		/*tp_dict*/
        0,                      		/*tp_descr_get*/
        0,                      		/*tp_descr_set*/
        0,                      		/*tp_dictoffset*/
        (initproc)PalFile_init,			/*tp_init*/
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
