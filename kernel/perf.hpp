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

#ifndef tuscany_perf_hpp
#define tuscany_perf_hpp

/**
 * Functions to help measure performance.
 */

#include <sys/time.h>
#include <time.h>

#include "function.hpp"

namespace tuscany
{

/**
 * Measure the time required to perform a function in msec.
 */
struct timeLambda {
    const lambda<bool()> f;
    timeLambda(const lambda<bool()>& f) : f(f) {
    }
    bool operator()(const long count) const {
        if (count == 0)
            return true;
        f();
        (*this)(count - 1);
        return true;
    }
};

const double time(const lambda<bool()>& f, const long warmup, const long count) {
    const lambda<bool(long)> tl = timeLambda(f);
    struct timeval start;
    struct timeval end;

    tl(warmup);
    gettimeofday(&start, NULL);
    tl(count);
    gettimeofday(&end, NULL);

    const long t = (end.tv_sec * 1000 + end.tv_usec / 1000) - (start.tv_sec * 1000 + start.tv_usec / 1000);
    return (double)t / (double)count;
}

}
#endif /* tuscany_perf_hpp */
