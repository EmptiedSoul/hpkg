#include <stdbool.h>
#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <structmember.h>

#include <libhrd.h>
#include <libhpm.h>

static PyTypeObject package_info;

typedef struct {
        PyObject_HEAD
	PyObject* name;
        PyObject* version;
        PyObject* dependencies;
        PyObject* conflicts;
        PyObject* description;
        PyObject* maintainer;
} py_package_entry_t;

static PyMemberDef package_info_members[] = {
	{"name", T_OBJECT, offsetof(py_package_entry_t, name), 0, "Package name"},
	{"version", T_OBJECT, offsetof(py_package_entry_t, version), 0, "Package Version"},
	{"dependencies", T_OBJECT, offsetof(py_package_entry_t, dependencies), 0, "Package Dependencies"},
	{"conflicts", T_OBJECT, offsetof(py_package_entry_t, conflicts), 0, "Package Conflicts"},
	{"description", T_OBJECT, offsetof(py_package_entry_t, description), 0, "Package Description"},
	{"maintainer", T_OBJECT, offsetof(py_package_entry_t, maintainer), 0, "Package Maintainer"},
	{NULL},
};

static int package_info_traverse(py_package_entry_t* self, visitproc visit, void* arg){
	Py_VISIT(self->name);
	Py_VISIT(self->version);
	Py_VISIT(self->dependencies);
	Py_VISIT(self->conflicts);
	Py_VISIT(self->description);
	Py_VISIT(self->maintainer);
	return 0;
}

static int package_info_clear(py_package_entry_t* self){
	Py_CLEAR(self->name);
	Py_CLEAR(self->version);
	Py_CLEAR(self->dependencies);
	Py_CLEAR(self->conflicts);
	Py_CLEAR(self->description);
	Py_CLEAR(self->maintainer);
	return 0;
}

static void package_info_dealloc(py_package_entry_t* self){
	PyObject_GC_UnTrack(self);
	package_info_clear(self);
	Py_TYPE(self)->tp_free((PyObject*)self);
	return;
}

static PyObject* package_info_new(PyTypeObject* type, PyObject* args, PyObject* kwds){
	py_package_entry_t* self;
	self = (py_package_entry_t *) type->tp_alloc(type, 0);
	if (self != NULL) {
		self->name = PyUnicode_FromString("");
		self->version = PyUnicode_FromString("");
		self->dependencies = PyUnicode_FromString("");
		self->conflicts = PyUnicode_FromString("");
		self->description = PyUnicode_FromString("");
		self->maintainer = PyUnicode_FromString("");
	}
	return (PyObject*)self;
}
/*
static int package_info_init(py_package_entry* self, PyObject* args, PyObject* kwds){
	static char* 
}
*/
static PyObject* get_package_info(PyObject* self, PyObject* args){
	
	char* package;	

	if (!PyArg_ParseTuple(args, "s", &package))
		return NULL;
	
	package_entry_t* _pkg = hpm_get_package_info(package);
	package_entry_t pkg = *_pkg;

	//PyTypeObject* package_info;

	py_package_entry_t* py_pkg = (py_package_entry_t*)package_info_new(&package_info, NULL, NULL); 

	int i = 0;

	py_pkg->name = PyUnicode_FromString(pkg.name);
	py_pkg->version = PyUnicode_FromString(pkg.version);
	py_pkg->description = PyUnicode_FromString(pkg.description);
	py_pkg->maintainer = PyUnicode_FromString(pkg.maintainer);

	int dep_size = 0;
	int confs_size = 0;

	if (pkg.dependencies != NULL) {
		while (pkg.dependencies[dep_size] != NULL)
			dep_size++;
		py_pkg->dependencies = PyList_New(dep_size);
		i = 0;
		while (pkg.dependencies[i] != NULL){
			PyList_SetItem(py_pkg->dependencies, i, PyUnicode_FromString(pkg.dependencies[i]));
			i++;
		}
	} else {
		py_pkg->dependencies = Py_None;
	}
	
	if (pkg.conflicts != NULL){
		while (pkg.conflicts[confs_size] != NULL)
			confs_size++;
		py_pkg->conflicts = PyList_New(confs_size);
		i = 0;
		while (pkg.conflicts[i] != NULL){
			PyList_SetItem(py_pkg->conflicts, i, PyUnicode_FromString(pkg.conflicts[i]));
			i++;
		}
	} else {
		py_pkg->conflicts = Py_None;
	}
		
	hpm_free_package_info(_pkg);

	return (PyObject*)py_pkg;
}

static PyObject* get_package_file_list(PyObject* self, PyObject* args){
	PyObject* py_pkg_info;
	PyObject* result;
	package_entry_t _pkg_info = {0};
	package_entry_t* pkg_info = &_pkg_info;

	if (!PyArg_ParseTuple(args, "O!", &package_info, &py_pkg_info))
		return NULL;
	
	wchar_t* temp_name = PyUnicode_AsWideCharString(((py_package_entry_t*)py_pkg_info)->name, NULL);
	wchar_t* temp_vers = PyUnicode_AsWideCharString(((py_package_entry_t*)py_pkg_info)->version, NULL);

	asprintf(&(pkg_info->name), "%ls", temp_name);
	asprintf(&(pkg_info->version), "%ls", temp_vers);

	PyMem_Free(temp_name);
	PyMem_Free(temp_vers);

	char** files = hpm_get_package_file_list(pkg_info);
	free(pkg_info->name); free(pkg_info->version);
	size_t files_size = 0;

	if (files != NULL){
		while (files[files_size] != NULL)
			files_size++;
		result = PyList_New(files_size);
		int i = 0;
		while (files[i] != NULL){
			PyList_SetItem(result, i, PyUnicode_FromString(files[i]));
			i++;
		}
	} else {
		result = Py_None;
	}

	hrd_string_array_free((void**)files);

	return result;
}

static PyObject* is_package_installed(PyObject* self, PyObject* args){
	char* package;
	if (!PyArg_ParseTuple(args, "s", &package))
		return NULL;
	if (hpm_is_package_installed(package))
		Py_RETURN_TRUE;
	else
		Py_RETURN_FALSE;
}

static PyTypeObject package_info = {
	PyVarObject_HEAD_INIT(NULL, 0)
	.tp_name = "libhpm.package_info",
	.tp_doc = "Package Info Structure",
	.tp_basicsize = sizeof(py_package_entry_t),
	.tp_itemsize = 0,
	.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_GC,
	.tp_dealloc = (destructor) package_info_dealloc,
	.tp_traverse = (traverseproc) package_info_traverse,
	.tp_clear = (inquiry) package_info_clear,
	.tp_members = package_info_members,
	.tp_new = package_info_new,
};

static PyMethodDef libhpm_methods[] = {
	{"get_package_info", get_package_info, METH_VARARGS, "Get information about installed package"},
	{"is_package_installed", is_package_installed, METH_VARARGS, "Is package installed in the system?"},
	{"get_package_file_list", get_package_file_list, METH_VARARGS, "Get list of files belong to package"},
	{0,0,0,0}
};

static PyModuleDef libhpm_module = {
	PyModuleDef_HEAD_INIT,
	.m_name = "libhpm",
	.m_doc = "Bindings to the HarDClanZ package management library",
	.m_size = -1,
	.m_methods = libhpm_methods,
};

PyMODINIT_FUNC
PyInit_libhpm(void){
	PyObject* m;
	if (PyType_Ready(&package_info) < 0)
		return NULL;
	m = PyModule_Create(&libhpm_module);
	if (m == NULL)
		return NULL;

	Py_INCREF(&package_info);
	if (PyModule_AddObject(m, "package_info", (PyObject*) &package_info) < 0) {
		Py_DECREF(&package_info);
		Py_DECREF(m);
		return NULL;
	}

	return m;
}
