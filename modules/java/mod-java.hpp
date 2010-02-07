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

#ifndef tuscany_modjava_hpp
#define tuscany_modjava_hpp

/**
 * Evaluation functions used by mod-eval to evaluate Java
 * component implementations.
 */

#include "string.hpp"
#include "stream.hpp"
#include "function.hpp"
#include "list.hpp"
#include "value.hpp"
#include "monad.hpp"
#include "eval.hpp"
#include "../server/mod-eval.hpp"

namespace tuscany {
namespace server {
namespace modjava {

/**
 * Return the Java runtime configured in a server.
 */
java::JavaRuntime& javaRuntime(modeval::ServerConf sc) {
    return *(java::JavaRuntime*)sc.moduleConf;
}

/**
 * Apply a Java component implementation function.
 */
struct applyImplementation {
    java::JavaClass impl;
    const list<value> px;
    java::JavaRuntime& jr;
    applyImplementation(const java::JavaClass& impl, const list<value>& px, java::JavaRuntime& jr) : impl(impl), px(px), jr(jr) {
    }
    const value operator()(const list<value>& params) const {
        const value expr = append<value>(params, px);
        debug(expr, "modeval::java::applyImplementation::input");
        const failable<value> val = java::evalClass(jr, expr, impl);
        debug(val, "modeval::java::applyImplementation::result");
        if (!hasContent(val))
            return mklist<value>(value(), reason(val));
        return mklist<value>(content(val));
    }
};

/**
 * Evaluate a Java component implementation and convert it to an applicable
 * lambda function.
 */
const failable<lambda<value(const list<value>&)> > evalImplementation(const string& path, const value& impl, const list<value>& px, modeval::ServerConf& sc) {
    const string cn(attributeValue("class", impl));
    const failable<java::JavaClass> jc = java::readClass(javaRuntime(sc), path, cn);
    if (!hasContent(jc))
        return mkfailure<lambda<value(const list<value>&)> >(reason(jc));
    return lambda<value(const list<value>&)>(applyImplementation(content(jc), px, javaRuntime(sc)));
}

}
}
}

#endif /* tuscany_modjava_hpp */
