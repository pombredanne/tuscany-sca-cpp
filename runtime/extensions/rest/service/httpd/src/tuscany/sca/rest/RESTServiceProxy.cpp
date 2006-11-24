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

#include "RESTServiceProxy.h"
#include "tuscany/sca/util/Logging.h"
#include "tuscany/sca/util/Exceptions.h"
#include "tuscany/sca/util/Utils.h"
#include "tuscany/sca/core/SCARuntime.h"
#include "tuscany/sca/model/Reference.h"
#include "tuscany/sca/model/ReferenceType.h"
#include "tuscany/sca/model/Service.h"
#include "tuscany/sca/model/ServiceType.h"
#include "tuscany/sca/model/Component.h"
#include "tuscany/sca/model/ComponentType.h"
#include "tuscany/sca/core/ServiceWrapper.h"
#include "tuscany/sca/model/Composite.h"
#include "tuscany/sca/model/ServiceBinding.h"
#include "model/RESTReferenceBinding.h"

#include "commonj/sdo/SDO.h"
using namespace commonj::sdo;

using namespace tuscany::sca::model;

namespace tuscany
{
    namespace sca
    {
        namespace rest
        {
            
            // ============================
            // Constructor: Create a proxy
            // ============================
            RESTServiceProxy::RESTServiceProxy(Reference* reference)
                : ServiceProxy(reference)
            {
                logentry();
    
                // Get the target service wrapper
                RESTReferenceBinding* referenceBinding = (RESTReferenceBinding*)reference->getBinding();
                serviceWrapper = referenceBinding->getTargetServiceBinding()->getServiceWrapper();
                
                // Define the SOAP Body type and element to allow a SOAP body to
                // be loaded in a DataObject
                DataFactoryPtr dataFactory = reference->getComponent()->getComposite()->getDataFactory();
                try {
                    const Type& bodyType = dataFactory->getType("http://www.w3.org/2003/05/soap-envelope", "Body");
                } catch (SDORuntimeException&)
                {
                    
                    // Define the SOAP 1.2 Body type
                    dataFactory->addType("http://www.w3.org/2003/05/soap-envelope", "RootType", false, false, false);                
                    dataFactory->addType("http://www.w3.org/2003/05/soap-envelope", "Body", false, true, false);                
                    dataFactory->addPropertyToType(
                        "http://www.w3.org/2003/05/soap-envelope", "RootType",
                        "Body",
                        "http://www.w3.org/2003/05/soap-envelope", "Body",
                        false, false, true);
    
                    // Define the SOAP 1.1 Body type
                    dataFactory->addType("http://schemas.xmlsoap.org/soap/envelope/", "RootType", false, false, false);
                    dataFactory->addType("http://schemas.xmlsoap.org/soap/envelope/", "Body", false, true, false);
                    dataFactory->addPropertyToType(
                        "http://schemas.xmlsoap.org/soap/envelope/", "RootType",
                        "Body",
                        "http://schemas.xmlsoap.org/soap/envelope/", "Body",
                        false, false, true);
                }
                
            }
            
            // ==========
            // Destructor
            // ==========
            RESTServiceProxy::~RESTServiceProxy()
            {
                logentry();
            }
            
            ///
            /// This method will be called to process an operation invocation.
            ///
            DataObjectPtr RESTServiceProxy::invoke(const string& operationName, DataObjectPtr inputDataObject)
            {
                logentry();
    
                Reference* reference = getReference();
                Component* component = reference->getComponent();
                Composite* composite = component ->getComposite();
            
                RESTReferenceBinding* referenceBinding = (RESTReferenceBinding*)reference->getBinding();
                DataFactoryPtr dataFactoryPtr = reference->getComponent()->getComposite()->getDataFactory();
                                    
//                const char* outputTypeURI = "";
//                const char* outputTypeName = "";
//
//                LOGINFO_2(4, "RESTServiceProxy has got WSDLOperation with inputType: %s#%s",
//                    wsdlOperation.getInputTypeUri().c_str(), 
//                    wsdlOperation.getInputTypeName().c_str());
//                LOGINFO_2(4, "RESTServiceProxy has got WSDLOperation with outputType: %s#%s",
//                    outputTypeURI, 
//                    outputTypeName);
//                LOGINFO_2(4, "RESTServiceProxy has got WSDLOperation with documentStyle=%d and wrapped=%d",        
//                    documentStyle,
//                    wrappedStyle);
//                
//                if (documentStyle)
//                {
//                    if (wrappedStyle)
//                    {
//
//                        // Create new Operation object and set parameters and return value
//                        Operation operation(wsdlOperation.getOperationName().c_str());
//                    
//                        // Go through the input data object to set the operation parameters
//                        PropertyList pl = inputDataObject->getInstanceProperties();
//                    
//                        for(int i=0; i<pl.size(); i++)
//                        {
//                            const char* name = pl[i].getName();
//                    
//                            switch (pl[i].getTypeEnum()) 
//                            {
//                            case Type::BooleanType:
//                                {
//                                    bool* boolData = new bool;
//                                    *boolData = inputDataObject->getBoolean(pl[i]);
//                                    operation.addParameter(boolData);
//                                }
//                                break;
//                            case Type::ByteType:
//                                {
//                                    char* byteData = new char;
//                                    *byteData = inputDataObject->getByte(pl[i]);
//                                    operation.addParameter(byteData);
//                                }
//                                break;
//                            case Type::BytesType:
//                                {
//                                    int len = inputDataObject->getLength(pl[i]);
//                                    char* bytesData = new char[len+1];
//                                    int bytesWritten = inputDataObject->getBytes(pl[i], bytesData, len);
//                                    // Ensure the bytes end with the null char. Not sure if this is neccessary
//                                    if(bytesWritten <= len)
//                                    {
//                                        bytesData[bytesWritten] = 0;
//                                    }
//                                    else
//                                    {
//                                        bytesData[len] = 0;
//                                    }
//                                    operation.addParameter(&bytesData);
//                                }
//                                break;
//                            case Type::CharacterType:
//                                {
//                                    // This code should work but won't be used as there is no mapping from an XSD type to the SDO CharacterType
//                                    wchar_t* charData = new wchar_t;
//                                    *charData = inputDataObject->getCharacter(pl[i]);
//                                    operation.addParameter(charData);
//                                }
//                                break;
//                            case Type::DoubleType:
//                                {
//                                    long double* doubleData = new long double;
//                                    *doubleData = inputDataObject->getDouble(pl[i]);
//                                    operation.addParameter(doubleData);
//                                }
//                                break;
//                            case Type::FloatType:
//                                {
//                                    float* floatData = new float;
//                                    *floatData = inputDataObject->getFloat(pl[i]);
//                                    operation.addParameter(floatData); 
//                                }
//                                break;
//                            case Type::IntegerType:
//                                {
//                                    long* intData = new long;
//                                    *intData = inputDataObject->getInteger(pl[i]);
//                                    operation.addParameter(intData);
//                                }
//                                break;
//                            case Type::ShortType:
//                                {
//                                    short* shortData = new short;
//                                    *shortData = inputDataObject->getShort(pl[i]);
//                                    operation.addParameter(shortData);
//                                }
//                                break;
//                            case Type::StringType:
//                                {
//                                    string* stringData;
//                                    if(inputDataObject->isSet(pl[i]))
//                                    {
//                                        stringData = new string(inputDataObject->getCString(pl[i]));                                        
//                                    }
//                                    else
//                                    {
//                                        // The data is not set, so pass an empty string as the parameter
//                                        stringData = new string();
//                                    }
//                                    operation.addParameter(stringData);
//                                }
//                                break;
//                            case Type::DataObjectType:
//                                {
//                                    DataObjectPtr dataObjectData = inputDataObject->getDataObject(pl[i]);
//                    
//                                    if(!dataObjectData)
//                                    {
//                                        LOGINFO_1(4, "SDO DataObject parameter named %s was null", name);
//                                    }
//                                    operation.addParameter(&dataObjectData);
//                                }
//                                break;
//                            case Type::OpenDataObjectType:
//                                {         
//                                    /*
//                                     * This code deals with xsd:any element parameters
//                                     * Get each element as a DataObject and add in to the parameter list
//                                     */
//                    
//                                    DataObjectList& dataObjectList = inputDataObject->getList(pl[i]);
//                                    
//                                    for(int j=0; j<dataObjectList.size(); j++)
//                                    {
//                                        DataObjectPtr dataObjectData = dataObjectList[j];
//                                        if(!dataObjectData)
//                                        {
//                                            
//                                            // Add a null DataObject ptr
//                                            LOGINFO_2(4, "SDO OpenDataObject parameter named %s[%d] was null", name, j);
//                                            operation.addParameter(&dataObjectData);
//                                        }
//                                        else
//                                        {
//                                            
//                                            SequencePtr sequence = dataObjectData->getSequence();
//                                            if (sequence->size()!=0)
//                                            {
//                                                // Add a text element        
//                                                if (sequence->isText(0))
//                                                {                                        
//                                                    string* stringData = new string(sequence->getCStringValue(0));
//                                                    operation.addParameter(stringData);
//                                                }
//                                                else
//                                                {
//                                                    // Add a complex element DataObject
//                                                    DataObjectPtr dob = sequence->getDataObjectValue(0);
//                                                    if(!dob)
//                                                    {
//                                                        LOGINFO_1(4, "SDO DataObject parameter named %s was null", name);
//                                                    }
//                                                    operation.addParameter(&dob);
//                                                }
//                                            }
//                                            else
//                                            {
//                                                // Empty content, add an empty string
//                                                LOGINFO_2(4, "SDO OpenDataObject parameter named %s[%d] was empty", name, j);
//                                                string* stringData = new string(""); 
//                                                operation.addParameter(stringData);
//                                            }
//                                        }                       
//                                    }
//                                }
//                                break;
//                            case Type::DateType:
//                                LOGERROR_1(0, "SDO DateType parameters are not yet supported (%s)", name);
//                                return NULL;
//                            case Type::LongType:
//                                LOGERROR_1(0, "SDO LongType (int64_t) parameters are not yet supported (%s)", name);
//                                return NULL;
//                                break;
//                            case Type::UriType:
//                                LOGERROR_1(0, "SDO UriType parameters are not yet supported (%s)", name);
//                                return NULL;
//                                break;
//                            case Type::BigDecimalType:
//                                LOGERROR_1(0, "SDO BigDecimalType parameters are not yet supported (%s)", name);
//                                return NULL;
//                                break;
//                            case Type::BigIntegerType:
//                                LOGERROR_1(0, "SDO BigIntegerType parameters are not yet supported (%s)", name);
//                                return NULL;
//                                break;
//                            default:
//                                LOGERROR_1(0, "Unknown SDO type parameter named %s has been found. Unknown types are not yet supported", name);
//                                return NULL;
//                            }         
//                        }
//                                
//                        try
//                        {
//                            
//                            // Call into the target service wrapper
//                            serviceWrapper->invoke(operation);
//                    
//                            // Set the data in the outputDataObject to be returned
//                            DataObjectPtr outputDataObject;
//                            try
//                            {
//                                
//                                // Create the output wrapper
//                                const Type& outputType = dataFactoryPtr->getType(outputTypeURI, outputTypeName);
//                                outputDataObject = dataFactoryPtr->create(outputType);
//                            }
//                            catch (SDORuntimeException&)
//                            {
//                                // The output wrapper type is not known, create an open DataObject 
//                                outputDataObject = dataFactoryPtr->create(Type::SDOTypeNamespaceURI, "OpenDataObject");
//                            }
//                            
//                            setOutputData(operation, outputDataObject, dataFactoryPtr);                            
//
//                            return outputDataObject;
//
//                        }
//                      catch(SDORuntimeException& ex)
//                      {   
//                            // TODO: Return more error information than just a null DataObject
//                            LOGERROR_2(0, "%s has been caught: %s\n", ex.getEClassName(), ex.getMessageText());
//                            return NULL;
//                      }  
//                      catch(TuscanyRuntimeException& ex)
//                      {   
//                            // TODO: Return more error information than just a null DataObject
//                            LOGERROR_2(0, "%s has been caught: %s\n", ex.getEClassName(), ex.getMessageText());
//                            return NULL;
//                      }  
//                    }
//                    else
//                    {
//                        LOGERROR(0, "CompositeServices with non-wrapped document style WSDL Operations are not yet supported");
//                        return NULL;
//                    }
//                }
//                else
//                {
//                    // RPC style
//                    LOGERROR(0, "CompositeServices with RPC style WSDL Operations are not yet supported");
//                    return NULL;
//                }

                return NULL;
            }
            
            
            void RESTServiceProxy::setOutputData(Operation operation, DataObjectPtr outputDataObject, DataFactoryPtr dataFactoryPtr)
            {    
                logentry();

                // Go through data object to set the return value
                PropertyList pl = outputDataObject->getType().getProperties();
            
                if(pl.size() == 0)
                {
                    if(outputDataObject->getType().isOpenType() && outputDataObject->getType().isDataObjectType())
                    {
                        /*
                         * This code deals with returning xsd:any elements
                         */
                        DataObjectList& l = outputDataObject->getList("return");
                        Operation::ParameterType resultType = operation.getReturnType();
                        switch(resultType)
                        {
                        case Operation::BOOL: 
                            {
                                l.append(*(bool*)operation.getReturnValue());
                                break;
                            }
                        case Operation::SHORT: 
                            {
                                l.append(*(short*)operation.getReturnValue());
                                break;
                            }
                        case Operation::INT: 
                            {
                                l.append(*(long*)operation.getReturnValue());
                                break;
                            }
                        case Operation::LONG: 
                            {
                                l.append(*(long*)operation.getReturnValue());
                                break;
                            }
                        case Operation::USHORT: 
                            {
                                l.append(*(short*)operation.getReturnValue());
                                break;
                            }
                        case Operation::UINT: 
                            {
                                l.append(*(long*)operation.getReturnValue());
                                break;
                            }
                        case Operation::ULONG: 
                            {
                                l.append(*(long*)operation.getReturnValue());
                                break;
                            }
                        case Operation::FLOAT: 
                            {
                                l.append(*(float*)operation.getReturnValue());
                                break;
                            }
                        case Operation::DOUBLE: 
                            {
                                l.append(*(long double*)operation.getReturnValue());
                                break;
                            }
                        case Operation::LONGDOUBLE: 
                            {
                                l.append(*(long double*)operation.getReturnValue());
                                break;
                            }
                        case Operation::CHARS: 
                            {
                                l.append(*(char**)operation.getReturnValue());
                                break;
                            }
                        case Operation::STRING: 
                            {
                                l.append((*(string*)operation.getReturnValue()).c_str());
                                break;
                            }
                        case Operation::DATAOBJECT: 
                            {
                                l.append(*(DataObjectPtr*)operation.getReturnValue());
                                break;
                            }
                        default:
                            {
                                string msg = "Unsupported parameter type";
                                msg += resultType;
                                throwException(ServiceDataException, msg.c_str()); 
                            }
                        }
                    }
                    else
                    {
                        loginfo("No return values defined");
                    }
                }
                else {
            
                    // Should only be one return value.. This goes through all return values
                    for(int i=0; i<pl.size(); i++)
                    {
                        const char* name = pl[i].getName();
    
                        Operation::ParameterType resultType = operation.getReturnType();
                        switch(resultType)
                        {
                        case Operation::BOOL: 
                            {
                                outputDataObject->setBoolean(pl[i], *(bool*)operation.getReturnValue());
                                break;
                            }
                        case Operation::SHORT: 
                            {
                                outputDataObject->setShort(pl[i], *(short*)operation.getReturnValue());
                                break;
                            }
                        case Operation::INT: 
                            {
                                outputDataObject->setInteger(pl[i], *(int*)operation.getReturnValue());
                                break;
                            }
                        case Operation::LONG: 
                            {
                                outputDataObject->setInteger(pl[i], *(long*)operation.getReturnValue());
                                break;
                            }
                        case Operation::USHORT: 
                            {
                                outputDataObject->setInteger(pl[i], *(unsigned short*)operation.getReturnValue());
                                break;
                            }
                        case Operation::UINT: 
                            {
                                outputDataObject->setInteger(pl[i], *(unsigned int*)operation.getReturnValue());
                                break;
                            }
                        case Operation::ULONG: 
                            {
                                outputDataObject->setInteger(pl[i], *(unsigned long*)operation.getReturnValue());
                                break;
                            }
                        case Operation::FLOAT: 
                            {
                                outputDataObject->setFloat(pl[i], *(float*)operation.getReturnValue());
                                break;
                            }
                        case Operation::DOUBLE: 
                            {
                                outputDataObject->setDouble(pl[i], *(double*)operation.getReturnValue());
                                break;
                            }
                        case Operation::LONGDOUBLE: 
                            {
                                outputDataObject->setDouble(pl[i], *(long double*)operation.getReturnValue());
                                break;
                            }
                        case Operation::CHARS: 
                            {
                                if(*(char**)operation.getReturnValue() != NULL)
                                {
                                    outputDataObject->setCString(pl[i], *(char**)operation.getReturnValue());
                                }
                                else
                                {
                                    loginfo("Null return value, leaving property %s unset", pl[i].getName());
                                }
                                break;
                            }
                        case Operation::STRING: 
                            {
                                outputDataObject->setCString(pl[i], (*(string*)operation.getReturnValue()).c_str());                                
                                break;
                            }
                        case Operation::DATAOBJECT: 
                            {

                                if(*(DataObjectPtr*)operation.getReturnValue() != NULL)
                                {
                                    outputDataObject->setDataObject(pl[i], *(DataObjectPtr*)operation.getReturnValue());
                                }
                                else
                                {
                                    loginfo("Null return value, so leaving property %s unset", pl[i].getName());
                                }
                                
                                break;
                            }
                        default:
                            {
                                string msg = "Unsupported parameter type";
                                msg += resultType;
                                throwException(ServiceDataException, msg.c_str()); 
                            }
                        }
                    }
                }
            }
    
        } // End namespace rest        
    } // End namespace sca
} // End namespace tuscany