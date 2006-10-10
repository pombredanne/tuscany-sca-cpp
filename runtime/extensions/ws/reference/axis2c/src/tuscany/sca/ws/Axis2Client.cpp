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


#if defined(WIN32)  || defined (_WINDOWS)
#pragma warning(disable: 4091)
#pragma warning(disable: 4786)
#endif

#include <axis2_client.h>

#include <axis2_error_default.h>
#include <axis2_log_default.h>
#include <axis2_defines.h>
#include <axiom_soap_const.h>
#include <platforms/axis2_platform_auto_sense.h>

#include <sdo_axiom.h>
#include "Axis2Client.h"

#include "tuscany/sca/core/SCARuntime.h"
#include "tuscany/sca/util/Logging.h"
#include "model/WSServiceBinding.h"
#include "tuscany/sca/util/Utils.h"
#include "tuscany/sca/util/Exceptions.h"
#include "tuscany/sca/model/Service.h"
#include "tuscany/sca/model/ServiceType.h"
#include "tuscany/sca/model/Composite.h"
#include "tuscany/sca/model/WSDLDefinition.h"
#include "tuscany/sca/model/WSDLInterface.h"
#include "tuscany/sca/model/WSDLOperation.h"

#include <sstream>

using namespace tuscany::sca::ws;


using namespace commonj::sdo;
using namespace commonj::sdo_axiom;

namespace tuscany
{
    namespace sca
    {
        namespace ws
        {
            
            Axis2Client::Axis2Client(CompositeReference* compositeReference)
                : compositeReference(compositeReference)
            {
            }
            
            Axis2Client::~Axis2Client()
            {
            }
            
            void Axis2Client::invoke(tuscany::sca::Operation& operation)
            {
                LOGENTRY(1, "Axis2Client::invoke");

                // Initialize Axis2 stuff
                axis2_allocator_t *allocator = axis2_allocator_init (NULL);
                axis2_log_t *log = axis2_log_create(allocator, NULL, "tuscany_client.log");
                axis2_error_t *error = axis2_error_create(allocator);
                axis2_env_t *env = axis2_env_create_with_error_log(allocator, error, log);
                env->log->level = AXIS2_LOG_LEVEL_TRACE;
                axis2_error_init();
                
                // Get the WS service binding and WSDLOperation
                Composite* composite=compositeReference->getComposite();
                Service* service = compositeReference->getService();
                WSServiceBinding* binding = (WSServiceBinding *)service->getBinding();
                const string& operationName = operation.getName();
                WSDLOperation wsdlOperation;
                
                // Get the WSDL namespace
                string wsdlNamespace = binding->getWSDLNamespaceURL();
                if (wsdlNamespace != "")
                {
                
                    // Lookup the wsdl model from the composite, keyed on the namespace 
                    // (the wsdl will have been loaded at startup)
                    WSDLDefinition* wsdlDefinition = composite->findWSDLDefinition(wsdlNamespace);
                    if (wsdlDefinition == 0)
                    {
                        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI,  "WSDL description %s not found\n", wsdlNamespace.c_str());
                        string msg = "WSDL not found for " + wsdlNamespace;
                        throw SystemConfigurationException(msg.c_str());
                    }
                    
                    // Match the operation in Operation to the operation in the wsdl port type.
                    try
                    {
                        wsdlOperation =  wsdlDefinition->findOperation(
                            binding->getServiceName(),
                            binding->getEndpointName(),
                            operationName);
                    }
                    catch(SystemConfigurationException &ex)
                    {   
                        AXIS2_LOG_ERROR((env)->log, AXIS2_LOG_SI, "SystemConfigurationException has been caught: %s\n", ex.getMessageText());
                        throw;
                    }
                    
                }
                else
                {
                    Interface* iface = service->getType()->getInterface();
                    if (iface != NULL &&
                        iface->getInterfaceTypeQName() == WSDLInterface::typeQName)
                    {
                        WSDLInterface* wsdlInterface = (WSDLInterface*)iface;
                        wsdlNamespace = wsdlInterface->getNamespaceURI();
                        
                        if (wsdlNamespace != "")
                        {
                            
                            WSDLDefinition* wsdl = composite->findWSDLDefinition(wsdlNamespace);
                            if (wsdl == 0)
                            {
                                AXIS2_LOG_ERROR((env)->log, AXIS2_LOG_SI, "WSDL description %s not found\n", wsdlNamespace.c_str());
                                string msg = "WSDL not found for " + wsdlNamespace;
                                throw SystemConfigurationException(msg.c_str());
                            }
                    
                            try
                            {
                                wsdlOperation = wsdl->findOperation(wsdlInterface->getName(), operationName);
                            }
                            catch(SystemConfigurationException &ex)
                            {   
                                AXIS2_LOG_ERROR((env)->log, AXIS2_LOG_SI, "SystemConfigurationException has been caught: %s\n", ex.getMessageText());
                                throw;
                            }
                        }
                    }
                }
                
                if (wsdlNamespace == "")
                {
                    
                    // Create a default document literal wrapped WSDL operation
                    wsdlNamespace = compositeReference->getName();
                    wsdlOperation = WSDLOperation();
                    wsdlOperation.setOperationName(operationName);
                    wsdlOperation.setSoapAction(wsdlNamespace+ "#" +operationName);
                    wsdlOperation.setEndpoint("");
                    wsdlOperation.setSoapVersion(WSDLOperation::SOAP11);
                    wsdlOperation.setDocumentStyle(true);
                    wsdlOperation.setWrappedStyle(true);
                    wsdlOperation.setEncoded(false);
                    wsdlOperation.setInputType(wsdlNamespace + "#" + operationName);
                    wsdlOperation.setOutputType(wsdlNamespace + "#" + operationName + "Response");
                }
                    
                // Get the target endpoint address
                // The URI specified in the binding overrides the address specified in
                // the WSDL
                axis2_char_t* address;
                if (binding->getURI() != "")
                {
                    address = (axis2_char_t*)binding->getURI().c_str();
                }
                else
                {
                    address = (axis2_char_t*)wsdlOperation.getEndpoint().c_str();
                }
                
                axis2_char_t* opName = (axis2_char_t*)operationName.c_str();
                axis2_char_t* soap_action = (axis2_char_t*)wsdlOperation.getSoapAction().c_str();
                axis2_char_t* serviceName = (axis2_char_t*)binding->getServiceName().c_str();
                
                // create OM from Operation and wsdlOperation
                axiom_node_t* payload = createPayload(operation, wsdlOperation, env);
                
                /* Create EPR with given address */
                axis2_endpoint_ref_t* endpoint_ref = axis2_endpoint_ref_create(env, address);
                
                /* Setup options */
                axis2_options_t* options = axis2_options_create(env);
                AXIS2_OPTIONS_SET_TO(options, env, endpoint_ref);
                int soap_version = AXIOM_SOAP11;
                if (binding->getSOAPVersion() == "1.2")
                {
                    soap_version = AXIOM_SOAP12;
                }
                else
                {
                    if (wsdlOperation.getSoapVersion() == WSDLOperation::SOAP12)
                    {
                        soap_version = AXIOM_SOAP12;
                    }
                }
                
                AXIS2_OPTIONS_SET_SOAP_VERSION(options, env, soap_version);
                AXIS2_OPTIONS_SET_ACTION(options, env, soap_action);
                
                /* Create service client */
                
                axis2_char_t* client_home = AXIS2_GETENV("AXIS2C_HOME");
                if (!client_home)
                {
                    throw SystemConfigurationException("Environment error: AXIS2C_HOME not set");
                }
                axis2_svc_client_t* svc_client = axis2_svc_client_create(env, client_home);
                if (!svc_client)
                {
                    AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "invoke FAILED: Error code:"
                        " %d :: %s", env->error->error_number,
                        AXIS2_ERROR_GET_MESSAGE(env->error));
                    
                    throw SystemConfigurationException("Axis2Client: axis2_svc_client_create failed");
                }
                
                /* Set service client options */
                AXIS2_SVC_CLIENT_SET_OPTIONS(svc_client, env, options);    
                
                /* Send request */
                axiom_node_t* ret_node = AXIS2_SVC_CLIENT_SEND_RECEIVE(svc_client, env, payload);
                if(ret_node)
                {
                    setReturn(ret_node, operation, wsdlOperation, env);
                }
                else
                {
                    AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "Invoke failed: Error code:"
                        " %d :: %s", env->error->error_number,
                        AXIS2_ERROR_GET_MESSAGE(env->error));
                    throw SystemConfigurationException("Axis2Client: Invoke failed");
                }
                
                LOGINFO(2, "Warning: Have not freed Axis2C service client memory due to Jira AXIS2C-209");
                // Uncommenting this code causes crash when invoking a composite with both Axis2C ws.binding 
                // service and reference. See Jiras TUSCANY-536 and AXIS2C-209
                //if (svc_client)
                //{
                //    AXIS2_SVC_CLIENT_FREE(svc_client, env);
                //    svc_client = NULL;
                //} 
                
                LOGEXIT(1, "Axis2Client::invoke");
            }
            
            axiom_node_t* Axis2Client::createPayload(Operation& operation, 
                const WSDLOperation& wsdlOperation,
                axis2_env_t* env)
            {	
                LOGENTRY(1, "Axis2Client::createPayload");
                axiom_node_t* request_node = NULL;
                
                // map the operation request to the wsdl
                if (wsdlOperation.isDocumentStyle())
                {
                    // Document style 
                    // only support single part messages - WS-I compliant
                    
                    // Build up the payload as an SDO
                    
                    // Get the data factory for the composite (it will already have the typecreates loaded for the xsds)
                    DataFactoryPtr dataFactory = compositeReference->getComposite()->getDataFactory();
                    
                    DataObjectPtr inputDataObject;
                    try
                    {
                        
                        // Create the output wrapper
                        const Type& inputType = dataFactory->getType(wsdlOperation.getInputTypeUri().c_str(), 
                            wsdlOperation.getInputTypeName().c_str());
                        inputDataObject = dataFactory->create(inputType);
                    }
                    catch (SDORuntimeException e)
                    {
                        // The input wrapper type is not known, create an open DataObject 
                        inputDataObject = dataFactory->create(Type::SDOTypeNamespaceURI, "OpenDataObject");
                    }
                            
                    // Go through data object to set the input parameters
                    PropertyList pl = inputDataObject->getType().getProperties();
                
                    if(pl.size() == 0)
                    {
                        if(inputDataObject->getType().isOpenType() && inputDataObject->getType().isDataObjectType())
                        {
                            /*
                             * This code deals with sending xsd:any elements
                             */
                            for (int i=0; i<operation.getNParms(); i++)
                            {
                                ostringstream pname;
                                pname << "param" << (i+1);
                                DataObjectList& l = inputDataObject->getList(pname.str());
                                
                                Operation::Parameter& parm = operation.getParameter(i);
                                switch(parm.getType())
                                {
                                case Operation::BOOL: 
                                    {
                                        l.append(*(bool*)parm.getValue());
                                        break;
                                    }
                                case Operation::SHORT: 
                                    {
                                        l.append(*(short*)parm.getValue());
                                        break;
                                    }
                                case Operation::INT: 
                                    {
                                        l.append(*(long*)parm.getValue());
                                        break;
                                    }
                                case Operation::LONG: 
                                    {
                                        l.append(*(long*)parm.getValue());
                                        break;
                                    }
                                case Operation::USHORT: 
                                    {
                                        l.append(*(short*)parm.getValue());
                                        break;
                                    }
                                case Operation::UINT: 
                                    {
                                        l.append(*(long*)parm.getValue());
                                        break;
                                    }
                                case Operation::ULONG: 
                                    {
                                        l.append(*(long*)parm.getValue());
                                        break;
                                    }
                                case Operation::FLOAT: 
                                    {
                                        l.append(*(float*)parm.getValue());
                                        break;
                                    }
                                case Operation::DOUBLE: 
                                    {
                                        l.append(*(long double*)parm.getValue());
                                        break;
                                    }
                                case Operation::LONGDOUBLE: 
                                    {
                                        l.append(*(long double*)parm.getValue());
                                        break;
                                    }
                                case Operation::CHARS: 
                                    {
                                        l.append(*(char**)parm.getValue());
                                        break;
                                    }
                                case Operation::STRING: 
                                    {
                                        l.append((*(string*)parm.getValue()).c_str());
                                        break;
                                    }
                                case Operation::DATAOBJECT: 
                                    {
                                        l.append(*(DataObjectPtr*)parm.getValue());
                                        break;
                                    }
                                default: throw "unsupported parameter type";
                                }
                            }
                        }
                    }
                    else {
                        
                        // Each parameter in the operation should be a property on the request dataobject
                        for (int i=0; i<operation.getNParms(); i++)
                        {
                            Operation::Parameter& parm = operation.getParameter(i);
                            switch(parm.getType())
                            {
                            case Operation::BOOL: 
                                {
                                    inputDataObject->setBoolean(i, *(bool*)parm.getValue());
                                    break;
                                }
                            case Operation::SHORT: 
                                {
                                    inputDataObject->setShort(i, *(short*)parm.getValue());
                                    break;
                                }
                            case Operation::INT: 
                                {
                                    inputDataObject->setInteger(i, *(int*)parm.getValue());
                                    break;
                                }
                            case Operation::LONG: 
                                {
                                    inputDataObject->setLong(i, *(long*)parm.getValue());
                                    break;
                                }
                            case Operation::USHORT: 
                                {
                                    inputDataObject->setInteger(i, *(unsigned short*)parm.getValue());
                                    break;
                                }
                            case Operation::UINT: 
                                {
                                    inputDataObject->setInteger(i, *(unsigned int*)parm.getValue());
                                    break;
                                }
                            case Operation::ULONG: 
                                {
                                    inputDataObject->setInteger(i, *(unsigned long*)parm.getValue());
                                    break;
                                }
                            case Operation::FLOAT: 
                                {
                                    inputDataObject->setFloat(i, *(float*)parm.getValue());
                                    break;
                                }
                            case Operation::DOUBLE: 
                                {
                                    inputDataObject->setDouble(i, *(double*)parm.getValue());
                                    break;
                                }
                            case Operation::LONGDOUBLE: 
                                {
                                    inputDataObject->setDouble(i, *(long double*)parm.getValue());
                                    break;
                                }
                            case Operation::CHARS: 
                                {
                                    inputDataObject->setCString(i, *(char**)parm.getValue());
                                    break;
                                }
                            case Operation::STRING: 
                                {
                                    inputDataObject->setCString(i, (*(string*)parm.getValue()).c_str());
                                    break;
                                }
                            case Operation::DATAOBJECT: 
                                {
                                    inputDataObject->setDataObject(i, *(DataObjectPtr*)parm.getValue());
                                    break;
                                }
                            default: throw "unsupported parameter type";
                            }
                        }
                    }
                    
                    // Create the Axiom object from the request dataobject
                    AxiomHelper* axiomHelper = AxiomHelper::getHelper();
                    request_node = axiomHelper->toAxiomNode(inputDataObject,
                        wsdlOperation.getInputTypeUri().c_str(), wsdlOperation.getInputTypeName().c_str());
                    AxiomHelper::releaseHelper(axiomHelper);
                }
                else
                {
                    // RPC
                }
                
                char* str =  AXIOM_NODE_TO_STRING(request_node, env);
                LOGINFO_1(3, "Sending this OM node in XML : %s \n",  str); 
                
                LOGEXIT(1, "Axis2Client::createPayload");
                return request_node;
             }
             
             void Axis2Client::setReturn(axiom_node_t* ret_node,
                 Operation& operation, 
                 const WSDLOperation& wsdlOperation,
                 axis2_env_t* env)
             {	
                 LOGENTRY(1, "Axis2Client::setReturn");
                 
                 if (wsdlOperation.isDocumentStyle())
                 {
                     // Document style 
                    DataFactoryPtr dataFactory = compositeReference->getComposite()->getDataFactory();

                    // Get the AXIOM node representing the SOAP Body 
                    axiom_node_t* body = AXIOM_NODE_GET_PARENT(ret_node, env);
                     
                    // Convert the AXIOM node to an SDO DataObject
                    char* str = NULL;
                    str = AXIOM_NODE_TO_STRING(body, env);
                    if (str)
                    {
                        AXIS2_LOG_INFO((env)->log, "Axis2Client invoke has response OM: %s\n", str);
                    }

                    // Convert the SOAP body to an SDO DataObject
                    AxiomHelper* axiomHelper = AxiomHelper::getHelper();
                    DataObjectPtr outputBodyDataObject = axiomHelper->toSdo(body, dataFactory);
                    AxiomHelper::releaseHelper(axiomHelper);

                    if(!outputBodyDataObject)
                    {
                        AXIS2_LOG_ERROR((env)->log, AXIS2_LOG_SI, "Axis2Service invoke: Could not convert received Axiom node to SDO");
                        /** TODO: return a SOAP fault here */
                        return;
                    }                    

                    // Get the first body part representing the doc-lit-wrapped wrapper element
                    DataObjectPtr outputDataObject = NULL; 
                    PropertyList bpl = outputBodyDataObject->getInstanceProperties();
                    if (bpl.size()!=0)
                    {
                        if (bpl[0].isMany())
                        {
                            DataObjectList& parts = outputBodyDataObject->getList((unsigned int)0);
                            outputDataObject = parts[0];
                        }
                        else
                        {
                            outputDataObject = outputBodyDataObject->getDataObject(bpl[0]);
                        }
                    }
                    if (outputDataObject == NULL)
                    {
                        AXIS2_LOG_ERROR((env)->log, AXIS2_LOG_SI, "Axis2Client invoke: Could not convert body part to SDO");
                        return;
                    }
                     
                    PropertyList pl = outputDataObject->getInstanceProperties();
                    unsigned int i = 0;
                     
                    switch(pl[i].getTypeEnum())
                    {
                    case Type::BooleanType:
                        {
                            bool* boolData = new bool;
                            *boolData = outputDataObject->getBoolean(pl[i]);
                            operation.setReturnValue(boolData);
                        }
                        break;
                    case Type::ByteType:
                        {
                            char* byteData = new char;
                            *byteData = outputDataObject->getByte(pl[i]);
                            operation.setReturnValue(byteData);
                        }
                        break;
                    case Type::BytesType:
                        {
                            int len = outputDataObject->getLength(pl[i]);
                            char* bytesData = new char[len+1];
                            int bytesWritten = outputDataObject->getBytes(pl[i], bytesData, len);
                            // Ensure the bytes end with the null char. Not sure if this is neccessary
                            if(bytesWritten <= len)
                            {
                                bytesData[bytesWritten] = 0;
                            }
                            else
                            {
                                bytesData[len] = 0;
                            }
                            //printf("outputDataObject has BytesType named %s with length %d\n", name, bytesWritten);
                            operation.setReturnValue(&bytesData);
                        }
                        break;
                    case Type::CharacterType:
                        {
                            // This code should work but won't be used as there is no mapping from an XSD type to the SDO CharacterType
                            wchar_t* charData = new wchar_t;
                            *charData = outputDataObject->getCharacter(pl[i]);
                            operation.setReturnValue(charData);
                        }
                        break;
                    case Type::DoubleType:
                        {
                            long double* doubleData = new long double;
                            *doubleData = outputDataObject->getDouble(pl[i]);
                            operation.setReturnValue(doubleData);
                        }
                        break;
                    case Type::FloatType:
                        {
                            float* floatData = new float;
                            *floatData = outputDataObject->getFloat(pl[i]);
                            operation.setReturnValue(floatData); 
                        }
                        break;
                    case Type::IntegerType:
                        {
                            long* intData = new long;
                            *intData = outputDataObject->getInteger(pl[i]);
                            operation.setReturnValue(intData);
                        }
                        break;
                    case Type::ShortType:
                        {
                            short* shortData = new short;
                            *shortData = outputDataObject->getShort(pl[i]);
                            operation.setReturnValue(shortData);
                        }
                        break;
                    case Type::StringType:
                        {
                            string* str = new string(outputDataObject->getCString(pl[i]));
                            operation.setReturnValue(str);
                        }
                        break;
                    case Type::DataObjectType:
                        {
                            DataObjectPtr* dataObjectData = new DataObjectPtr;
                            *dataObjectData = outputDataObject->getDataObject(pl[i]);
                            if(!dataObjectData)
                            {
                                LOGINFO(4, "SDO DataObject return value was null");
                            }
                            operation.setReturnValue(dataObjectData);
                        }
                        break;
                    case Type::OpenDataObjectType:
                        {         
                            /*
                             * This code deals with xsd:any element parameters
                             */
            
                            DataObjectList& dataObjectList = outputDataObject->getList(pl[i]);
                            
                            for(int j=0; j<dataObjectList.size(); j++)
                            {
                                DataObjectPtr dataObjectData = dataObjectList[j];
                                if(!dataObjectData)
                                {
                                    DataObjectPtr* dob = new DataObjectPtr;
                                    *dob = NULL;
                                    operation.setReturnValue(dob);
                                    LOGINFO(4, "SDO OpenDataObject return value was null");
                                }
                                else 
                                {
                                    
                                    SequencePtr sequence = dataObjectData->getSequence();
                                    if (sequence->size()!=0)
                                    {
                                        // Return a text element        
                                        if (sequence->isText(0))
                                        {                                        
                                            string* stringData = new string(sequence->getCStringValue(0));
                                            operation.setReturnValue(stringData);
                                        }
                                        else
                                        {
                                            // Return a DataObject representing a complex element
                                            DataObjectPtr* dob = new DataObjectPtr;
                                            *dob = sequence->getDataObjectValue(0);
                                            if(!*dob)
                                            {
                                                LOGINFO(4, "SDO DataObject return value was null");
                                            }
                                            operation.setReturnValue(dob);
                                        }
                                    }
                                    else
                                    {
                                        // Empty content, add an empty string
                                        LOGINFO(4, "SDO OpenDataObject return value was empy");
                                        string *stringData = new string(""); 
                                        operation.setReturnValue(stringData);
                                    }
                                }
                            }
                        }
                        break;
                    case Type::DateType:
                        LOGERROR(0, "SDO DateType return values are not yet supported");
                        break;
                    case Type::LongType:
                        LOGERROR(0, "SDO LongType (int64_t) return values are not yet supported");
                        break;
                    case Type::UriType:
                        LOGERROR(0, "SDO UriType return values are not yet supported");
                        break;
                    case Type::BigDecimalType:
                        LOGERROR(0, "SDO BigDecimalType return values are not yet supported");
                        break;
                    case Type::BigIntegerType:
                        LOGERROR(0, "SDO BigIntegerType return values are not yet supported");
                        break;
                    default:
                        LOGERROR(0, "Unknown SDO type has been found in return value. Unknown types are not yet supported");
                        break;
                    }         
                 }
                 else
                 {
                     // RPC
                 }
                 LOGEXIT(1, "Axis2Client::setReturn");
             }
             
        } // End namespace ws
    } // End namespace sca
} // End namespace tuscany