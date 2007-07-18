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

#include "CustomerInfoImpl_CustomerInfoService_Proxy.h"

#include "osoa/sca/sca.h"

extern "C"
{

    #if defined(WIN32) || defined(_WINDOWS)
    __declspec(dllexport) 
    #endif
    CustomerInfoImpl_CustomerInfoService_Proxy* CustomerInfoImpl_CustomerInfoService_Proxy_Factory(tuscany::sca::ServiceWrapper* target)
    {
        return new CustomerInfoImpl_CustomerInfoService_Proxy(target);
    }

    #if defined(WIN32) || defined(_WINDOWS)
    __declspec(dllexport) 
    #endif
    void CustomerInfoImpl_CustomerInfoService_Proxy_Destructor(void* proxy)
    {
        delete (CustomerInfoImpl_CustomerInfoService_Proxy*)proxy;
    }
}

CustomerInfoImpl_CustomerInfoService_Proxy::CustomerInfoImpl_CustomerInfoService_Proxy(tuscany::sca::ServiceWrapper* targ) : target(targ)
{
}

CustomerInfoImpl_CustomerInfoService_Proxy::~CustomerInfoImpl_CustomerInfoService_Proxy()
{
    if (target)
        delete target;
}

const char* CustomerInfoImpl_CustomerInfoService_Proxy::getCustomerInformationCharPublic( char* arg0, const char* arg1)
{
    tuscany::sca::Operation operation("getCustomerInformationCharPublic");
    operation.addParameter("p1", &arg0);
    operation.addParameter("customerID", &arg1);
    const char* ret;
    operation.setReturnValue(&ret);
    target->invoke(operation);
    return *(const char**)operation.getReturnValue();
}

const char* CustomerInfoImpl_CustomerInfoService_Proxy::getCustomerInfoACharPublic( char* arg0, const char* arg1)
{
    tuscany::sca::Operation operation("getCustomerInfoACharPublic");
    operation.addParameter("p1", &arg0);
    operation.addParameter("", &arg1);
    const char* ret;
    operation.setReturnValue(&ret);
    target->invoke(operation);
    return *(const char**)operation.getReturnValue();
}

const char* CustomerInfoImpl_CustomerInfoService_Proxy::getCustomerInfoBCharPublic( char* arg0,  char* arg1)
{
    tuscany::sca::Operation operation("getCustomerInfoBCharPublic");
    operation.addParameter("p1", &arg0);
    operation.addParameter("customerID", &arg1);
    const char* ret;
    operation.setReturnValue(&ret);
    target->invoke(operation);
    return *(const char**)operation.getReturnValue();
}

