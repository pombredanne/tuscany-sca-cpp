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
 * Test JSON data conversion functions.
 */

#include <assert.h>
#include "stream.hpp"
#include "string.hpp"
#include "json.hpp"

namespace tuscany {
namespace json {

bool testJSEval() {
    JSONContext cx;
    const string script("(function testJSON(n){ return JSON.parse(JSON.stringify(n)) })(5)");
    jsval rval;
    assert(JS_EvaluateScript(cx, cx.getGlobal(), c_str(script), length(script), "testJSON.js", 1, &rval));
    const string r(JS_GetStringBytes(JS_ValueToString(cx, rval)));
    assert(r == "5");
    return true;
}

ostream* jsonWriter(const string& s, ostream* os) {
    (*os) << s;
    return os;
}

bool testJSON() {
    const JSONContext cx;

    {
        const list<value> ad = mklist<value>(mklist<value>(attribute, "city", string("san francisco")), mklist<value>(attribute, "state", string("ca")));
        const list<value> ac = mklist<value>(mklist<value>(element, "id", string("1234")), mklist<value>(attribute, "balance", 1000));
        const list<value> cr = mklist<value>(mklist<value> (attribute, "name", string("jdoe")), cons<value>(element, cons<value>("address", ad)), cons<value>(element, cons<value>("account", ac)));
        const list<value> c = mklist<value>(cons<value>(element, cons<value>("customer", cr)));

        ostringstream os;
        writeJSON<ostream*>(jsonWriter, &os, c, cx);
        assert(str(os) == "{\"customer\":{\"@name\":\"jdoe\",\"address\":{\"@city\":\"san francisco\",\"@state\":\"ca\"},\"account\":{\"id\":\"1234\",\"@balance\":1000}}}");
    }
    {
        const list<value> phones = mklist<value> (string("408-1234"), string("650-1234"));
        const list<value> l = mklist<value> (mklist<value> (element, "phones", phones), mklist<value> (element, "lastName", string("test\ttab")), mklist<value> (attribute, "firstName", string("test1")));

        ostringstream os;
        writeJSON<ostream*>(jsonWriter, &os, l, cx);
        assert(str(os) == "{\"phones\":[\"408-1234\",\"650-1234\"],\"lastName\":\"test\\u0009tab\",\"@firstName\":\"test1\"}");

        istringstream is(str(os));
        const list<string> il = streamList(is);
        const list<value> r = content(readJSON(il, cx));
        assert(r == l);

        ostringstream wos;
        write(content(writeJSON(r, cx)), wos);
        assert(str(wos) == str(os));
    }
    {
        const list<value> l = mklist<value>(list<value>() + "ns1:echoString" + (list<value>() + "@xmlns:ns1" + string("http://ws.apache.org/axis2/services/echo")) + (list<value>() + "text" + string("Hello World!")));
        ostringstream wos;
        write(content(writeJSON(valuesToElements(l), cx)), wos);
        assert(str(wos) == "{\"ns1:echoString\":{\"@xmlns:ns1\":\"http://ws.apache.org/axis2/services/echo\",\"text\":\"Hello World!\"}}");

        istringstream is(str(wos));
        const list<string> il = streamList(is);
        const list<value> r = elementsToValues(content(readJSON(il, cx)));
        assert(r == l);
    }
    return true;
}

bool testJSONRPC() {
    JSONContext cx;
    {
        const string lm("{\"id\": 1, \"method\": \"test\", \"params\": []}");
        const list<value> e = content(readJSON(mklist(lm), cx));
        const list<value> v = elementsToValues(e);
        assert(assoc<value>("id", v) == mklist<value>("id", 1));
        assert(assoc<value>("method", v) == mklist<value>("method", string("test")));
        assert(assoc<value>("params", v) == mklist<value>("params", list<value>()));
    }
    {
        const string i("{\"id\":3,\"result\":[{\"price\":\"$2.99\",\"name\":\"Apple\"},{\"price\":\"$3.55\",\"name\":\"Orange\"},{\"price\":\"$1.55\",\"name\":\"Pear\"}]}");
        const list<value> e = content(readJSON(mklist(i), cx));
        const string i2("{\"id\":3,\"result\":{\"0\":{\"price\":\"$2.99\",\"name\":\"Apple\"},\"1\":{\"price\":\"$3.55\",\"name\":\"Orange\"},\"2\":{\"price\":\"$1.55\",\"name\":\"Pear\"}}}");
        const list<value> e2 = content(readJSON(mklist(i), cx));
        assert(e == e2);
    }
    {
        const string i("{\"id\":3,\"result\":[{\"price\":\"$2.99\",\"name\":\"Apple\"},{\"price\":\"$3.55\",\"name\":\"Orange\"},{\"price\":\"$1.55\",\"name\":\"Pear\"}]}");
        const list<value> e = content(readJSON(mklist(i), cx));
        ostringstream os;
        write(content(writeJSON(e, cx)), os);
        assert(str(os) == i);
        const list<value> v = elementsToValues(e);
        const list<value> r = valuesToElements(v);
        assert(r == e);
    }
    {
        const list<value> r = mklist<value>(mklist<value>("id", 1), mklist<value>("result", mklist<value>(string("Service.get"), string("Service.getTotal"))));
        const list<value> e = valuesToElements(r);
        ostringstream os;
        write(content(writeJSON(e, cx)), os);
        assert(str(os) == "{\"id\":1,\"result\":[\"Service.get\",\"Service.getTotal\"]}");
    }
    {
        const string f("{\"id\":1,\"result\":[\"Sample Feed\",\"123456789\",[\"Item\",\"111\",{\"name\":\"Apple\",\"currencyCode\":\"USD\",\"currencySymbol\":\"$\",\"price\":2.99}],[\"Item\",\"222\",{\"name\":\"Orange\",\"currencyCode\":\"USD\",\"currencySymbol\":\"$\",\"price\":3.55}],[\"Item\",\"333\",{\"name\":\"Pear\",\"currencyCode\":\"USD\",\"currencySymbol\":\"$\",\"price\":1.55}]]}");
        const list<value> r = content(readJSON(mklist(f), cx));
        const list<value> v = elementsToValues(r);
        const list<value> e = valuesToElements(v);
        ostringstream os;
        write(content(writeJSON(e, cx)), os);
        assert(str(os) == f);
    }
    {
        const list<value> arg = mklist<value>(list<value>() + "ns1:echoString" + (list<value>() + "@xmlns:ns1" + string("http://ws.apache.org/axis2/services/echo")) + (list<value>() + "text" + string("Hello World!")));
        const failable<list<string> > r = jsonRequest(1, "echo", mklist<value>(arg), cx);
        ostringstream os;
        write(content(r), os);
        assert(str(os) == "{\"id\":1,\"method\":\"echo\",\"params\":[{\"ns1:echoString\":{\"@xmlns:ns1\":\"http://ws.apache.org/axis2/services/echo\",\"text\":\"Hello World!\"}}]}");

        istringstream is(str(os));
        const list<string> il = streamList(is);
        const list<value> ir = elementsToValues(content(readJSON(il, cx)));
        assert(car<value>(cadr<value>(caddr<value>(ir))) == arg);
    }
    {
        const list<value> res = mklist<value>(list<value>() + "ns1:echoString" + (list<value>() + "@xmlns:ns1" + string("http://ws.apache.org/axis2/c/samples")) + (list<value>() + "text" + string("Hello World!")));
        const failable<list<string> > r = jsonResult(1, res, cx);
        ostringstream os;
        write(content(r), os);
        assert(str(os) == "{\"id\":1,\"result\":{\"ns1:echoString\":{\"@xmlns:ns1\":\"http://ws.apache.org/axis2/c/samples\",\"text\":\"Hello World!\"}}}");

        istringstream is(str(os));
        const list<string> il = streamList(is);
        const list<value> ir = elementsToValues(content(readJSON(il, cx)));
        assert(cdr<value>(cadr<value>(ir)) == res);
    }
    return true;
}

}
}

int main() {
    tuscany::cout << "Testing..." << tuscany::endl;

    tuscany::json::testJSEval();
    tuscany::json::testJSON();
    tuscany::json::testJSONRPC();

    tuscany::cout << "OK" << tuscany::endl;

    return 0;
}
