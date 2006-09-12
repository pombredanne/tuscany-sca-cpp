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

/* $Rev$ $Date$ */

#include "tuscany/sca/util/Logging.h"
#include "tuscany/sca/model/CompositeReference.h"
#include "tuscany/sca/model/ReferenceType.h"
#include "tuscany/sca/model/Composite.h"
#include "tuscany/sca/model/Component.h"
#include "tuscany/sca/model/ComponentType.h"
#include "tuscany/sca/model/ServiceType.h"
#include "tuscany/sca/model/Service.h"

namespace tuscany
{
    namespace sca
    {

        namespace model
        {

            // Constructor
            CompositeReference::CompositeReference(Composite* composite, const string& name,
                    Interface* intface, Interface* callbackInterface, bool conversational, ReferenceType::Multiplicity multiplicity) :
                Component(composite, name, new ComponentType(composite, name))
            {
                LOGENTRY(1, "CompositeReference::constructor");
                LOGINFO_1(2, "CompositeReference::constructor: CompositeReference name: %s", name.c_str());
                
                // Initialize the component type, service type and service
                ComponentType* componentType = getType();
                ServiceType* serviceType = new ServiceType(
                    componentType, "", intface, callbackInterface);
                componentType->addServiceType(serviceType);
                
                service = new Service(this, serviceType);
                addService(service);
                
                LOGEXIT(1, "CompositeReference::constructor");
            }

            // Destructor
            CompositeReference::~CompositeReference()
            {
            }

        } // End namespace model

    } // End namespace sca
} // End namespace tuscany
