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

#ifndef tuscany_sca_model_compositeservicetype_h
#define tuscany_sca_model_compositeservicetype_h

#include <string>
using std::string;
#include <vector>
using std::vector;

#include "tuscany/sca/model/Component.h"
#include "tuscany/sca/model/ReferenceType.h"

namespace tuscany
{
    namespace sca
    {
        namespace model
        {
            class Composite;
            class Component;
            class Interface;
            class Reference;

            /**
             * Represents a service in a composite.
             * Composite services define the public services provided by the composite, which can be
             * accessed from outside the composite.
             */
            class CompositeService : public Component
            {

            public:

                /**
                 * Constructor.
                 * @param componentType The component type on which this service is defined.
                 * @param name The name of the service.
                 */
                CompositeService(Composite* composite, const string& name,
                    Interface* interface, Interface* callbackInterface, bool conversational, ReferenceType::Multiplicity multiplicity);

                /**
                 * Destructor.
                 */
                virtual ~CompositeService();

                /**
                 * Returns the reference used by this composite service.
                 * @return The reference used by this composite service.
                 */
                Reference* getReference() const { return reference; };
                
            private:
            
                /**
                 * The reference used by this composite service.
                 */
                Reference* reference;
           };

        } // End namespace model
    } // End namespace sca
} // End namespace tuscany

#endif // tuscany_sca_model_compositeservicetype_h
