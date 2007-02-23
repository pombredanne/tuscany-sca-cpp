#!/bin/sh

#  Licensed to the Apache Software Foundation (ASF) under one
#  or more contributor license agreements.  See the NOTICE file
#  distributed with this work for additional information
#  regarding copyright ownership.  The ASF licenses this file
#  to you under the Apache License, Version 2.0 (the
#  "License"); you may not use this file except in compliance
#  with the License.  You may obtain a copy of the License at
#  
#    http://www.apache.org/licenses/LICENSE-2.0
#    
#  Unless required by applicable law or agreed to in writing,
#  software distributed under the License is distributed on an
#  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
#  KIND, either express or implied.  See the License for the
#  specific language governing permissions and limitations
#  under the License.


if [ x$TUSCANY_SCACPP = x ]; then
echo "TUSCANY_SCACPP not set"
exit;
fi
echo "Using SCA installed at $TUSCANY_SCACPP"

if [ x$TUSCANY_SDOCPP = x ]; then
echo "TUSCANY_SDOCPP not set"
exit;
fi
echo "Using SDO installed at $TUSCANY_SDOCPP"


if [ x$PYTHON_LIB = x ]; then
echo "PYTHON_LIB not set. Python samples will not be built"
elif [ x$PYTHON_INCLUDE = x ]; then
echo "PYTHON_INCLUDE not set. Python samples will not be built"
elif [ x$PYTHON_VERSION = x ]; then
echo "PYTHON_VERSION not set. Python samples will not be built"
else
echo "Building PYTHON samples with Python $PYTHON_VERSION installed at $PYTHON_LIB, $PYTHON_INCLUDE"
ENABLE_PYTHON=--enable-python
fi

if [ x$RUBY_LIB = x ]; then
echo "RUBY_LIB not set. Ruby samples will not be built"
elif [ x$RUBY_INCLUDE = x ]; then
echo "RUBY_INCLUDE not set. Ruby samples will not be built"
else
echo "Building Ruby samples with Ruby installed at $RUBY_LIB, $RUBY_INCLUDE"
ENABLE_RUBY=--enable-ruby
fi

./configure --prefix=${TUSCANY_SCACPP} --enable-static=no ${ENABLE_RUBY} ${ENABLE_PYTHON}
make
make install

