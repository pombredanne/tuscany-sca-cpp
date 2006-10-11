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

#ifndef tuscany_sca_extension_ws_wsserviceproxy_h
#define tuscany_sca_extension_ws_wsserviceproxy_h

#include "tuscany/sca/core/ServiceProxy.h" 
#include "tuscany/sca/core/ServiceWrapper.h" 
#include "tuscany/sca/model/Component.h"
#include "tuscany/sca/model/Reference.h"
#include "tuscany/sca/model/Service.h"
#include "tuscany/sca/model/WSDLOperation.h"
#include "model/WSReferenceBinding.h"
#include "commonj/sdo/SDO.h"

using namespace tuscany::sca::model;

using commonj::sdo::DataObjectPtr;
using commonj::sdo::DataFactoryPtr;

namespace tuscany
{
    namespace sca
    {
        namespace ws
        {
            
            /**
             * Holds a proxy for a given component and reference.
             * The proxy which is held inside a ServiceProxy will be specific to the programming
             * interface expected by the client. In this particular case the client is an Axis2 
             * Web service skeleton.
             */
            class WSServiceProxy : public ServiceProxy
            {
            public:
                /**
                 * Create a new service proxy for a reference. The proxy will contain a pointer to
                 * the target ServiceWrapper.
                 * @param reference The reference on the source component.
                 * @param target The wrapper of the service which is wired to this reference.
                 */
                WSServiceProxy(Reference* reference);
    
                /**
                 * Create a new service proxy for a service. The proxy will contain a pointer to
                 * the target ServiceWrapper.
                 * @param reference The service on the target component.
                 * @param target The wrapper of the target service.
                 */
                WSServiceProxy(Service* service);
    
                /**
                 * Destructor.
                 */
                virtual ~WSServiceProxy();
    
                /**
                 * Invoke the specified operation
                 */
                DataObjectPtr invoke(const WSDLOperation& wsdlOperation, DataObjectPtr inputDataObject);    
    
            private:

                void setOutputData(Operation& operation, DataObjectPtr outputDataObject, DataFactoryPtr dataFactoryPtr);
                
                /**
                 * The target service wrapper
                 */
                 ServiceWrapper* serviceWrapper;
            
            };
            
        } // End namespace ws
    } // End namespace sca
} // End namespace tuscany

#endif // tuscany_sca_extension_ws_wsserviceproxy_h