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
 * Test PostgreSQL access functions.
 */

#include <assert.h>
#include "stream.hpp"
#include "string.hpp"
#include "perf.hpp"
#include "pgsql.hpp"

namespace tuscany {
namespace pgsql {

bool testPGSql() {
    PGSql pg("dbname=db", "test");
    const value k = mklist<value>("a");

    assert(hasContent(post(k, string("AAA"), pg)));
    assert((get(k, pg)) == value(string("AAA")));
    assert(hasContent(put(k, string("aaa"), pg)));
    assert((get(k, pg)) == value(string("aaa")));
    assert(hasContent(del(k, pg)));
    assert(!hasContent(get(k, pg)));

    return true;
}

struct getLoop {
    const value k;
    PGSql& pg;
    getLoop(const value& k, PGSql& pg) : k(k), pg(pg) {
    }
    const bool operator()() const {
        assert((get(k, pg)) == value(string("CCC")));
        return true;
    }
};

bool testGetPerf() {
    const value k = mklist<value>("c");
    PGSql pg("dbname=db", "test");
    assert(hasContent(post(k, string("CCC"), pg)));

    const lambda<bool()> gl = getLoop(k, pg);
    cout << "PGSql get test " << time(gl, 5, 200) << " ms" << endl;
    return true;
}

}
}

int main() {
    tuscany::cout << "Testing..." << tuscany::endl;

    tuscany::pgsql::testPGSql();
    tuscany::pgsql::testGetPerf();

    tuscany::cout << "OK" << tuscany::endl;

    return 0;
}
