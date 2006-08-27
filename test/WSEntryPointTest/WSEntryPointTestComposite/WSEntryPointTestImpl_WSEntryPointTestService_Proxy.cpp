/*
 *  Copyright 2005 The Apache Software Foundation or its licensors, as applicable.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#include "WSCompositeServiceTypeTestImpl_WSCompositeServiceTypeTestService_Proxy.h"

#include "osoa/sca/sca.h"
using namespace osoa::sca;
using namespace tuscany::sca;

extern "C"
{

    #if defined(WIN32) || defined(_WINDOWS)
    __declspec(dllexport) 
    #endif
    WSCompositeServiceTypeTestImpl_WSCompositeServiceTypeTestService_Proxy* WSCompositeServiceTypeTestImpl_WSCompositeServiceTypeTestService_Proxy_Factory(ServiceWrapper* target)
    {
        return new WSCompositeServiceTypeTestImpl_WSCompositeServiceTypeTestService_Proxy(target);
    }

    #if defined(WIN32) || defined(_WINDOWS)
    __declspec(dllexport) 
    #endif
    void WSCompositeServiceTypeTestImpl_WSCompositeServiceTypeTestService_Proxy_Destructor(void* proxy)
    {
        delete (WSCompositeServiceTypeTestImpl_WSCompositeServiceTypeTestService_Proxy*)proxy;
    }
}

WSCompositeServiceTypeTestImpl_WSCompositeServiceTypeTestService_Proxy::WSCompositeServiceTypeTestImpl_WSCompositeServiceTypeTestService_Proxy(ServiceWrapper* targ) : target(targ)
{
}

WSCompositeServiceTypeTestImpl_WSCompositeServiceTypeTestService_Proxy::~WSCompositeServiceTypeTestImpl_WSCompositeServiceTypeTestService_Proxy()
{
    if (target)
        delete target;
}

char* WSCompositeServiceTypeTestImpl_WSCompositeServiceTypeTestService_Proxy::doChars( char* arg0)
{
    Operation operation("doChars");
    operation.addParameter(&arg0);
    char* ret;
    operation.setReturnValue(&ret);
    target->invoke(operation);
    return ret;
}

long WSCompositeServiceTypeTestImpl_WSCompositeServiceTypeTestService_Proxy::doLong( long arg0)
{
    Operation operation("doLong");
    operation.addParameter(&arg0);
    long ret;
    operation.setReturnValue(&ret);
    target->invoke(operation);
    return ret;
}

int WSCompositeServiceTypeTestImpl_WSCompositeServiceTypeTestService_Proxy::doInt( int arg0)
{
    Operation operation("doInt");
    operation.addParameter(&arg0);
    int ret;
    operation.setReturnValue(&ret);
    target->invoke(operation);
    return ret;
}

float WSCompositeServiceTypeTestImpl_WSCompositeServiceTypeTestService_Proxy::doFloat( float arg0)
{
    Operation operation("doFloat");
    operation.addParameter(&arg0);
    float ret;
    operation.setReturnValue(&ret);
    target->invoke(operation);
    return ret;
}

long double WSCompositeServiceTypeTestImpl_WSCompositeServiceTypeTestService_Proxy::doLongDouble( long double arg0)
{
    Operation operation("doLongDouble");
    operation.addParameter(&arg0);
    long double ret;
    operation.setReturnValue(&ret);
    target->invoke(operation);
    return ret;
}

double WSCompositeServiceTypeTestImpl_WSCompositeServiceTypeTestService_Proxy::doDouble( double arg0)
{
    Operation operation("doDouble");
    operation.addParameter(&arg0);
    double ret;
    operation.setReturnValue(&ret);
    target->invoke(operation);
    return ret;
}

bool WSCompositeServiceTypeTestImpl_WSCompositeServiceTypeTestService_Proxy::doBool( bool arg0)
{
    Operation operation("doBool");
    operation.addParameter(&arg0);
    bool ret;
    operation.setReturnValue(&ret);
    target->invoke(operation);
    return ret;
}

short WSCompositeServiceTypeTestImpl_WSCompositeServiceTypeTestService_Proxy::doShort( short arg0)
{
    Operation operation("doShort");
    operation.addParameter(&arg0);
    short ret;
    operation.setReturnValue(&ret);
    target->invoke(operation);
    return ret;
}

char* WSCompositeServiceTypeTestImpl_WSCompositeServiceTypeTestService_Proxy::doBytes( char* arg0)
{
    Operation operation("doBytes");
    operation.addParameter(&arg0);
    char* ret;
    operation.setReturnValue(&ret);
    target->invoke(operation);
    return ret;
}

char WSCompositeServiceTypeTestImpl_WSCompositeServiceTypeTestService_Proxy::doByte( char arg0)
{
    Operation operation("doByte");
    operation.addParameter(&arg0);
    char ret;
    operation.setReturnValue(&ret);
    target->invoke(operation);
    return ret;
}

DataObjectPtr WSCompositeServiceTypeTestImpl_WSCompositeServiceTypeTestService_Proxy::doMixed( char* arg0,  long arg1,  DataObjectPtr arg2,  bool arg3,  double arg4)
{
    Operation operation("doMixed");
    operation.addParameter(&arg0);
    operation.addParameter(&arg1);
    operation.addParameter(&arg2);
    operation.addParameter(&arg3);
    operation.addParameter(&arg4);
    DataObjectPtr ret;
    operation.setReturnValue(&ret);
    target->invoke(operation);
    return ret;
}

DataObjectPtr WSCompositeServiceTypeTestImpl_WSCompositeServiceTypeTestService_Proxy::doDataObject( DataObjectPtr arg0)
{
    Operation operation("doDataObject");
    operation.addParameter(&arg0);
    DataObjectPtr ret;
    operation.setReturnValue(&ret);
    target->invoke(operation);
    return ret;
}

DataObjectPtr WSCompositeServiceTypeTestImpl_WSCompositeServiceTypeTestService_Proxy::doAny( DataObjectPtr arg0)
{
    Operation operation("doAny");
    operation.addParameter(&arg0);
    DataObjectPtr ret;
    operation.setReturnValue(&ret);
    target->invoke(operation);
    return ret;
}

