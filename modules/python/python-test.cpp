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

/**
 * Test Python script evaluator.
 */

#include <assert.h>
#include "stream.hpp"
#include "string.hpp"
#include "driver.hpp"

namespace tuscany {
namespace python {

const string testPythonAdd =
        "def add(x, y):\n"
        "    return x + y\n";

bool testEvalExpr() {
    gc_scoped_pool pool;
    PythonRuntime py;

    istringstream is(testPythonAdd);
    failable<PyObject*> script = readScript("script", "script.py", is);
    assert(hasContent(script));

    const value exp = mklist<value>("add", 2, 3);
    const failable<value> r = evalScript(exp, content(script));
    assert(hasContent(r));
    assert(content(r) == value(5));

    return true;
}

const value mult(const list<value>& args) {
    const double x = car(args);
    const double y = cadr(args);
    return x * y;
}

const string testReturnLambda(
  "def mul(x, y):\n"
  "    return x * y\n"
  "\n"
  "def testReturnLambda():\n"
  "    return mul\n");

const string testCallLambda(
  "def testCallLambda(l, x, y):\n"
  "    return l(x, y)\n");

bool testEvalLambda() {
    gc_scoped_pool pool;
    PythonRuntime py;

    const value trl = mklist<value>("testReturnLambda");
    istringstream trlis(testReturnLambda);
    const failable<value> trlv = evalScript(trl, trlis);

    assert(hasContent(trlv));
    assert(isLambda(content(trlv)));
    const lambda<value(const list<value>&)> trll(content(trlv));
    assert(trll(mklist<value>(2, 3)) == value(6));

    istringstream tclis(testCallLambda);
    const value tcl = mklist<value>("testCallLambda", content(trlv), 2, 3);
    const failable<value> tclv = evalScript(tcl, tclis);
    assert(hasContent(tclv));
    assert(content(tclv) == value(6));

    istringstream tcelis(testCallLambda);
    const value tcel = mklist<value>("testCallLambda", lambda<value(const list<value>&)>(mult), 3, 4);
    const failable<value> tcelv = evalScript(tcel, tcelis);
    assert(hasContent(tcelv));
    assert(content(tcelv) == value(12));
    return true;
}

}
}

int main() {
    tuscany::cout << "Testing..." << tuscany::endl;

    tuscany::python::testEvalExpr();
    tuscany::python::testEvalLambda();

    tuscany::cout << "OK" << tuscany::endl;
    return 0;
}
