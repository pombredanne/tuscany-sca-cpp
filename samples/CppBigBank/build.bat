@echo off
@REM  Licensed to the Apache Software Foundation (ASF) under one
@REM  or more contributor license agreements.  See the NOTICE file
@REM  distributed with this work for additional information
@REM  regarding copyright ownership.  The ASF licenses this file
@REM  to you under the Apache License, Version 2.0 (the
@REM  "License"); you may not use this file except in compliance
@REM  with the License.  You may obtain a copy of the License at
@REM  
@REM    http://www.apache.org/licenses/LICENSE-2.0
@REM    
@REM  Unless required by applicable law or agreed to in writing,
@REM  software distributed under the License is distributed on an
@REM  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
@REM  KIND, either express or implied.  See the License for the
@REM  specific language governing permissions and limitations
@REM  under the License.

setlocal

if "%TUSCANY_SDOCPP%" == "" (
echo "TUSCANY_SDOCPP not set"
goto end
)
echo using Tuscany SCACPP: %TUSCANY_SCACPP%

if "%TUSCANY_SCACPP%" == "" (
echo "TUSCANY_SCACPP not set"
goto end
)
echo using Tuscany SCACPP: %TUSCANY_SCACPP%

if "%AXIS2C_HOME%" == "" (
echo "AXIS2C_HOME not set"
goto end
)
echo using Axis2C: %AXIS2C_HOME%"

call vcvars32.bat
cd VSExpress
call vcbuild CppBigBank.sln "Release|Win32"

:end
endlocal