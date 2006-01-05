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

/* $Rev$ $Date: 2005/12/22 11:33:21 $ */

#ifndef MyValueImpl_h
#define MyValueImpl_h

#include "MyValue.h"



class MyValueImpl : public MyValue
{
public:
    MyValueImpl();
    virtual ~MyValueImpl();
    virtual float getMyValue(const char* customerID);
    virtual float getMyValueS(const string& customerID);
    virtual string getCustname(string& customerID);
    virtual const string& getCustnamecs(string customerID);

};

#endif
