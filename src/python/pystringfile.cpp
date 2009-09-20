// TODO: This is *very* crude for now, need to figure out some neat way for
// 	 python to deal with C++ streams ...
#include <istream>
#include <sstream>
#include "StdDef.h"

#include "pyeastwood.h"
#include <structmember.h>

#include "StringFile.h"
#include "PalFile.h"

#include "pystringfile.h"
#include "pypalfile.h"
#include "pysurface.h"

using namespace eastwood;

static int
StringFile_init(Py_StringFile *self, PyObject *args)
{
    Py_buffer pdata;
    if (!PyArg_ParseTuple(args, "s*", &pdata))
	return -1;

    std::istream stream(new std::stringbuf(std::string(reinterpret_cast<char*>(pdata.buf), pdata.len)));
    if(!stream.good()) {
	PyErr_SetFromErrno(PyExc_IOError);
	goto error;
    }

    self->stringFile = new StringFile(stream);
    self->size = self->stringFile->size();

    PyBuffer_Release(&pdata);
    return 0;

error:
    PyBuffer_Release(&pdata);
    return -1;
}

static void
StringFile_dealloc(Py_StringFile *self)
{
    delete self->stringFile;
}

static PyObject *
StringFile_getMissionString(Py_StringFile *self, PyObject *args)
{
    uint16_t mission = 0;
    MissionType missionType = MISSION_INVALID;
    if(!PyArg_ParseTuple(args, "HH", &mission, &missionType))
	return NULL;

    if(missionType >= MISSION_INVALID) {
	PyErr_SetString(PyExc_TypeError, "If given, second argument must be a PalFile object");
	return NULL;
    }

    return Py_BuildValue("s", self->stringFile->getString(mission, missionType).c_str());
}

static PyObject *
StringFile_getString(Py_StringFile *self, PyObject *args)
{
    uint16_t index = 0;
    if(!PyArg_ParseTuple(args, "H", &index))
	return NULL;

    return Py_BuildValue("s", self->stringFile->getString(index).c_str());
}

static PyMethodDef StringFile_methods[] = {
    {"getMissionString", (PyCFunction)StringFile_getMissionString, METH_VARARGS, NULL},
    {"getString", (PyCFunction)StringFile_getString, METH_VARARGS, NULL},
    {NULL, NULL, 0, NULL}		/* sentinel */
};

static PyMemberDef StringFile_members[] = {
    {const_cast<char*>("size"), T_USHORT, offsetof(Py_StringFile, size), RO, NULL},
    {NULL, 0, 0, 0, NULL}
};

PyTypeObject StringFile_Type = {
    PyObject_HEAD_INIT(NULL)
    0,						/*ob_size*/
    "pyeastwood.StringFile",			/*tp_name*/
    sizeof(Py_StringFile),			/*tp_basicsize*/
    0,						/*tp_itemsize*/
    (destructor)StringFile_dealloc,		/*tp_dealloc*/
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
    StringFile_methods,				/*tp_methods*/
    StringFile_members,				/*tp_members*/
    0,						/*tp_getset*/
    0,                      			/*tp_base*/
    0,                      			/*tp_dict*/
    0,                      			/*tp_descr_get*/
    0,                      			/*tp_descr_set*/
    0,                      			/*tp_dictoffset*/
    (initproc)StringFile_init,			/*tp_init*/
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
