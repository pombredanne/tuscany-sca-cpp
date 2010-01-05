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
 * Store Test case.
 */

#include <assert.h>
#include <regex.h>
#include "stream.hpp"
#include "string.hpp"
#include "list.hpp"
#include "xml.hpp"
#include "../../modules/scheme/driver.hpp"
#include "../../modules/json/json.hpp"

namespace store {

using namespace tuscany;

bool testScript() {
    gc_scoped_pool pool;

    ifstream is("store-script-test.scm");
    ostringstream os;
    scheme::evalDriverRun(is, os, pool);
    assert(contains(str(os), "(\"Sample Feed\" \""));
    assert(contains(str(os), "\" (\"Item\" \""));
    assert(contains(str(os), "\" ((javaClass \"services.Item\") (name \"Orange\") (currencyCode \"USD\") (currencySymbol \"$\") (price 3.55))) (\"Item\" \""));
    assert(contains(str(os), "\" ((javaClass \"services.Item\") (name \"Apple\") (currencyCode \"USD\") (currencySymbol \"$\") (price 2.99))))"));
    return true;
}

bool testEval() {
    {
        gc_scoped_pool pool;
        ifstream is("store-script-test.scm");
        ostringstream os;
        scheme::setupDisplay(os);
        scheme::Env globalEnv = scheme::setupEnvironment(pool);
        const value exp(mklist<value>("storeui_service", string("getcatalog")));
        const value val = scheme::evalScript(exp, is, globalEnv, pool);

        ostringstream vs;
        vs << val;
        assert(contains(str(vs), "(((javaClass \"services.Item\") (name \"Apple\") (currencyCode \"USD\") (currencySymbol \"$\") (price 2.99)) ((javaClass \"services.Item\") (name \"Orange\") (currencyCode \"USD\") (currencySymbol \"$\") (price 3.55)) ((javaClass \"services.Item\") (name \"Pear\") (currencyCode \"USD\") (currencySymbol \"$\") (price 1.55)))"));
    }

    {
        gc_scoped_pool pool;
        ifstream is("store-script-test.scm");
        ostringstream os;
        scheme::setupDisplay(os);

        scheme::Env globalEnv = scheme::setupEnvironment(pool);
        const value exp(mklist<value>("storeui_service", string("gettotal")));
        const value res = scheme::evalScript(exp, is, globalEnv, pool);

        ostringstream rs;
        rs << res;
        assert(contains(str(rs), "10"));
    }
    return true;
}

}

int main() {

    tuscany::cout << "Testing..." << tuscany::endl;

    store::testScript();
    store::testEval();

    tuscany::cout << "OK" << tuscany::endl;

    return 0;
}
