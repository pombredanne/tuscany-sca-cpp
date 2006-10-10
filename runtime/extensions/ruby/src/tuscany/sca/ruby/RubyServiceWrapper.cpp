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


#include "tuscany/sca/ruby/RubyServiceWrapper.h"

#include "tuscany/sca/util/Logging.h"
#include "tuscany/sca/util/Utils.h"
#include "tuscany/sca/util/Library.h"
#include "tuscany/sca/model/Component.h"
#include "tuscany/sca/model/Composite.h"
#include "tuscany/sca/model/Service.h"
#include "tuscany/sca/model/ServiceType.h"
#include "tuscany/sca/model/Reference.h"
#include "tuscany/sca/model/ReferenceType.h"
#include "tuscany/sca/core/SCARuntime.h"
#include "tuscany/sca/ruby/model/RubyImplementation.h"
#include "tuscany/sca/ruby/model/RubyServiceBinding.h"
#include "tuscany/sca/ruby/RubyServiceProxy.h"
#include "commonj/sdo/SDO.h"

using commonj::sdo::PropertyList;

namespace tuscany
{
    namespace sca
    {
        namespace ruby
        {
            
            // ===========
            // Constructor
            // ===========
            RubyServiceWrapper::RubyServiceWrapper(Service* service)
                : ServiceWrapper(service)
            {
                LOGENTRY(1,"RubyServiceWrapper::constructor");
                
                component = service->getComponent();
                implementation = (RubyImplementation*)component->getType();
                interf = service->getType()->getInterface();
                
                LOGEXIT(1,"RubyServiceWrapper::constructor");
                
            }
            
            // ==========
            // Destructor
            // ==========
            RubyServiceWrapper::~RubyServiceWrapper()
            {
                LOGENTRY(1,"RubyServiceWrapper::destructor");
                LOGEXIT(1,"RubyServiceWrapper::destructor");
            }
            
            // ======================================================================
            // invoke: wrapper call to service with setting the component context
            // ======================================================================
            void RubyServiceWrapper::invoke(Operation& operation)
            {
                LOGENTRY(1,"RubyServiceWrapper::invoke");
    
                SCARuntime* runtime = SCARuntime::getInstance();
                runtime->setCurrentComponent(component);
                
                try
                {

                    // Create a new instance of the Ruby implementation class
                    VALUE instance = rb_class_new_instance(0, NULL, implementation->getImplementationClass());

                    // Set all the references
                    const Component::REFERENCE_MAP& references = component->getReferences();
                    Component::REFERENCE_MAP::const_iterator refiter = references.begin();
                    for (int ri=0; ri< references.size(); ri++)
                    {
                        Reference* reference = refiter->second;
                        RubyServiceProxy* proxy = (RubyServiceProxy*)reference->getBinding()->getServiceProxy();
                        if (proxy != NULL)
                        {
                            VALUE proxyValue = proxy->getProxyValue();
                            string varName = "@" + refiter->first; 
                            rb_iv_set(instance, varName.c_str(), proxyValue);
                        }
                        refiter++;
                    }
                    
                    // Set all the configured properties
                    DataObjectPtr properties = component->getProperties();
                    PropertyList pl = properties->getInstanceProperties();
                    for (int i = 0; i < pl.size(); i++)
                    {
                        if (properties->isSet(pl[i]))
                        {
                            string varName = "@";
                            varName += pl[i].getName();
                            string cstr = properties->getCString(pl[i]);
                            VALUE propertyValue;
                            if (cstr == "true")
                            {
                                propertyValue = Qtrue;
                            }
                            else if (cstr == "false")
                            {
                                propertyValue = Qfalse;
                            }
                            else
                            {
                                //TODO use one of the rb_str_to_inum() functions
                                // to convert a numeric value to a Ruby numeric
                                propertyValue = rb_str_new2(cstr.c_str());
                            }  
                            rb_iv_set(instance, varName.c_str(), propertyValue);
                        }
                    }
                    
                    // Get the ID of the specified method
                    ID method = rb_intern(operation.getName().c_str());

                    // Convert C++ parameters to Ruby parameters
                    VALUE *args = NULL;
                    int n = operation.getNParms();
                    if (n != 0)
                    {
                        args=new VALUE[n];
                        
                        for(int i = 0; i < operation.getNParms(); i++) 
                        {
                            VALUE value;
                            
                            Operation::Parameter& parm = operation.getParameter(i);
                            Operation::ParameterType parmType = parm.getType();
                            switch(parmType)
                            {
                                case Operation::BOOL: 
                                {
                                    if( *(bool*)parm.getValue())
                                    {
                                        //boolean true
                                        value = rb_int2inum(1);
                                    }
                                    else
                                    {
                                        value = rb_int2inum(0);
                                    }
                                    break;
                                }
                                case Operation::SHORT: 
                                {
                                    value = rb_int2inum(*(short*)parm.getValue());
                                    break;
                                }
                                case Operation::USHORT: 
                                {
                                    value = rb_uint2inum(*(unsigned short*)parm.getValue());
                                    break;
                                }
                                case Operation::LONG: 
                                {
                                    value = rb_int2inum(*(long*)parm.getValue());
                                    break;
                                }
                                case Operation::ULONG: 
                                {
                                    value = rb_uint2inum(*(unsigned long*)parm.getValue());
                                    break;
                                }
                                case Operation::FLOAT: 
                                {
                                    value = rb_float_new(*(float*)parm.getValue());
                                    break;
                                }
                                case Operation::DOUBLE: 
                                {
                                    value = rb_float_new(*(double*)parm.getValue());
                                    break;
                                }
                                case Operation::LONGDOUBLE: 
                                {
                                    value = rb_float_new(*(long double*)parm.getValue());
                                    break;
                                }
                                case Operation::CHARS: 
                                {
                                    value = rb_str_new2(*(char**)parm.getValue());
                                    break;
                                }
                                case Operation::STRING: 
                                {
                                    value = rb_str_new2((*(string*)parm.getValue()).c_str());
                                    break;
                                }
                                case Operation::DATAOBJECT: 
                                {
                                    DataObjectPtr dob = *(DataObjectPtr*)parm.getValue();

                                    // Convert a DataObject to a REXML Document object
                                    Composite* composite = component->getComposite();                                    
                                    commonj::sdo::XMLHelper* xmlHelper = composite->getXMLHelper();
                                    char* str = xmlHelper->save(
                                        dob,
                                        dob->getType().getURI(),
                                        dob->getType().getName());
                                    VALUE vstr[1];
                                    vstr[0] = rb_str_new2(str);

                                    value = rb_class_new_instance(1, vstr, RubyImplementation::getXMLDocumentClass());
                                    break;
                                }
                                default:
                                {
                                    //throw new ComponentInvocationException("Operation parameter type not supported");
                                    string msg = "Operation parameter type not supported" + parmType;
                                    throw msg.c_str();
                                }
                            }
                            
                            args[i] = value;
                        }
                    }                    

                    
                    // Invoke the specified method
                    VALUE result;
                    if (n == 0)
                    {
                        result = rb_funcall(instance, method, 0);
                    }
                    else
                    {
                        result = rb_funcall2(instance, method, n, args);
                    }
                    
                    // Convert the Ruby result value to a C++ result
                    int resultType = TYPE(result);
                    char buf[20];
                    switch(resultType)
                    {
                    case T_FLOAT: 
                        {
                            float* data = new float; 
                            *data = rb_num2dbl(result);
                            
                            // Check if the return type has already been set for typed languages
    		                switch(operation.getReturnType())
    		                {
    			                case Operation::BOOL: 
    			                {
    				                *(bool*)operation.getReturnValue() = (*data != 0.0);
    				                break;
    			                }
    			                case Operation::SHORT: 
    			                {
                                    *(short*)operation.getReturnValue() = (short)*data;
    				                break;
    			                }
    			                case Operation::USHORT: 
    			                {
                                    *(unsigned short*)operation.getReturnValue() = (unsigned short)*data;
    				                break;
    			                }
    			                case Operation::INT: 
    			                {
                                    *(int*)operation.getReturnValue() = (int)*data;
    				                break;
    			                }
    			                case Operation::UINT: 
    			                {
                                    *(unsigned int*)operation.getReturnValue() = (unsigned int)*data;
    				                break;
    			                }
    			                case Operation::LONG: 
    			                {
                                    *(long*)operation.getReturnValue() = (long)*data;
    				                break;
    			                }
    			                case Operation::ULONG: 
    			                {
                                    *(unsigned long*)operation.getReturnValue() = (unsigned long)*data;
    				                break;
    			                }
    			                case Operation::FLOAT: 
    			                {
                                    *(float*)operation.getReturnValue() = (float)*data;
    				                break;
    			                }
    			                case Operation::DOUBLE: 
    			                {
                                    *(double*)operation.getReturnValue() = (double)*data;
    				                break;
    			                }
    			                case Operation::LONGDOUBLE: 
    			                {
                                    *(long double*)operation.getReturnValue() = (long double)*data;
    				                break;
    			                }
    			                case Operation::CHARS: 
    			                {
                                    sprintf(buf, "%f", *data);
                                    *(char**)operation.getReturnValue() = buf;
    				                break;
    			                }
    			                case Operation::STRING: 
    			                {
                                    sprintf(buf, "%f", *data);
                                    *(string*)operation.getReturnValue() = buf;
    				                break;
    			                }
                                default:
                                {
                                    // The type is set as something else or has not been set
                                    operation.setReturnValue(data);
                                }
                            }
                            break;
                        }
                    case T_STRING: 
                        {
                            string* stringdata = new string(rb_string_value_cstr(&result));
                            const char** data = new const char*; 
                            *data = stringdata->c_str();

                            // Check if the return type has already been set (for typed languages)
    		                switch(operation.getReturnType())
    		                {
    			                case Operation::BOOL: 
    			                {
                                    // If the string is empty or "0" or "false" set to false, otherwise true
                                    if(strlen(*data) == 0 || strcmp(*data, "0") == 0 || strcmp(*data, "false") == 0)
                                    {
    				                    *(bool*)operation.getReturnValue() = false;
                                    }
                                    else
                                    {
                                        *(bool*)operation.getReturnValue() = true;
                                    }
    				                break;
    			                }
    			                case Operation::SHORT: 
    			                {
                                    *(short*)operation.getReturnValue() = (short)atoi(*data);
    				                break;
    			                }
    			                case Operation::USHORT: 
    			                {
                                    *(unsigned short*)operation.getReturnValue() = (unsigned short)atoi(*data);
    				                break;
    			                }
    			                case Operation::INT: 
    			                {
                                    *(int*)operation.getReturnValue() = (int)atoi(*data);
    				                break;
    			                }
    			                case Operation::UINT: 
    			                {
                                    *(unsigned int*)operation.getReturnValue() = (unsigned int)atoi(*data);
    				                break;
    			                }
    			                case Operation::LONG: 
    			                {
                                    *(long*)operation.getReturnValue() = (long)atol(*data);
    				                break;
    			                }
    			                case Operation::ULONG: 
    			                {
                                    *(unsigned long*)operation.getReturnValue() = (unsigned long)atol(*data);
    				                break;
    			                }
    			                case Operation::FLOAT: 
    			                {
                                    *(float*)operation.getReturnValue() = (float)atof(*data);
    				                break;
    			                }
    			                case Operation::DOUBLE: 
    			                {
                                    *(double*)operation.getReturnValue() = (double)atof(*data);
    				                break;
    			                }
    			                case Operation::LONGDOUBLE: 
    			                {
                                    *(long double*)operation.getReturnValue() = (long double)atof(*data);
    				                break;
    			                }
    			                case Operation::CHARS: 
    			                {
                                    *(const char**)operation.getReturnValue() = *data;
    				                break;
    			                }
    			                case Operation::STRING: 
    			                {
                                    *(string*)operation.getReturnValue() = *data;
    				                break;
    			                }
                                default:
                                {
                                    // The type is set as something else or has not been set
                                    operation.setReturnValue(data);
                                }
                            }
                            break;
                        }
                    case T_FIXNUM: 
                        {
                            long* data = new long;
                            *data = rb_num2long(result);
                                                            
                            // Check if the return type has already been (set for typed languages)
    		                switch(operation.getReturnType())
    		                {
    			                case Operation::BOOL: 
    			                {
    				                *(bool*)operation.getReturnValue() = (*data != 0);
    				                break;
    			                }
    			                case Operation::SHORT: 
    			                {
                                    *(short*)operation.getReturnValue() = (short)*data;
    				                break;
    			                }
    			                case Operation::USHORT: 
    			                {
                                    *(unsigned short*)operation.getReturnValue() = (unsigned short)*data;
    				                break;
    			                }
    			                case Operation::INT: 
    			                {
                                    *(int*)operation.getReturnValue() = (int)*data;
    				                break;
    			                }
    			                case Operation::UINT: 
    			                {
                                    *(unsigned int*)operation.getReturnValue() = (unsigned int)*data;
    				                break;
    			                }
    			                case Operation::LONG: 
    			                {
                                    *(long*)operation.getReturnValue() = (long)*data;
    				                break;
    			                }
    			                case Operation::ULONG: 
    			                {
                                    *(unsigned long*)operation.getReturnValue() = (unsigned long)*data;
    				                break;
    			                }
    			                case Operation::FLOAT: 
    			                {
                                    *(float*)operation.getReturnValue() = (float)*data;
    				                break;
    			                }
    			                case Operation::DOUBLE: 
    			                {
                                    *(double*)operation.getReturnValue() = (double)*data;
    				                break;
    			                }
    			                case Operation::LONGDOUBLE: 
    			                {
                                    *(long double*)operation.getReturnValue() = (long double)*data;
    				                break;
    			                }
    			                case Operation::CHARS: 
    			                {
                                    sprintf(buf, "%d", *data);
                                    *(char**)operation.getReturnValue() = buf;
    				                break;
    			                }
    			                case Operation::STRING: 
    			                {
                                    sprintf(buf, "%d", *data);
                                    *(string*)operation.getReturnValue() = buf;
    				                break;
    			                }
                                default:
                                {
                                    // The type is set as something else or has not been set
                                    operation.setReturnValue(data);
                                }
    		                }

                            break;
                        }
                    case T_BIGNUM: 
                        {
                            long double* data = new long double; 
                            *data = rb_num2dbl(result);

                            // Check if the return type has already been set (for typed languages)
    		                switch(operation.getReturnType())
    		                {
    			                case Operation::BOOL: 
    			                {
    				                *(bool*)operation.getReturnValue() = (*data != 0.0);
    				                break;
    			                }
    			                case Operation::SHORT: 
    			                {
                                    *(short*)operation.getReturnValue() = (short)*data;
    				                break;
    			                }
    			                case Operation::USHORT: 
    			                {
                                    *(unsigned short*)operation.getReturnValue() = (unsigned short)*data;
    				                break;
    			                }
    			                case Operation::INT: 
    			                {
                                    *(int*)operation.getReturnValue() = (int)*data;
    				                break;
    			                }
    			                case Operation::UINT: 
    			                {
                                    *(unsigned int*)operation.getReturnValue() = (unsigned int)*data;
    				                break;
    			                }
    			                case Operation::LONG: 
    			                {
                                    *(long*)operation.getReturnValue() = (long)*data;
    				                break;
    			                }
    			                case Operation::ULONG: 
    			                {
                                    *(unsigned long*)operation.getReturnValue() = (unsigned long)*data;
    				                break;
    			                }
    			                case Operation::FLOAT: 
    			                {
                                    *(float*)operation.getReturnValue() = (float)*data;
    				                break;
    			                }
    			                case Operation::DOUBLE: 
    			                {
                                    *(double*)operation.getReturnValue() = (double)*data;
    				                break;
    			                }
    			                case Operation::LONGDOUBLE: 
    			                {
                                    *(long double*)operation.getReturnValue() = (long double)*data;
    				                break;
    			                }
    			                case Operation::CHARS: 
    			                {
                                    sprintf(buf, "%f", *data);
                                    *(char**)operation.getReturnValue() = buf;
    				                break;
    			                }
    			                case Operation::STRING: 
    			                {
                                    sprintf(buf, "%f", *data);
                                    *(string*)operation.getReturnValue() = buf;
    				                break;
    			                }
                                default:
                                {
                                    // The type is set as something else or has not been set
                                    operation.setReturnValue(data);
                                }
                            }
                            break;
                        }
                    case T_TRUE: 
                    case T_FALSE: 
                        {
                            bool* data = new bool;
                            *data = (resultType == T_TRUE); 
                            // Check if the return type has already been set (for typed languages)                                
    		                switch(operation.getReturnType())
    		                {
    			                case Operation::BOOL: 
    			                {
    				                *(bool*)operation.getReturnValue() = *data;
    				                break;
    			                }
    			                case Operation::SHORT: 
    			                {
                                    *(short*)operation.getReturnValue() = (short)*data;
    				                break;
    			                }
    			                case Operation::USHORT: 
    			                {
                                    *(unsigned short*)operation.getReturnValue() = (unsigned short)*data;
    				                break;
    			                }
    			                case Operation::INT: 
    			                {
                                    *(int*)operation.getReturnValue() = (int)*data;
    				                break;
    			                }
    			                case Operation::UINT: 
    			                {
                                    *(unsigned int*)operation.getReturnValue() = (unsigned int)*data;
    				                break;
    			                }
    			                case Operation::LONG: 
    			                {
                                    *(long*)operation.getReturnValue() = (long)*data;
    				                break;
    			                }
    			                case Operation::ULONG: 
    			                {
                                    *(unsigned long*)operation.getReturnValue() = (unsigned long)*data;
    				                break;
    			                }
    			                case Operation::FLOAT: 
    			                {
                                    *(float*)operation.getReturnValue() = (float)*data;
    				                break;
    			                }
    			                case Operation::DOUBLE: 
    			                {
                                    *(double*)operation.getReturnValue() = (double)*data;
    				                break;
    			                }
    			                case Operation::LONGDOUBLE: 
    			                {
                                    *(long double*)operation.getReturnValue() = (long double)*data;
    				                break;
    			                }
    			                case Operation::CHARS: 
    			                {
                                    if(*data)
                                    {
                                        *(char**)operation.getReturnValue() = "true";
                                    }
                                    else
                                    {
                                        *(char**)operation.getReturnValue() = "false";
                                    }
    				                break;
    			                }
    			                case Operation::STRING: 
    			                {
                                    if(*data)
                                    {
                                        *(string*)operation.getReturnValue() = "true";
                                    }
                                    else
                                    {
                                        *(string*)operation.getReturnValue() = "false";
                                    }
    				                break;
    			                }
                                default:
                                {
                                    // The type is set as something else or has not been set
                                    operation.setReturnValue(data);
                                }
    		                }

                            break;
                        }
                    case T_OBJECT: 
                        {
                            VALUE klass = rb_obj_class(result);
                            if (klass == RubyImplementation::getXMLDocumentClass())
                            {
                                // Convert a REXML::Document to a DataObject
                                ID to_s = rb_intern("to_s");
                                VALUE vstr = rb_funcall(result, to_s, 0);
                                string str = string(rb_string_value_cstr(&vstr));
                                                                
                                Composite* composite = component->getComposite();                                   
                                commonj::sdo::XMLHelper* xmlHelper = composite->getXMLHelper();
                                commonj::sdo::XMLDocumentPtr xmlDoc = xmlHelper->load(str.c_str());
                                
                                DataObjectPtr* dob = new DataObjectPtr;
                                if (xmlDoc != NULL)
                                {
                                    *dob = xmlDoc->getRootDataObject();
                                }
                                if (*dob != NULL)
                                {
                                    operation.setReturnValue(dob);
                                }
                                else
                                {
                                    string msg = "Document could not be converted to a DataObject";
                                    throw msg.c_str();
                                }
                            }
                            else
                            {
                                string msg = "Ruby type not supported: " + resultType;
                                throw msg.c_str();
                            }
                            break;
                        }
                    default:
                        {
                            string msg = "Ruby type not supported: " + resultType;
                            throw msg.c_str();
                        } 
                    }
                    
                }
                catch (...)
                {
                    runtime->unsetCurrentComponent();
                    throw;
                }
                runtime->unsetCurrentComponent();
                LOGEXIT(1,"RubyServiceWrapper::invoke");
                
            }
            
        } // End namespace ruby        
    } // End namespace sca
} // End namespace tuscany
