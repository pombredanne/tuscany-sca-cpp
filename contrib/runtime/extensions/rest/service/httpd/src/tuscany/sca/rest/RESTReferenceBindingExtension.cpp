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

#include "RESTReferenceBindingExtension.h"
#include "model/RESTReferenceBinding.h"
#include "tuscany/sca/util/Logging.h"
#include "tuscany/sca/util/Utils.h"
#include "tuscany/sca/core/SCARuntime.h"

using namespace std;
using namespace commonj::sdo;
using namespace tuscany::sca::model;

extern "C"
{
#if defined(WIN32) || defined(_WINDOWS)
    __declspec(dllexport) 
#endif
        void tuscany_sca_rest_service_initialize()
    {
        tuscany::sca::rest::RESTReferenceBindingExtension::initialize();
    }
}

namespace tuscany
{
    namespace sca
    {
        namespace rest
        {
            // ===================================================================
            // Constructor for the RESTReferenceBinding class. 
            // ===================================================================
            RESTReferenceBindingExtension::RESTReferenceBindingExtension()
            { 
                logentry();
            }
            
            // ===================================================================
            // Destructor for the RESTReferenceBindingExtension class.
            // ===================================================================
            RESTReferenceBindingExtension::~RESTReferenceBindingExtension()
            { 
                logentry();
            }

            const string RESTReferenceBindingExtension::extensionName("rest");
            const string RESTReferenceBindingExtension::typeQName("http://www.osoa.org/xmlns/sca/1.0#RESTBinding");

            // ===================================================================
            // loadModelElement - load the info from binding.rest
            // ===================================================================
            ReferenceBinding* RESTReferenceBindingExtension::getReferenceBinding(Composite *composite, Reference* reference, DataObjectPtr scdlBinding)
            {
                string uri = scdlBinding->getCString("uri");

                RESTReferenceBinding* referenceBinding = new RESTReferenceBinding(reference, uri);
                
                return referenceBinding;
            }

           void RESTReferenceBindingExtension::initialize()
            { 
                logentry();
                SCARuntime::getCurrentRuntime()->registerReferenceBindingExtension(new RESTReferenceBindingExtension());
            }

        } // End namespace rest
    } // End namespace sca
} // End namespace tuscany