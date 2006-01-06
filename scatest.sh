#!/bin/sh

#  Copyright 2005 The Apache Software Foundation or its licensors, as applicable.
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.


APFULLDIR=`pwd`

export SCA4CPP_HOME="$APFULLDIR"
if [ x$SCA4CPP = x ]; then
export SCA4CPP="$SCA4CPP_HOME"
fi
echo "Using SCA installed at $SCA4CPP"

if [ x$AXISCPP_DEPLOY = x ]; then
echo "AXISCPP_DEPLOY not set"
exit;
fi
echo "Using Axis C++ installed at $AXISCPP_DEPLOY"

if [ x$XERCES_DEPLOY = x ]; then
echo "XERCES_DEPLOY not set"
exit;
fi
echo "Using Xerces C++ installed at $XERCES_DEPLOY"

if [ x$SDO4CPP = x ]; then
echo "SDO4CPP not set"
export SDO4CPP=$SCA4CPP_HOME/../sdo
fi
echo "Using SDO installed at $SDO4CPP"

TEST_ROOT=$SCA4CPP_HOME/runtime/core/test

cp $TEST_ROOT/MyValue/MyValue.h $TEST_ROOT/testSCASystem/modules/MyValueServiceModule
cp $TEST_ROOT/MyValue/MyValueImpl.h $TEST_ROOT/testSCASystem/modules/MyValueServiceModule
cp $TEST_ROOT/MyValue/StockQuoteService.h $TEST_ROOT/testSCASystem/modules/MyValueServiceModule
cp $TEST_ROOT/CustomerInfo/CustomerInfo.h $TEST_ROOT/testSCASystem/modules/MyValueServiceModule
cp $TEST_ROOT/CustomerInfo/CustomerInfoImpl.h $TEST_ROOT/testSCASystem/modules/MyValueServiceModule

cd $SCA4CPP/bin
./scagen.sh -dir $TEST_ROOT/testSCASystem/modules/MyValueServiceModule -output $TEST_ROOT/tmp
cp $TEST_ROOT/tmp/MyValueImpl*.* $TEST_ROOT/MyValue
cp $TEST_ROOT/tmp/CustomerInfoImpl*.* $TEST_ROOT/CustomerInfo

cd $TEST_ROOT/CustomerInfo/Debug
make clean
make
cd $TEST_ROOT/MyValue/Debug
make clean
make
cd $TEST_ROOT/src/Debug
make clean
make

cp $TEST_ROOT/MyValue/Debug/libMyValue.so $TEST_ROOT/testSCASystem/modules/MyValueServiceModule
cp $TEST_ROOT/CustomerInfo/Debug/libCustomerInfo.so $TEST_ROOT/testSCASystem/modules/MyValueServiceModule

export LD_LIBRARY_PATH=$SCA4CPP/lib:$SDO4CPP/lib:$LD_LIBRARY_PATH:$AXISCPP_DEPLOY/lib:$XERCES_DEPLOY/lib

export SCA4CPP_SYSTEM_ROOT=$SCA4CPP_HOME/runtime/core/test/testSCASystem
export SCA4CPP_DEFAULT_MODULE=SubSystem1

cd $TEST_ROOT/src
Debug/tuscany_sca_test
