#include <Python.h>
#import "gglib.h"

using namespace gg;


// ---
// ---  graph object
// ---
typedef struct {
	PyObject_HEAD
	dgraph *graph;
} graph_object;


static void 
graph_destructor(graph_object *self)
{
	delete self->graph;
	self->ob_type->tp_free((PyObject*)self);
}

static PyObject *
graph_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	PyObject *self;
	assert(type != NULL && type->tp_alloc != NULL);
    
	self = type->tp_alloc(type, 0);
	if (self != NULL) {
		((graph_object *)self)->graph = 0;
	}
	return self;
}

static int
graph_init(PyObject *_self, PyObject *args, PyObject *kwargs)
{
	graph_object *self = (graph_object *) _self;

	if (kwargs && PyDict_Size(kwargs)) {
		PyErr_SetString(PyExc_TypeError, "graph.__init__ does not take keyword arguments");
		return -1;
	}

	if (self->graph) {
		delete self->graph;
		self->graph = 0;
	}

	self->graph = new dgraph();
	return 0;
}

static PyObject*
graph_add_link(PyObject *_self, PyObject *args, PyObject*kwds)
{
	graph_object *self = (graph_object *) _self;
	int s,e;

	if (!PyArg_ParseTuple(args, "ii:graph.add_link", &s, &e)) {
		return NULL;
	}
	
	self->graph->add_link(s,e);
	return Py_BuildValue("");	
}

static PyObject*
graph_remove_link(PyObject *_self, PyObject *args, PyObject*kwds)
{
	graph_object *self = (graph_object *) _self;
	int s,e;

	if (!PyArg_ParseTuple(args, "ii:graph.remove_link", &s, &e)) {
		return NULL;
	}
	
	self->graph->remove_link(s,e);
	return Py_BuildValue("");
}

static PyObject *
graph_get_links_to(PyObject *_self, PyObject *args, PyObject*kwds)
{
	graph_object *self = (graph_object *) _self;
	int docid;
	if (!PyArg_ParseTuple(args, "i:graph.get_links_to", &docid)) {
		return NULL;
	}
	
	vector<ggint> result;
	self->graph->get_links_to(docid, result);

	PyObject *pyresult = PyList_New(result.size());
	if (pyresult) {
		int count=0;
		for (vector<ggint>::iterator it=result.begin();
		     it!=result.end();
		     it++) {

			PyList_SetItem(pyresult, count, PyInt_FromLong(*it));
			count++;
			
		}
	}

	return pyresult;
}

static PyObject *
graph_get_links_from(PyObject *_self, PyObject *args, PyObject*kwds)
{
	graph_object *self = (graph_object *) _self;
	int docid;
	if (!PyArg_ParseTuple(args, "i:graph.get_links_from", &docid)) {
		return NULL;
	}
	
	vector<ggint> result;
	self->graph->get_links_from(docid, result);

	PyObject *pyresult = PyList_New(result.size());
	if (pyresult) {
		int count=0;
		for (vector<ggint>::iterator it=result.begin();
		     it!=result.end();
		     ++it) {
			PyList_SetItem(pyresult, count, PyInt_FromLong(*it));
			++count;
		}
	}

	return pyresult;
}


static PyObject *
graph_remove_links_to(PyObject *_self, PyObject *args, PyObject*kwds)
{
	graph_object *self = (graph_object *) _self;
	int docid;
	if (!PyArg_ParseTuple(args, "i:graph.remove_links_to", &docid)) {
		return NULL;
	}
	
	self->graph->remove_links_to(docid);
	return Py_BuildValue("");	
}

static PyObject *
graph_remove_links_from(PyObject *_self, PyObject *args, PyObject*kwds)
{
	graph_object *self = (graph_object *) _self;
	int con;
	if (!PyArg_ParseTuple(args, "i:graph.remove_links_from", &con)) {
		return NULL;
	}
	
	self->graph->remove_links_from(con);
	return Py_BuildValue("");	
}

static PyMethodDef graph_methods[] = {
	{"add_link"		, (PyCFunction)graph_add_link,		METH_VARARGS, "add_link(s,e)."},
	{"remove_link"		, (PyCFunction)graph_remove_link,	METH_VARARGS, "remove_link(s,e)."},
	{"get_links_to"		, (PyCFunction)graph_get_links_to,	METH_VARARGS, "get links to docid"},
	{"get_links_from"	, (PyCFunction)graph_get_links_from,    METH_VARARGS, "get links from container"},
	{"remove_links_to"	, (PyCFunction)graph_remove_links_to,	METH_VARARGS, "remove all links to docid"},
	{"remove_links_from"	, (PyCFunction)graph_remove_links_from,	METH_VARARGS, "remove all links from container"},
	{ NULL			, NULL }
};


PyTypeObject graph_type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,
	"graph",
	sizeof(graph_object),
	0,
	(destructor)graph_destructor, 		/* tp_dealloc - abstract class*/
	0,					/* tp_print */
	0,			 		/* tp_getattr */
	0,			 		/* tp_setattr */
	0,					/* tp_compare */
	0, 			                /* tp_repr */
	0,					/* tp_as_number */
	0,					/* tp_as_sequence */
	0,					/* tp_as_mapping */
	0,					/* tp_hash */
	0,					/* tp_call */
	0,					/* tp_str */
	PyObject_GenericGetAttr,		/* tp_getattro */
	PyObject_GenericSetAttr,		/* tp_setattro */
	0,					/* tp_as_buffer */
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /* tp_flags */
	"to be written",				/* tp_doc */
	0,					/* tp_traverse */
	0,					/* tp_clear */
	0,					/* tp_richcompare */
	0,					/* tp_weaklistoffset */
	0, //graph_iter,				/* tp_iter */
	0,		                        /* tp_iternext */
	graph_methods,				/* tp_methods */
	0, 			/* tp_members */
	0,			/* tp_getset */
	0,					/* tp_base */
	0,					/* tp_dict */
	0,					/* tp_descr_get */
	0,					/* tp_descr_set */
	0,					/* tp_dictoffset */
	(initproc)graph_init,			/* tp_init - abstract class*/
	PyType_GenericAlloc,			/* tp_alloc */
	graph_new,				/* tp_new  - abstract class*/
	PyObject_Del,                           /* tp_free */
};

extern "C" {
	DL_EXPORT(void) initgg();
}

DL_EXPORT(void)
initgg()
{
 	PyObject *module = Py_InitModule3("gg", 0, "");
 	PyObject *dict = PyModule_GetDict(module);

	PyDict_SetItemString(dict, "graph", (PyObject*)&graph_type);  /*  fixme: failure check */


	// need __builtin__ for PyRun_String
	if (PyDict_SetItemString(dict, "__builtins__", PyEval_GetBuiltins()) != 0) {
		return;
	}

	PyRun_String("version = '0.1.0'\n"
		     "version_info = (0,1,0)\n",
		     Py_file_input, dict, dict);
}
