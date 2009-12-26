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

#ifndef tuscany_scdl_hpp
#define tuscany_scdl_hpp

/**
 * SCDL read functions.
 */

#include <string>
#include "list.hpp"
#include "value.hpp"
#include "monad.hpp"
#include "xml.hpp"

namespace tuscany {
namespace scdl {

/**
 * Returns a list of components in a composite.
 */
const list<value> components(const value& l) {
    const list<value> cs = elementChildren("composite", l);
    if (isNil(cs))
        return cs;
    return elementChildren("component", car(cs));
}

/**
 * Returns the name of a component, service or reference.
 */
const value name(const value& l) {
    return attributeValue("name", l);
}

/**
 * Returns the scdl declaration with the given name.
 */
struct filterName {
    const value n;
    filterName(const value& n) : n(n) {
    }
    const bool operator()(const value& v) const {
        return name(v) == n;
    }
};

const value named(const value& name, const value& l) {
    const list<value> c = filter<value>(filterName(name), l);
    if (isNil(c))
        return value();
    return car(c);
}

/**
 * Returns the implementation of a component.
 */
const bool filterImplementation(const value& v) {
    return isElement(v) && std::string(cadr<value>(v)).find("implementation.") != std::string::npos;
}

const value implementation(const value& l) {
    const list<value> n = filter<value>(filterImplementation, l);
    if (isNil(n))
        return value();
    return car(n);
}

/**
 * Returns the URI of a service, reference or implementation.
 */
const value uri(const value& l) {
    return attributeValue("uri", l);
}

/**
 * Returns a list of services in a component.
 */
const list<value> services(const value& l) {
    return elementChildren("service", l);
}

/**
 * Returns a list of references in a component.
 */
const list<value> references(const value& l) {
    return elementChildren("reference", l);
}

/**
 * Returns a list of properties in a component.
 */
const list<value> properties(const value& l) {
    return elementChildren("property", l);
}

/**
 * Returns the target of a reference.
 */
const value target(const value& l) {
    return attributeValue("target", l);
}

/**
 * Returns a list of bindings in a service or reference.
 */
const bool filterBinding(const value& v) {
    return isElement(v) && std::string(cadr<value>(v)).find("binding.") != std::string::npos;
}

const list<value> bindings(const value& l) {
    return filter<value>(filterBinding, l);
}

/**
 * Returns the type of an implementation.
 */
const value implementationType(const value& l) {
    return elementName(l);
}

/**
 * Returns the type of a binding.
 */
const value bindingType(const value& l) {
    return elementName(l);
}

/**
 * Returns the value of a property.
 */
const value propertyValue(const value& l) {
    return elementValue(l);
}

}
}

#endif /* tuscany_scdl_hpp */