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

#include "commonj/sdo/SDO.h"

#include "osoa/sca/ServiceRuntimeException.h"

#include "tuscany/sca/util/Logging.h"
#include "tuscany/sca/cpp/CompositeContextImpl.h"
#include "tuscany/sca/model/Component.h"
#include "tuscany/sca/model/Composite.h"
#include "tuscany/sca/model/Service.h"
#include "tuscany/sca/model/ServiceBinding.h"
#include "tuscany/sca/model/ServiceType.h"
#include "tuscany/sca/core/ServiceProxy.h"
#include "tuscany/sca/core/ServiceWrapper.h"
#include "tuscany/sca/cpp/CPPServiceProxy.h"

using namespace std;
using namespace commonj::sdo;
using namespace osoa::sca;
using namespace tuscany::sca::model;

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
            logentry();
        }

        // ==========
        // Destructor
        // ==========
        CompositeContextImpl::~CompositeContextImpl()
        {
            logentry();
        }

        // ===========================================================================
        // locateService: return a proxy connected to a wrapper for the target service
        // ===========================================================================
        void* CompositeContextImpl::locateService(const std::string& serviceName)
        {
            logentry();
            try
            {
                // ----------------------------
                // Locate the component service
                // ----------------------------
                Service* service = composite->findComponentService(serviceName);
                string msg;
                if (!service)
                {
                    msg = "Service not found: ";
                    msg = msg + serviceName;
                    throwException(ServiceNotFoundException, msg.c_str());
                }
    
                // ----------------------------
                // Get a Proxy for this service
                // ----------------------------
    
                // The locate service API is used from CPP clients so we are using
                // our default service proxy here
                CPPServiceProxy* serviceProxy =  new CPPServiceProxy(service);
                return serviceProxy->getProxy();
            }
            catch (ServiceRuntimeException&)
            {
                throw;
            }
            catch (TuscanyRuntimeException& e)
            {
                throwException(ServiceRuntimeException, e);
            }
        }
        
        // ==============================================
        // getDataFactory: return the data factory for the current composite
        // ==============================================
        DataFactoryPtr CompositeContextImpl::getDataFactory()
        {
            logentry();
            try
            {
                DataFactoryPtr dataFactory = composite->getDataFactory();
                return dataFactory;
            }
            catch (ServiceRuntimeException&)
            {
                throw;
            }
            catch (TuscanyRuntimeException& e)
            {
                throwException(ServiceRuntimeException, e);
            }
        }

       } // End namespace cpp
    } // End namespace sca
} // End namespace tuscany