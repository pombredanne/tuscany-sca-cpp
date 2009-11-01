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

#ifndef tuscany_list_hpp
#define tuscany_list_hpp

/**
 * Simple list functions.
 */

#include <iostream>
#include "function.hpp"

namespace tuscany {

long countlists = 0;
long countIlists = 0;
long countClists = 0;
long countElists = 0;

bool resetListCounters() {
    countlists = countIlists = countClists = countElists = 0;
    return true;
}

bool printListCounters() {
    std::cout << "countlists " << countlists << std::endl;
    std::cout << "countElists " << countElists << std::endl;
    std::cout << "countIlists " << countIlists << std::endl;
    std::cout << "countClists " << countClists << std::endl;
    return true;
}

/**
 * A car/cdr lisp-like pair, base structure to construct lists.
 */

template<typename T> class list {
public:

    list() {
        countlists++;
        countElists++;
    }

    list(const T car, const lambda<list<T> ()>& cdr) :
        car(car), cdr(cdr) {
        countlists++;
        countIlists++;
    }

    list(const list& p) :
        car(p.car), cdr(p.cdr) {
        countlists++;
        countClists++;
    }

    const list& operator=(const list<T>& p) {
        if(this == &p)
            return *this;
        car = p.car;
        cdr = p.cdr;
        return *this;
    }

    ~list() {
        countlists--;
    }

    const bool operator==(const list<T>& p) const {
        if(this == &p)
            return true;
        if(isNil(cdr))
            return isNil(p.cdr);
        if(isNil(p.cdr))
            return false;
        if(!(car == p.car))
            return false;
        if(cdr == p.cdr)
            return true;
        return cdr() == p.cdr();
    }

    const bool operator!=(const list<T>& p) const {
        return !this->operator==(p);
    }

    operator const list<list<T> >() const {
        return (list<list<T> >)T(*this);
    }

    list<T>& operator<<(const T& v) {
        *this = append(*this, mklist(v));
        return *this;
    }

    template<typename X> friend const bool isNil(const list<X>& p);
    template<typename X> friend const X car(const list<X>& p);
    template<typename X> friend const list<X> cdr(const list<X>& p);
    template<typename X> friend const bool setCar(list<X>& p, const X& car);
    template<typename X> friend const bool setCdr(list<X>& p, const list<X>& cdr);
    template<typename X> friend const bool setCdr(list<X>& p, const lambda<list<X> ()>& cdr);

private:
    T car;
    lambda<list<T> ()> cdr;
};

/**
 * Returns true if the given list is nil.
 */
template<typename T> const bool isNil(const list<T>& p) {
    return isNil(p.cdr);
}

/**
 * Write a list to an output stream.
 */
template<typename T> std::ostream& operator<<(std::ostream& out, const list<T>& l) {
    if(isNil(l))
        return out << "()";
    out << "(";
    list<T> ml = l;
    while(true) {
        out << car(ml);
        ml = cdr(ml);
        if (isNil(ml))
            break;
        out << " ";
    }
    return out << ")";
}

/**
 * Construct a (lazy) list from a value and a lambda function that returns the cdr.
 */
template<typename T> const list<T> cons(const T& car, const lambda<list<T> ()>& cdr) {
    return list<T> (car, cdr);
}

/**
 * Construct a list from a value and a cdr list.
 */
template<typename T> const list<T> cons(const T& car, const list<T>& cdr) {
    return list<T> (car, result(cdr));
}

/**
 * Cons variations for use with the reduce and reduceRight functions.
 */
template<typename T> const list<T> lcons(const list<T>& cdr, const T& car) {
    return cons<T>(car, cdr);
}

template<typename T> const list<T> rcons(const T& car, const list<T>& cdr) {
    return cons<T>(car, cdr);
}

/**
 * Construct a list from a single value.
 */
template<typename T> const list<T> cons(const T& car) {
    return list<T> (car, result(list<T> ()));
}

/**
 * Construct a list of one value.
 */
template<typename T> const list<T> mklist(const T& car) {
    return cons<T>(car);
}

/**
 * Construct a list of two values.
 */
template<typename T> const list<T> mklist(const T& a, const T& b) {
    return cons(a, mklist(b));
}

/**
 * Construct a list of three values.
 */
template<typename T> const list<T> mklist(const T& a, const T& b, const T& c) {
    return cons(a, cons(b, mklist(c)));
}

/**
 * Construct a list of four values.
 */
template<typename T> const list<T> mklist(const T& a, const T& b, const T& c, const T& d) {
    return cons(a, cons(b, cons(c, mklist(d))));
}

/**
 * Construct a list of five values.
 */
template<typename T> const list<T> mklist(const T& a, const T& b, const T& c, const T& d, const T& e) {
    return cons(a, cons(b, cons(c, cons(d, mklist(e)))));
}

/**
 * Construct a list of six values.
 */
template<typename T> const list<T> mklist(const T& a, const T& b, const T& c, const T& d, const T& e, const T& f) {
    return cons(a, cons(b, cons(c, cons(d, cons(e, mklist(f))))));
}

/**
 * Returns the car of a list.
 */
template<typename T> const T car(const list<T>& p) {
    return p.car;
}

/**
 * Returns the cdr of a list.
 */
template<typename T> const list<T> cdr(const list<T>& p) {
    return p.cdr();
}

/**
 * Sets the car of a list.
 */
template<typename T> const bool setCar(list<T>& p, const T& car) {
    p.car = car;
    return true;
}

/**
 * Sets the cdr of a list.
 */
template<typename T> const bool setCdr(list<T>& p, const list<T>& c) {
    p.cdr = result(c);
    return true;
}

/**
 * Sets the cdr of a list to a lambda function.
 */
template<typename T> const bool setCdr(list<T>& p, const lambda<list<T> ()>& cdr) {
    p.cdr = cdr;
    return true;
}


/**
 * Returns the car of the cdr of a list.
 */
template<typename T> const T cadr(const list<T>& p) {
    return car(cdr(p));
}

/**
 * Returns the car of the cdr of the cdr of a list.
 */
template<typename T> const T caddr(const list<T>& p) {
    return car(cdr(cdr(p)));
}

/**
 * Returns the cdr of a cdr of a list.
 */
template<typename T> const list<T> cddr(const list<T>& p) {
    return cdr(cdr(p));
}

/**
 * Returns the cdr of a cdr of the cdr of a list.
 */
template<typename T> const list<T> cdddr(const list<T>& p) {
    return cdr(cdr(cdr(p)));
}

/**
 * Returns the length of a list.
 */
template<typename T> struct lengthRef {
    const int operator()(const int c, const list<T>& p) {
        if(isNil(p))
            return c;
        return (*this)(c + 1, cdr(p));
    }
};

template<typename T> const int length(const list<T>& p) {
    return lengthRef<T> ()(0, p);
}

/**
 * Appends a list and a lambda function returning a list.
 */
template<typename T> struct appendCdr {
    const list<T> a;
    const lambda<list<T> ()> fb;
    appendCdr(const list<T>& a, const lambda<list<T> ()>& fb) :
        a(a), fb(fb) {
    }
    const list<T> operator()() const {
        return append(a, fb);
    }
};

template<typename T> const list<T> append(const list<T>&a, const lambda<list<T> ()>& fb) {
    if(isNil(a))
        return fb();

    return cons<T>(car(a), appendCdr<T> (cdr(a), fb));
}

/**
 * Appends two lists.
 */
template<typename T> const list<T> append(const list<T>&a, const list<T>& b) {
    return append(a, result(b));
}

/**
 * Map a lambda function on a list.
 */
template<typename T, typename R> const list<R> map(const lambda<R(T)>& f, const list<T>& p) {
    if(isNil(p))
        return list<R> ();
    return cons(f(car(p)), map(f, cdr(p)));
}

/**
 * Run a reduce lambda function on a list.
 */
template<typename T, typename R> struct reduceAccumulate {
    const lambda<R(R, T)> f;
    reduceAccumulate(const lambda<R(R, T)>& f) :
        f(f) {
    }
    R operator()(const R& acc, const list<T>& p) const {
        if(isNil(p))
            return acc;
        return (*this)(f(acc, car(p)), cdr(p));
    }
};

template<typename T, typename R> const R reduce(const lambda<R(R, T)>& f, const R& initial, const list<T>& p) {
    return reduceAccumulate<T, R> (f)(initial, p);
}

template<typename T, typename R> struct reduceRightAccumulate {
    const lambda<R(T, R)> f;
    reduceRightAccumulate(const lambda<R(T, R)>& f) :
        f(f) {
    }
    R operator()(const list<T>& p, const R& acc) const {
        if(isNil(p))
            return acc;
        return (*this)(cdr(p), f(car(p), acc));
    }
};

template<typename T, typename R> const R reduceRight(const lambda<R(T, R)>& f, const R& initial, const list<T>& p) {
    return reduceRightAccumulate<T, R> (f)(p, initial);
}

/**
 * Run a filter lambda function on a list.
 */
template<typename T> const list<T> filter(const lambda<bool(T)>& f, const list<T>& p) {
    if(isNil(p))
        return list<T> ();
    if(f(car(p))) {
        const lambda<list<T> (lambda<bool(T)> , list<T> )> ff(filter<T> );
        return cons(car(p), curry(ff, f, cdr(p)));
    }
    return filter(f, cdr(p));
}

/**
 * Returns a list pointing to a member of a list.
 */
template<typename T> const list<T> member(const T& t, const list<T>& p) {
    if(isNil(p))
        return list<T> ();
    if(t == car(p))
        return p;
    return member(t, cdr(p));
}

/**
 * Reverse a list.
 */
template<typename T> const list<T> reverseIter(const list<T>& acc, const list<T>& p) {
    if(isNil(p))
        return acc;
    return reverseIter(cons(car(p), acc), cdr(p));
}

template<typename T> const list<T> reverse(const list<T>& p) {
    return reverseIter(list<T> (), p);
}

template<typename T> const list<T> seq(const T& start, const T& end);

template<typename T> struct seqGenerate {
    const T start;
    const T end;
    seqGenerate(const T& start, const T&end) :
        start(start), end(end) {
    }
    const list<T> operator()() const {
        return seq<T> (start, end);
    }
};

/**
 * Returns a sequence of values between the given bounds.
 */
template<typename T> const list<T> seq(const T& start, const T& end) {
    if(start == end)
        return mklist(start);
    if(start < end)
        return cons<T>(start, seqGenerate<T> (start + 1, end));
    return cons<T>(start, seqGenerate<T> (start - 1, end));
}

/**
 * Returns the i-th element of a list.
 */
template<typename T> const T listRef(const list<T>& l, const int i) {
    if (i == 0)
        return car(l);
    return listRef(cdr(l), i - 1);
}

/**
 * Returns the first pair matching a key from a list of key value pairs.
 */
template<typename T> const list<T> assoc(const T& k, const list<list<T> >& p) {
    if(isNil(p))
        return list<T> ();
    if(k == car(car(p)))
        return car(p);
    return assoc(k, cdr(p));
}

/**
 * Returns a list of lists containing elements from two input lists.
 */
template<typename T> const list<list<T> > zip(const list<T>& a, const list<T>& b) {
    if (isNil(a) || isNil(b))
        return list<list<T> >();
    return cons<list<T> >(mklist<T>(car(a), car(b)), zip(cdr(a), cdr(b)));
}

/**
 * Converts a list of key value pairs to a list containing the list of keys and the list of values.
 */
template<typename T> const list<T> unzipKeys(const list<list<T> >& l) {
    if (isNil(l))
        return list<T>();
    return cons(car(car(l)), unzipKeys(cdr(l)));
}

template<typename T> const list<T> unzipValues(const list<list<T> >& l) {
    if (isNil(l))
        return list<T>();
    return cons(cadr(car(l)), unzipValues(cdr(l)));
}

template<typename T> const list<list<T> > unzip(const list<list<T> >& l) {
    return mklist<list<T> >(unzipKeys(l), unzipValues(l));
}

}

#endif /* tuscany_list_hpp */
