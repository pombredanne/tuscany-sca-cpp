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

JAVAROOT = ${top_builddir}/samples/store-java

if WANT_JAVA

dist_sample_SCRIPTS = start stop ssl-start
sampledir=$(prefix)/samples/store-java

AM_JAVACFLAGS = -cp ${top_builddir}/modules/java/libmod-tuscany-java-${PACKAGE_VERSION}.jar:${JAVAROOT}
dist_sample_JAVA = store/*.java
CLEANFILES = *.stamp store/*.class

nobase_dist_sample_DATA = store.composite htdocs/*.html store/*.*

dist_noinst_SCRIPTS = server-test
TESTS = server-test

endif
