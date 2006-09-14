/*
 * Copyright 2004,2005 The Apache Software Foundation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


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
                            binding->getPortName(),
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
                if (wsdlOperation.getSoapVersion() == WSDLOperation::SOAP12)
                {
                    soap_version = AXIOM_SOAP12;
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
                
                if (svc_client)
                {
                    AXIS2_SVC_CLIENT_FREE(svc_client, env);
                    svc_client = NULL;
                } 
                
                LOGEXIT(1, "Axis2Client::invoke");
            }
            
            axiom_node_t* Axis2Client::createPayload(Operation& operation, 
                const WSDLOperation& wsdlOp,
                axis2_env_t* env)
            {	
                LOGENTRY(1, "Axis2Client::createPayload");
                axiom_node_t* requestOM = NULL;
                
                // map the operation request to the wsdl
                if (wsdlOp.isDocumentStyle())
                {
                    // Document style 
                    // only support single part messages - WS-I compliant
                    
                    // Build up the payload as an SDO
                    
                    // Get the data factory for the composite (it will already have the typecreates loaded for the xsds)
                    DataFactoryPtr dataFactory = compositeReference->getComposite()->getDataFactory();
                    DataObjectPtr requestDO = dataFactory->create(wsdlOp.getInputTypeUri().c_str(), 
                        wsdlOp.getInputTypeName().c_str());
                    
                    // Each parameter in the operation should be a property on the request dataobject
                    for (int i=0; i<operation.getNParms(); i++)
                    {
                        Operation::Parameter& parm = operation.getParameter(i);
                        switch(parm.getType())
                        {
                        case Operation::BOOL: 
                            {
                                requestDO->setBoolean(i, *(bool*)parm.getValue());
                                break;
                            }
                        case Operation::SHORT: 
                            {
                                requestDO->setShort(i, *(short*)parm.getValue());
                                break;
                            }
                        case Operation::LONG: 
                            {
                                requestDO->setLong(i, *(long*)parm.getValue());
                                break;
                            }
                        case Operation::USHORT: 
                            {
                                requestDO->setInteger(i, *(unsigned short*)parm.getValue());
                                break;
                            }
                        case Operation::ULONG: 
                            {
                                requestDO->setInteger(i, *(unsigned long*)parm.getValue());
                                break;
                            }
                        case Operation::FLOAT: 
                            {
                                requestDO->setFloat(i, *(float*)parm.getValue());
                                break;
                            }
                        case Operation::DOUBLE: 
                            {
                                requestDO->setDouble(i, *(double*)parm.getValue());
                                break;
                            }
                        case Operation::LONGDOUBLE: 
                            {
                                requestDO->setDouble(i, *(long double*)parm.getValue());
                                break;
                            }
                        case Operation::CHARS: 
                            {
                                requestDO->setCString(i, *(char**)parm.getValue());
                                break;
                            }
                        case Operation::STRING: 
                            {
                                requestDO->setCString(i, (*(string*)parm.getValue()).c_str());
                                break;
                            }
                        case Operation::DATAOBJECT: 
                            {
                                requestDO->setDataObject(i, *(DataObjectPtr*)parm.getValue());
                                break;
                            }
                        default: throw "unsupported parameter type";
                        }
                    }
                    
                    // Create the Axiom object from the request dataobject
                    AxiomHelper myHelper;
                    requestOM = myHelper.toAxiomNode(requestDO);
                }
                else
                {
                    // RPC
                }
                
                // Logging start
                axiom_xml_writer_t* xml_writer = axiom_xml_writer_create_for_memory(env, NULL, AXIS2_FALSE, AXIS2_FALSE, AXIS2_XML_PARSER_TYPE_BUFFER);
                axiom_output_t* om_output = axiom_output_create( env, xml_writer);
                
                AXIOM_NODE_SERIALIZE(requestOM, env, om_output);
                axis2_char_t* buffer = (axis2_char_t*)AXIOM_XML_WRITER_GET_XML(xml_writer, env);         
                LOGINFO_1(3, "Sending this OM node in XML : %s \n",  buffer); 
                
                AXIOM_OUTPUT_FREE(om_output, env);
                AXIS2_FREE((env)->allocator, buffer);
                // Logging end
                
                LOGEXIT(1, "Axis2Client::createPayload");
                
                return requestOM;
             }
             
             void Axis2Client::setReturn(axiom_node_t* ret_node,
                 Operation& operation, 
                 const WSDLOperation& wsdlOp,
                 axis2_env_t* env)
             {	
                 LOGENTRY(1, "Axis2Client::setReturn");
                 /* Get the response value from the SOAP message */
                 
                 // logging start
                 axiom_xml_writer_t* writer = axiom_xml_writer_create_for_memory(env, NULL, AXIS2_TRUE, 0, AXIS2_XML_PARSER_TYPE_BUFFER);
                 axiom_output_t* om_output = axiom_output_create (env, writer);
                 AXIOM_NODE_SERIALIZE (ret_node, env, om_output);
                 axis2_char_t* buffer = (axis2_char_t*)AXIOM_XML_WRITER_GET_XML(writer, env);
                 LOGINFO_1(3,"Received OM node in XML : %s\n", buffer);
                 AXIOM_OUTPUT_FREE(om_output, env);
                 AXIS2_FREE((env)->allocator, buffer);
                 // Logging end
                 
                 
                 if (wsdlOp.isDocumentStyle())
                 {
                     // Document style 
                     
                     DataFactoryPtr dataFactory = compositeReference->getComposite()->getDataFactory();
                     AxiomHelper myHelper;
                     DataObjectPtr returnDO = myHelper.toSdo(ret_node, dataFactory);
                     
                    PropertyList pl = returnDO->getInstanceProperties();
                    unsigned int i = 0;
                     
                    switch(pl[i].getTypeEnum())
                    {
                    case Type::BooleanType:
                        {
                            bool* boolData = new bool;
                            *boolData = returnDO->getBoolean(pl[i]);
                            //printf("returnDO has BooleanType named %s with value %d\n", name, boolData);
                            operation.setReturnValue(boolData);
                        }
                        break;
                    case Type::ByteType:
                        {
                            char* byteData = new char;
                            //printf("returnDO has ByteType named %s\n", name);
                            *byteData = returnDO->getByte(pl[i]);
                            operation.setReturnValue(byteData);
                        }
                        break;
                    case Type::BytesType:
                        {
                            int len = returnDO->getLength(pl[i]);
                            char* bytesData = new char[len+1];
                            int bytesWritten = returnDO->getBytes(pl[i], bytesData, len);
                            // Ensure the bytes end with the null char. Not sure if this is neccessary
                            if(bytesWritten <= len)
                            {
                                bytesData[bytesWritten] = 0;
                            }
                            else
                            {
                                bytesData[len] = 0;
                            }
                            //printf("returnDO has BytesType named %s with length %d\n", name, bytesWritten);
                            operation.setReturnValue(&bytesData);
                        }
                        break;
                    case Type::CharacterType:
                        {
                            // This code should work but won't be used as there is no mapping from an XSD type to the SDO CharacterType
                            wchar_t* charData = new wchar_t;
                            //printf("returnDO has CharacterType named %s\n", name);
                            *charData = returnDO->getCharacter(pl[i]);
                            operation.setReturnValue(charData);
                        }
                        break;
                    case Type::DoubleType:
                        {
                            long double* doubleData = new long double;
                            *doubleData = returnDO->getDouble(pl[i]);
                            //printf("returnDO has DoubleType named %s\n", name);            
                            operation.setReturnValue(doubleData);
                        }
                        break;
                    case Type::FloatType:
                        {
                            float* floatData = new float;
                            *floatData = returnDO->getFloat(pl[i]);
                            //printf("returnDO has FloatType named %s with value %f\n", name, *floatData);
                            operation.setReturnValue(floatData); 
                        }
                        break;
                    case Type::IntegerType:
                        {
                            long* intData = new long;
                            //printf("returnDO has IntegerType named %s\n", name);
                            *intData = returnDO->getInteger(pl[i]);
                            operation.setReturnValue(intData);
                        }
                        break;
                    case Type::ShortType:
                        {
                            short* shortData = new short;
                            //printf("returnDO has ShortType named %s\n", name);
                            *shortData = returnDO->getShort(pl[i]);
                            operation.setReturnValue(shortData);
                        }
                        break;
                    case Type::StringType:
                        {
                            const char** stringData = new const char*; 
                            string* str = new string(returnDO->getCString(pl[i]));
                            *stringData = str->c_str();
                            //printf("returnDO has StringType named %s with value %s\n", name, stringData);
                            operation.setReturnValue(stringData);
                        }
                        break;
                    case Type::DataObjectType:
                        {
                            DataObjectPtr dataObjectData = returnDO->getDataObject(pl[i]);
                            //printf("returnDO has DataObjectType named %s (#%d)\n", name, dataObjectData);
            
                            if(!dataObjectData)
                            {
                                LOGINFO(4, "SDO DataObject return value was null");
                            }
                            operation.setReturnValue(&dataObjectData);
                        }
                        break;
                    case Type::OpenDataObjectType:
                        {         
                            /*
                             * This code deals with xsd:any element parameters
                             * Get each element as a DataObject and add in to the parameter list
                             */
            
                            //printf("returnDO has OpenDataObjectType named %s\n", name);
                            DataObjectList& dataObjectList = returnDO->getList(pl[i]);
                            
                            for(int j=0; j<dataObjectList.size(); j++)
                            {
                                DataObjectPtr dataObjectData = dataObjectList[j];
                                if(!dataObjectData)
                                {
                                    LOGINFO(4, "SDO OpenDataObject return value was null");
                                }
                                operation.setReturnValue(&dataObjectData);
                                //Utils::printDO(dataObjectData);
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