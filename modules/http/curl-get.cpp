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
 * HTTP GET command line test tool.
 */

#include <assert.h>
#include "stream.hpp"
#include "string.hpp"
#include "perf.hpp"
#include "http.hpp"

namespace tuscany {
namespace http {

const bool testGet(const string& url, const string& ca = "", const string& cert = "", const string& key = "") {
    CURLSession ch(ca, cert, key);
    const failable<value> val = get(url, ch);
    assert(hasContent(val));
    cout << content(val) << endl;
    return true;
}

}
}

int main(unused const int argc, const char** argv) {
    if (argc > 2)
        tuscany::http::testGet(tuscany::string(argv[1]), tuscany::string(argv[2]), tuscany::string(argv[3]), tuscany::string(argv[4]));
    else
        tuscany::http::testGet(tuscany::string(argv[1]));
    return 0;
}

