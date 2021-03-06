/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

/* $Rev$ $Date$ */

#ifndef tuscany_python_eval_hpp
#define tuscany_python_eval_hpp

/**
 * Python script evaluation logic.
 */
#include <python2.6/Python.h>

#include "list.hpp"
#include "value.hpp"

namespace tuscany {
namespace python {

/**
 * Represent a Python runtime.
 */
class PythonRuntime {
public:
    PythonRuntime() {
        debug("python::pythonruntime");
        if (Py_IsInitialized())
            return;
        Py_InitializeEx(0);
        const char* arg0 = "";
        PySys_SetArgv(0, const_cast<char**>(&arg0));
    }

    ~PythonRuntime() {
        debug("python::~pythonruntime");
    }
};

/**
 * Return the last python error.
 */
const string lastError() {
    if(PyErr_Occurred()) {
        PyObject* type;
        PyObject* val;
        PyObject* trace;
        PyErr_Fetch(&type, &val, &trace);
        if (type != NULL && val != NULL) {
            PyObject* stype = PyObject_Str(type);    
            PyObject* sval = PyObject_Str(val);    
            string msg = string() + PyString_AsString(stype) + " : " + PyString_AsString(sval);
            Py_DECREF(stype);
            Py_DECREF(sval);
            PyErr_Restore(type, val, trace);
            PyErr_Print();
            return msg;
        }
        PyErr_Restore(type, val, trace);
        PyErr_Print();
        return "Unknown Python error";
    }
    return "";
}

/**
 * Declare conversion functions.
 */
PyObject* valueToPyObject(const value& v);
const value pyObjectToValue(PyObject *o);
PyObject* valuesToPyTuple(const list<value>& v);
const list<value> pyTupleToValues(PyObject* o);

/**
 * Callable python type used to represent a lambda expression.
 */
typedef struct {
  PyObject_HEAD
  lambda<value(const list<value>&)> func;
} pyLambda;

PyObject *mkPyLambda(const lambda<value(const list<value>&)>& l);

void pyLambda_dealloc(PyObject* self) {
    debug(self, "python::pylambda_dealloc");
    PyObject_Del(self);
}

const string pyRepr(PyObject* o) {
    PyObject* r = PyObject_Repr(o);
    const string s = PyString_AsString(r);
    Py_DECREF(r);
    return s;
}

PyObject* pyLambda_call(PyObject* self, PyObject* args, unused PyObject* kwds) {
    debug("python::call");
    const pyLambda* pyl = (pyLambda*)self;
    const value result = pyl->func(pyTupleToValues(args));
    debug(result, "python::call::result");
    PyObject *pyr = valueToPyObject(result);
    return pyr;
}

struct pyProxy {
    const value name;
    const lambda<value(const list<value>&)> func;

    pyProxy(const value& name, const lambda<value(const list<value>&)>& func) : name(name), func(func) {
    }

    const value operator()(const list<value>& args) const {
        debug(name, "python::proxy::name");
        const value result = func(cons<value>(name, args));
        debug(result, "python::proxy::result");
        return result;
    }
};

PyObject* pyLambda_getattr(PyObject *self, PyObject *attrname) {
    const string name = PyString_AsString(attrname);
    if (substr(name, 0, 1) == "_")
        return PyObject_GenericGetAttr(self, attrname);

    if (name == "eval") {
        Py_INCREF(self);
        return self;
    }

    const pyLambda* pyl = (pyLambda*)self;
    debug(name, "python::getattr::name");
    PyObject* pyr = mkPyLambda(pyProxy(name, pyl->func));
    return pyr;
}

PyTypeObject pyLambda_type = {
    PyObject_HEAD_INIT(0)
    0,
    "lambda",
    sizeof(pyLambda),
    0,
    (destructor)pyLambda_dealloc,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    (ternaryfunc)pyLambda_call,
    0, 
    (binaryfunc)pyLambda_getattr,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0
};

/**
 * Create a new python object representing a lambda expression.
 */
PyObject *mkPyLambda(const lambda<value(const list<value>&)>& l) {
    pyLambda* pyl = NULL;
    pyl = PyObject_New(pyLambda, &pyLambda_type);
    if (pyl != NULL)
      pyl->func = l;
    debug(pyl, "python::mkpylambda");
    return (PyObject *)pyl;
}

/**
 * Convert a list of values to a python list.
 */
PyObject* valuesToPyListHelper(PyObject* l, const list<value>& v) {
    if (isNil(v))
        return l;
    PyObject* pyv = valueToPyObject(car(v));
    PyList_Append(l, pyv);
    Py_DECREF(pyv);
    return valuesToPyListHelper(l, cdr(v));
}

PyObject* valuesToPyTuple(const list<value>& v) {
    PyObject* pyl = valuesToPyListHelper(PyList_New(0), v);
    PyObject* pyt = PyList_AsTuple(pyl);
    Py_DECREF(pyl);
    return pyt;
}

/**
 * Convert a value to a python object.
 */
PyObject* valueToPyObject(const value& v) {
    switch (type(v)) {
    case value::List:
        return valuesToPyTuple(v);
    case value::Lambda:
        return mkPyLambda(v);
    case value::Symbol:
        return PyString_FromString(c_str(string("'") + v));
    case value::String:
        return PyString_FromString(c_str(v));
    case value::Number:
        return PyFloat_FromDouble((double)v);
    case value::Bool:
        return (bool)v? Py_True : Py_False;
    default:
        return Py_None;
    }
}

/**
 * Convert a python tuple to a list of values.
 */

const list<value> pyTupleToValuesHelper(PyObject* o, const size_t i, const size_t size) {
    if (i == size)
        return list<value>();
    return cons(pyObjectToValue(PyTuple_GetItem(o, i)), pyTupleToValuesHelper(o, i + 1, size));
}

const list<value> pyTupleToValues(PyObject* o) {
    return pyTupleToValuesHelper(o, 0, PyTuple_Size(o));
}

/**
 * Lambda function used to represent a python callable object.
 */
struct pyCallable {
    PyObject* func;

    pyCallable(PyObject* func) : func(func) {
        Py_INCREF(func);
    }

    ~pyCallable() {
        Py_DECREF(func);
    }

    const value operator()(const list<value>& args) const {
        PyObject* pyargs = valuesToPyTuple(args);
        PyObject* result = PyObject_CallObject(func, pyargs);
        const value v = pyObjectToValue(result);
        Py_DECREF(pyargs);
        Py_DECREF(result);
        return v;
    }
};

/**
 * Convert a python object to a value.
 */
const value pyObjectToValue(PyObject *o) {
    if (PyString_Check(o)) {
        const char* s = PyString_AsString(o);
        if (*s == '\'')
            return value(s + 1);
        return value(string(s));
    }
    if (PyBool_Check(o))
        return value(o == Py_True);
    if (PyInt_Check(o))
        return value((double)PyInt_AsLong(o));
    if (PyLong_Check(o))
        return value((double)PyLong_AsLong(o));
    if (PyFloat_Check(o))
        return value((double)PyFloat_AsDouble(o));
    if (PyTuple_Check(o))
        return pyTupleToValues(o);
    if (PyCallable_Check(o))
        return lambda<value(const list<value>&)>(pyCallable(o));
    return value();
}

/**
 * Convert a python script path to a module name.
 */
const string moduleName(const string& path) {
    return join(".", tokenize("/", substr(path, 0, length(path) -3)));
}

/**
 * Evaluate an expression against a script provided as a python object.
 */
const failable<value> evalScript(const value& expr, PyObject* script) {

    // Get the requested function
    PyObject* func = PyObject_GetAttrString(script, c_str(car<value>(expr)));
    if (func == NULL) {

        // The start, stop, and restart functions are optional
        const value fn = car<value>(expr);
        if (fn == "start" || fn == "stop") {
            PyErr_Clear();
            return value(lambda<value(const list<value>&)>());
        }

        return mkfailure<value>(string("Couldn't find function: ") + car<value>(expr) + " : " + lastError());
    }
    if (!PyCallable_Check(func)) {
        Py_DECREF(func);
        return mkfailure<value>(string("Couldn't find callable function: ") + car<value>(expr));
    }

    // Convert args to python objects
    PyObject* args = valuesToPyTuple(cdr<value>(expr));

    // Call the function
    PyObject* result = PyObject_CallObject(func, args);
    Py_DECREF(func);
    Py_DECREF(args);
    if (result == NULL)
        return mkfailure<value>(string("Function call failed: ") + car<value>(expr) + " : " + lastError());

    // Convert python result to a value
    const value v = pyObjectToValue(result);
    Py_DECREF(result);
    return v;
}

/**
 * Read a python script from an input stream.
 */
const failable<PyObject*> readScript(const string& name, const string& path, istream& is) {
    const list<string> ls = streamList(is);
    ostringstream os;
    write(ls, os);
    PyObject* code = Py_CompileStringFlags(c_str(str(os)), c_str(path), Py_file_input, NULL);
    if (code == NULL)
        return mkfailure<PyObject*>(string("Couldn't compile script: ") + path + " : " + lastError());
    PyObject* mod = PyImport_ExecCodeModuleEx(const_cast<char*>(c_str(name)), code, const_cast<char*>(c_str(path)));
    if (mod == NULL)
        return mkfailure<PyObject*>(string("Couldn't import module: ") + path + " : " + lastError());
    return mod;
}

/**
 * Evaluate an expression against a script provided as an input stream.
 */
const failable<value> evalScript(const value& expr, istream& is) {
    failable<PyObject*> script = readScript("script", "script.py", is);
    if (!hasContent(script))
        return mkfailure<value>(reason(script));
    return evalScript(expr, content(script));
}

}
}
#endif /* tuscany_python_eval_hpp */
