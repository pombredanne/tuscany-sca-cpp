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


TUSCANY_SCACPP_HOME=`pwd`

if [ x$AXIS2C_HOME = x ]; then
echo "AXIS2C_HOME not set"
exit;
fi
echo "Using Axis2C installed at $AXIS2C_HOME"

if [ x$TUSCANY_SDOCPP = x ]; then
echo "TUSCANY_SDOCPP not set"
exit;
fi
echo "Using SDO installed at $TUSCANY_SDOCPP"

if [ x$PYTHON_INCLUDE = x ]; then
echo "PYTHON_INCLUDE not set"
exit;
fi
if [ x$PYTHON_LIB = x ]; then
echo "PYTHON_LIB not set"
exit;
fi
if [ x$PYTHON_VERSION = x ]; then
echo "PYTHON_VERSION not set"
exit;
fi
echo "Building PYTHON extension with Python $PYTHON_VERSION installed at $PYTHON_LIB, $PYTHON_INCLUDE"

if [ x$RUBY_INCLUDE = x ]; then
echo "RUBY_INCLUDE not set"
exit;
fi
if [ x$RUBY_LIB = x ]; then
echo "RUBY_LIB not set"
exit;
fi
echo "Building Ruby extension with Ruby installed at $RUBY_LIB, $RUBY_INCLUDE"

cd ${TUSCANY_SCACPP_HOME}/samples
./autogen.sh
./configure --prefix=${TUSCANY_SCACPP_HOME}/deploy --enable-static=no --enable-python=yes --enable-ruby=yes

cd $TUSCANY_SCACPP_HOME
./autogen.sh
./configure --prefix=${TUSCANY_SCACPP_HOME}/deploy --enable-static=no --enable-python=yes --enable-ruby=yes

make bindist


