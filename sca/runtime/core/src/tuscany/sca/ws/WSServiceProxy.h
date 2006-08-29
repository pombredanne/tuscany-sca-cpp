/*
 *
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

/* $Rev: 437730 $ $Date: 2006-08-28 08:57:52 -0700 (Mon, 28 Aug 2006) $ */

#ifndef tuscany_sca_extension_ws_wsserviceproxy_h
#define tuscany_sca_extension_ws_wsserviceproxy_h

#include "osoa/sca/export.h"
#include "tuscany/sca/core/ServiceProxy.h" 
#include "tuscany/sca/core/ServiceWrapper.h" 
#include "tuscany/sca/model/Component.h"
#include "tuscany/sca/model/Reference.h"
#include "tuscany/sca/model/Service.h"
#include "tuscany/sca/ws/WSReferenceBinding.h"
#include "commonj/sdo/SDO.h"

using namespace tuscany::sca::model;

using commonj::sdo::DataObjectPtr;

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
                 * Return an instance of the proxy created for this particular component and reference.
                 * @return The proxy.
                 */
                virtual void* getProxy();
                
                /**
                 * Return the proxies created for this particular component and reference.
                 * @return The proxies.
                 */
                virtual PROXIES getProxies();

                /**
                 * Invoke the specified operation
                 */
                DataObjectPtr invoke(const char* operationName, DataObjectPtr inputDataObject);    
    
            private:

                void setOutputData(Operation operation, DataObjectPtr outputDataObject);
                
                /**
                 * The target service wrapper
                 */
                 ServiceWrapper* serviceWrapper;
            
                /**
                 * Holds the instances of the WS proxies.
                 */ 
                PROXIES proxies;
    
            };
            
        } // End namespace ws
    } // End namespace sca
} // End namespace tuscany

#endif // tuscany_sca_extension_ws_wsserviceproxy_h
