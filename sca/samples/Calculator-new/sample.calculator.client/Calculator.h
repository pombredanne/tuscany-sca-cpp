/*
 *
 *  Copyright 2006 The Apache Software Foundation or its licensors, as applicable.
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

#ifndef sample_calculator_h
#define sample_calculator_h

class Calculator  
{
public:
	virtual float add(float arg1, float arg2) = 0;
	virtual float sub(float arg1, float arg2) = 0;
	virtual float mul(float arg1, float arg2) = 0;
	virtual float div(float arg1, float arg2) = 0;
};

#endif // sample_calculator_h


