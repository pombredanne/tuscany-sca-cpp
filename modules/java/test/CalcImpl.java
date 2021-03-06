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

package test;

import java.util.ArrayList;
import java.util.List;

import org.apache.tuscany.Service;

public class CalcImpl {

    public Double add(final Double x, final Double y, final Adder adder) {
        return adder.add(x, y);
    }

    public Double addEval(final Double x, final Double y, final Service adder) {
        return adder.eval("add", x, y);
    }

    public Double mult(final Double x, final Double y) {
        return x * y;
    }

    public Boolean even(final Double x) {
        return (double)((int)(double)x / 2 * 2) == (double)x;
    }

    public Iterable<Double> square(final Iterable<Double> l) {
        final List<Double> r = new ArrayList<Double>();
        for(final Double x : l)
            r.add(x * x);
        return r;
    }

}
