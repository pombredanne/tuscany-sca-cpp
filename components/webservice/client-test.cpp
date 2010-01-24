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
 * Test Web service component.
 */

#include <assert.h>
#include "stream.hpp"
#include "string.hpp"
#include "list.hpp"
#include "element.hpp"
#include "value.hpp"
#include "monad.hpp"
#include "perf.hpp"
#include "../../modules/http/curl.hpp"

namespace tuscany {
namespace webservice {

const string url("http://localhost:8090/webservice");

bool testEval() {
    http::CURLSession cs;

    const value func = "http://ws.apache.org/axis2/c/samples/echoString";
    const list<value> arg = mklist<value>(
            list<value>() + "ns1:echoString"
            + (list<value>() + "@xmlns:ns1" + string("http://ws.apache.org/axis2/services/echo"))
            + (list<value>() + "text" + string("Hello World!")));

    const failable<value> rval = http::evalExpr(mklist<value>(func, arg), url, cs);
    assert(hasContent(rval));

    const list<value> r = mklist<value>(
            list<value>() + "ns1:echoString"
            + (list<value>() + "@xmlns:ns1" + string("http://ws.apache.org/axis2/c/samples"))
            + (list<value>() + "text" + string("Hello World!")));
    assert(content(rval) == r);
    return true;
}

}
}

int main() {
    tuscany::cout << "Testing..." << tuscany::endl;

    tuscany::webservice::testEval();

    tuscany::cout << "OK" << tuscany::endl;

    return 0;
}
