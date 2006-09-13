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

#ifndef tuscany_sca_python_export_h
#define tuscany_sca_python_export_h

#if defined(WIN32)  || defined (_WINDOWS)
#pragma warning(disable: 4786)

#ifdef TUSCANY_SCA_PYTHON_EXPORTS
#define SCA_PYTHON_API __declspec(dllexport)
#else
#define SCA_PYTHON_API __declspec(dllimport)
#endif

#else
#include <sys/time.h>
#include <inttypes.h> 
#include <stdlib.h>
#define SCA_PYTHON_API 
#endif

#endif // tuscany_sca_export_h
