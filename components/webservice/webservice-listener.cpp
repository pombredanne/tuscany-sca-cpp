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
 * Web service listener component implementation.
 */

#include "string.hpp"
#include "function.hpp"
#include "list.hpp"
#include "value.hpp"
#include "monad.hpp"
#include "../../modules/http/httpd.hpp"
#include "axis2.hpp"

namespace tuscany {
namespace webservice {

/**
 * Redirect an HTTP request to the Axis2/C HTTPD module. The given relay lambda function
 * is stored in the HTTPD request, for later retrieval by the Axis2 service to relay the request
 * to a target component.
 */
extern "C" {
    extern module axis2_module;
}

const value redirectToAxis2(const string& uri, request_rec* r, const value& relay) {
    const failable<request_rec*, int> nr = httpd::internalRedirectRequest(uri, r);
    if (!hasContent(nr))
        return value(reason(nr));
    ap_set_module_config(content(nr)->request_config, &axis2_module, const_cast<void*>((const void*)&relay));
    return value(httpd::internalRedirect(content(nr)));
}

/**
 * Handle an HTTP request.
 */
const failable<value> handle(const list<value>& params) {

    // Extract HTTPD request from the params
    request_rec* r = httpd::request(car(params));
    httpdDebugRequest(r, "webservice::handle");

    // Extract the relay lambda from the params and store it in the HTTPD request,
    // for later retrieval by our Axis2 service
    const value relay = cadr(params);
    cout << "relay: " << &relay << endl;

    // Redirect HTTPD request to Mod-axis2
    if (r->args == NULL)
        return redirectToAxis2(httpd::redirectURI("/axis2", string(r->uri)), r, relay);
    return redirectToAxis2(httpd::redirectURI("/axis2", string(r->uri), string(r->args)), r, relay);
}

}
}

extern "C" {

const tuscany::value apply(const tuscany::list<tuscany::value>& params) {
    const tuscany::value func(car(params));
    if (func == "handle")
        return tuscany::webservice::handle(cdr(params));
    return tuscany::mkfailure<tuscany::value>();
}

}
