#!/bin/sh

#  Licensed to the Apache Software Foundation (ASF) under one
#  or more contributor license agreements.  See the NOTICE file
#  distributed with this work for additional information
#  regarding copyright ownership.  The ASF licenses this file
#  to you under the Apache License, Version 2.0 (the
#  "License"); you may not use this file except in compliance
#  with the License.  You may obtain a copy of the License at
#  
#    http://www.apache.org/licenses/LICENSE-2.0
#    
#  Unless required by applicable law or agreed to in writing,
#  software distributed under the License is distributed on an
#  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
#  KIND, either express or implied.  See the License for the
#  specific language governing permissions and limitations
#  under the License.

APFULLDIR=`pwd`

if [ x$TUSCANY_SCACPP = x ]; then
echo "TUSCANY_SCACPP not set"
exit;
fi
echo "Using SCA installed at $TUSCANY_SCACPP"

if [ x$TUSCANY_SDOCPP = x ]; then
echo "TUSCANY_SDOCPP not set"
exit;
fi
echo "Using SDO installed at $TUSCANY_SDOCPP"

if [ x$AXIS2C_HOME = x ]; then
echo "AXIS2C_HOME not set"
exit;
fi
echo "Using Axis2C installed at $AXIS2C_HOME"

export LD_LIBRARY_PATH=$TUSCANY_SCACPP/extensions/cpp/lib:$TUSCANY_SCACPP/lib:$TUSCANY_SDOCPP/lib:$AXIS2C_HOME/lib:$LD_LIBRARY_PATH

export TUSCANY_SCACPP_ROOT=$APFULLDIR/../
export TUSCANY_SCACPP_BASE_URI=http://localhost:9090

# Generate the mod_axis2 configuration
if [ ! -f conf/mod_axis2.conf ]; then
  echo "LoadModule axis2_module $AXIS2C_HOME/lib/libmod_axis2.so.0.0.0" >conf/mod_axis2.conf
  echo "<Location /axis2>" >>conf/mod_axis2.conf
  echo "        SetHandler axis2_module" >>conf/mod_axis2.conf
  echo "        RepoPath $AXIS2C_HOME" >>conf/mod_axis2.conf
  echo "        LogFile $AXIS2C_HOME/logs/httpd.log" >>conf/mod_axis2.conf
  echo "        Axis2LogLevel AXIS2_LOG_LEVEL_INFO" >>conf/mod_axis2.conf
  echo "</Location>" >>conf/mod_axis2.conf
fi

# Generate the mod_rest configuration
if [ ! -f conf/tuscany_sca_mod_rest.conf ]; then
  echo "LoadModule sca_rest_module $TUSCANY_SCACPP/extensions/rest/service/lib/libtuscany_sca_mod_rest.so.0.0.0" >conf/tuscany_sca_mod_rest.conf
  echo "TuscanyHome $TUSCANY_SCACPP" >>conf/tuscany_sca_mod_rest.conf
  echo "<Location /rest>" >>conf/tuscany_sca_mod_rest.conf
  echo "        SetHandler sca_rest_module" >>conf/tuscany_sca_mod_rest.conf
  echo "        TuscanyRoot $TUSCANY_SCACPP_ROOT" >>conf/tuscany_sca_mod_rest.conf
  echo "</Location>" >>conf/tuscany_sca_mod_rest.conf
fi

# Generate the HTTPD base configuration
if [ ! -f conf/base.conf ]; then
  echo "DocumentRoot $APFULLDIR/htdocs" >conf/base.conf
fi

# Create logs directory
if [ ! -d logs ]; then
  mkdir logs
fi

# Start the HTTP server
echo "Starting Apache httpd"
apachectl -k start -d $APFULLDIR
