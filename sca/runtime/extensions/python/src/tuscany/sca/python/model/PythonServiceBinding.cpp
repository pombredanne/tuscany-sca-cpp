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

#include "tuscany/sca/util/Logging.h"
#include "tuscany/sca/python/model/PythonServiceBinding.h"
#include "tuscany/sca/python/PythonServiceWrapper.h"

namespace tuscany
{
    namespace sca
    {
        namespace python
        {

            // Constructor
            PythonServiceBinding::PythonServiceBinding(Service* service)
                : ServiceBinding(service, "")
            {
                LOGENTRY(1,"PythonServiceBinding::constructor");
                serviceWrapper = new PythonServiceWrapper(service);
                LOGEXIT(1,"PythonServiceBinding::constructor");
            }

            // Destructor
            PythonServiceBinding::~PythonServiceBinding()
            {
                LOGENTRY(1,"PythonServiceBinding::destructor");
                LOGEXIT(1,"PythonServiceBinding::destructor");
            }
            
            ServiceWrapper* PythonServiceBinding::getServiceWrapper()
            {
                LOGENTRY(1,"PythonServiceBinding::getServiceWrapper");
                LOGEXIT(1,"PythonServiceBinding::getServiceWrapper");
                return (ServiceWrapper*)serviceWrapper;
            }
                
        } // End namespace ws
    } // End namespace sca
} // End namespace tuscany
