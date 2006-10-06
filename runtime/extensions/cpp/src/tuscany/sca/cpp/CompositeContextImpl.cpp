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

#include "tuscany/sca/util/Logging.h"
#include "osoa/sca/ServiceRuntimeException.h"
#include "tuscany/sca/cpp/CompositeContextImpl.h"
#include "tuscany/sca/model/Component.h"
#include "tuscany/sca/model/Composite.h"
#include "tuscany/sca/model/Service.h"
#include "tuscany/sca/model/ServiceBinding.h"
#include "tuscany/sca/model/ServiceType.h"
#include "tuscany/sca/core/ServiceProxy.h"
#include "tuscany/sca/core/ServiceWrapper.h"
#include "tuscany/sca/cpp/CPPServiceProxy.h"
#include "commonj/sdo/SDO.h"

using namespace tuscany::sca::model;
using namespace osoa::sca;
using namespace commonj::sdo;

namespace tuscany
{
    namespace sca
    {
        namespace cpp
        {

        // ===========
        // Constructor 
        // ===========
        CompositeContextImpl::CompositeContextImpl(Component* component)
            : CompositeContext(0), component(component),  composite((Composite*)component->getType())
        {
        }

        // ==========
        // Destructor
        // ==========
        CompositeContextImpl::~CompositeContextImpl()
        {
        }

        // ===========================================================================
        // locateService: return a proxy connected to a wrapper for the target service
        // ===========================================================================
        void* CompositeContextImpl::locateService(const std::string& serviceName)
        {
            LOGENTRY(1, "CompositeContextImpl::locateService");

            // ----------------------------
            // Locate the component service
            // ----------------------------
            Service* service = composite->findComponentService(serviceName);
            string msg;
            if (!service)
            {
                msg = "Service not found: ";
                msg = msg + serviceName;
                throw ServiceNotFoundException(msg.c_str());
            }

            // ----------------------------
            // Get a Proxy for this service
            // ----------------------------

            // The locate service API is used from CPP clients so we are using
            // our default service proxy here
            CPPServiceProxy* serviceProxy =  new CPPServiceProxy(service);
            LOGEXIT(1, "CompositeContextImpl::locateService");
            return serviceProxy->getProxy();

        }
        
        // ==============================================
        // getDataFactory: return the data factory for the current composite
        // ==============================================
        DataFactoryPtr CompositeContextImpl::getDataFactory()
        {
            LOGENTRY(1, "CompositeContextImpl::getDataFactory");
            DataFactoryPtr dataFactory = composite->getDataFactory();
            
            LOGEXIT(1, "CompositeContextImpl::getDataFactory");
            return dataFactory;
        }

       } // End namespace cpp
    } // End namespace sca
} // End namespace tuscany