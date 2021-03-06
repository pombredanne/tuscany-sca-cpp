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

#ifndef tuscany_modcpp_hpp
#define tuscany_modcpp_hpp

/**
 * Evaluation functions used by mod-eval to evaluate C++
 * component implementations.
 */

#include "string.hpp"
#include "stream.hpp"

#include "function.hpp"
#include "list.hpp"
#include "element.hpp"
#include "value.hpp"
#include "monad.hpp"
#include "dynlib.hpp"
#include "../scheme/driver.hpp"

namespace tuscany {
namespace server {
namespace modcpp {

/**
 * Apply a C++ component implementation function.
 */
const list<value> failableResult(const value& func, const list<value>& v) {
    if (isNil(cdr(v)))
        return v;

    // Report a failure with an empty reason as 'function not supported'
    // Except for the start, and stop functions, which are optional
    const value reason = cadr(v);
    if (length(reason) == 0) {
        if (func == "start" || func == "stop")
            return mklist<value>(lambda<value(const list<value>&)>());
        return mklist<value>(value(), string("Function not supported: ") + func);
    }
    return v;
}

struct applyImplementation {
    const lib ilib;
    const lambda<value(const list<value>&)> impl;
    const list<value> px;

    applyImplementation(const lib& ilib, const lambda<value(const list<value>&)>& impl, const list<value>& px) : ilib(ilib), impl(impl), px(px) {
    }

    const value operator()(const list<value>& params) const {
        debug(params, "modeval::cpp::applyImplementation::input");

        // Apply the component implementation function
        const value val = failableResult(car(params), impl(append(params, px)));

        debug(val, "modeval::cpp::applyImplementation::result");
        return val;
    }
};

/**
 * Evaluate a C++ component implementation and convert it to
 * an applicable lambda function.
 */
const failable<lambda<value(const list<value>&)> > evalImplementation(const string& path, const value& impl, const list<value>& px) {

    // Configure the implementation's lambda function
    const value ipath(attributeValue("path", impl));
    const value iname(attributeValue("library", impl));
    const string fpath(isNil(ipath)? path + iname : path + ipath + "/" + iname);
    const lib ilib(*(new (gc_new<lib>()) lib(fpath + dynlibExt)));
    const failable<lambda<value(const list<value>&)> > evalf(dynlambda<value(const list<value>&)>("apply", ilib));
    if (!hasContent(evalf))
        return evalf;
    const lambda<value(const list<value>&)> l(applyImplementation(ilib, content(evalf), px));
    return l;
}

}
}
}

#endif /* tuscany_modcpp_hpp */
