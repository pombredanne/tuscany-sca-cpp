# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
# 
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied.  See the License for the
# specific language governing permissions and limitations
# under the License.
#
#
# 
# 
# This Python code is a simple sample that provides a Python 
# client for the Calculator sample
 

import sys
import TuscanySCA

op = sys.argv[1]
val1 = sys.argv[2]
val2 = sys.argv[3]

# Locate the calculator service
calc = TuscanySCA.locateService("CalculatorService")

# Invoke the calculator operations
if op == 'add':
    result = calc.add(val1, val2)

elif op == 'sub':
    result = calc.sub(val1, val2)

elif op == 'mul':
    result = calc.mul(val1, val2)

elif op == 'div':
    result = calc.div(val1, val2)


print "calculator_wsclient: ",op,"(",val1,",",val2,") = ",result

