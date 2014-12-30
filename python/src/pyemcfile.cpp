// TODO: This is *very* crude for now, need to figure out some neat way for
// 	 python to deal with C++ streams ...
#include <istream>
#include <sstream>
#include "eastwood/StdDef.h"

#include "pyeastwood.h"
#include <structmember.h>

#include "eastwood/Exception.h"
#include "eastwood/EmcFileAssemble.h"
#include "eastwood/EmcFileDisassemble.h"

#include "pyemcfile.h"

using namespace eastwood;

static int
EmcFile_init(Py_EmcFile *self, PyObject *args)
{
    Py_buffer pdata;
    if (!PyArg_ParseTuple(args, "s*c", &pdata, &self->mode))
	return -1;

    self->input = new std::istream(new std::stringbuf(std::string(reinterpret_cast<char*>(pdata.buf), pdata.len)));
    if(!self->input->good()) {
	PyErr_SetFromErrno(PyExc_IOError);
	goto error;
    }

    self->output = new std::ostream(new std::stringbuf());
    if(!self->output->good()) {
	PyErr_SetFromErrno(PyExc_IOError);
	goto error;
    }

    try {
	switch(self->mode) {
	    case 'a':
		self->emcFile = new EmcFileAssemble(*self->input, *self->output);
		break;
	    case 'd':
		self->emcFile = new EmcFileDisassemble(*self->input, *self->output);
		break;
	    default:
		PyErr_Format(PyExc_ValueError, "invalid mode: %c!", self->mode);
		goto error;
		break;
	}

	if(!self->emcFile->execute()) {
	    PyErr_Format(PyExc_ValueError, "Failed at line: %lu!", self->emcFile->labelCountGet());
	    goto error;
	}
    } catch(Exception e) {
	PyErr_Format(PyExc_Exception, "%s: %s", e.getLocation().c_str(), e.getMessage().c_str());
	goto error;
    }

    switch(self->emcFile->type()) {
	case script_BUILD:
	    self->type = PyString_FromString("BUILD");
	    break;
	case script_UNIT:
	    self->type = PyString_FromString("UNIT");
	    break;
	case script_TEAM:
	    self->type = PyString_FromString("TEAM");
	    break;
	default:
	    PyErr_Format(PyExc_TypeError, "invalid type: %d!", self->emcFile->type());
	    goto error;
	    break;
    }

    PyBuffer_Release(&pdata);
    return 0;

error:
    PyBuffer_Release(&pdata);
    return -1;
}

static PyObject *
EmcFile_alloc(PyTypeObject *type, Py_ssize_t nitems)
{
    Py_EmcFile *self = (Py_EmcFile *)PyType_GenericAlloc(type, nitems);
    self->input = nullptr;
    self->output = nullptr;
    self->emcFile = nullptr;
    self->mode = 0;
    self->type = nullptr;

    return (PyObject *)self;
}

static void
EmcFile_dealloc(Py_EmcFile *self)
{
    if(self->emcFile)
    	delete self->emcFile;
    if(self->input) {
	delete self->input->rdbuf();
    	delete self->input;
    }
    if(self->output) {
	delete self->output->rdbuf();
    	delete self->output;
    }
    Py_XDECREF(self->type);
    Py_TYPE(self)->tp_free((PyObject *)self);
}

static PyObject *
EmcFile_get(Py_EmcFile *self)
{
    std::string buf = ((std::stringbuf*)self->output->rdbuf())->str();
    return PyString_FromStringAndSize(buf.c_str(), buf.size());
}

static PyMethodDef EmcFile_methods[] = {
    {"get", (PyCFunction)EmcFile_get, METH_NOARGS, nullptr},
    {nullptr, nullptr, 0, nullptr}		/* sentinel */
};

static PyMemberDef EmcFile_members[] = {
    {const_cast<char*>("mode"), T_CHAR, offsetof(Py_EmcFile, mode), RO, nullptr},
    {const_cast<char*>("type"), T_OBJECT, offsetof(Py_EmcFile, type), RO, nullptr},
    {nullptr, 0, 0, 0, nullptr}
};

PyTypeObject EmcFile_Type = {
    PyObject_HEAD_INIT(nullptr)
    0,						/*ob_size*/
    "pyeastwood.EmcFile",			/*tp_name*/
    sizeof(Py_EmcFile),				/*tp_basicsize*/
    0,						/*tp_itemsize*/
    (destructor)EmcFile_dealloc,		/*tp_dealloc*/
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
    EmcFile_methods,				/*tp_methods*/
    EmcFile_members,				/*tp_members*/
    0,						/*tp_getset*/
    0,                      			/*tp_base*/
    0,                      			/*tp_dict*/
    0,                      			/*tp_descr_get*/
    0,                      			/*tp_descr_set*/
    0,                      			/*tp_dictoffset*/
    (initproc)EmcFile_init,			/*tp_init*/
    EmcFile_alloc,    				/*tp_alloc*/
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
