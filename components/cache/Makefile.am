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

incl_HEADERS = *.hpp
incldir = $(prefix)/include/components/cache

dist_comp_SCRIPTS = memcached-start memcached-stop
compdir=$(prefix)/components/cache

comp_DATA = memcached.prefix
memcached.prefix: $(top_builddir)/config.status
	echo ${MEMCACHED_PREFIX} >memcached.prefix

EXTRA_DIST = cache.composite memcache.componentType datacache.componentType memocache.componentType *.scm

comp_LTLIBRARIES = libmemcache.la libdatacache.la libmemocache.la
noinst_DATA = libmemcache.so libdatacache.so libmemocache.so

libmemcache_la_SOURCES = memcache.cpp
libmemcache.so:
	ln -s .libs/libmemcache.so

libdatacache_la_SOURCES = datacache.cpp
libdatacache.so:
	ln -s .libs/libdatacache.so

libmemocache_la_SOURCES = memocache.cpp
libmemocache.so:
	ln -s .libs/libmemocache.so

memcache_test_SOURCES = memcache-test.cpp
memcache_test_LDFLAGS = -lxml2

client_test_SOURCES = client-test.cpp
client_test_LDFLAGS = -lxml2 -lcurl -lmozjs

dist_noinst_SCRIPTS = memcached-test memcached-ssl-test server-test
noinst_PROGRAMS = memcache-test client-test
TESTS = memcached-test memcached-ssl-test server-test
