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


dist_sample_SCRIPTS = start stop ssl-start
sampledir = $(prefix)/samples/store-cpp

nobase_dist_sample_DATA = currency-converter.cpp fruits-catalog.cpp shopping-cart.cpp store.composite htdocs/*.html

sample_LTLIBRARIES = libcurrency-converter.la libfruits-catalog.la libshopping-cart.la
noinst_DATA = libcurrency-converter.so libfruits-catalog.so libshopping-cart.so

libcurrency_converter_la_SOURCES = currency-converter.cpp
libcurrency-converter.so:
	ln -s .libs/libcurrency-converter.so

libfruits_catalog_la_SOURCES = fruits-catalog.cpp
libfruits-catalog.so:
	ln -s .libs/libfruits-catalog.so

libshopping_cart_la_SOURCES = shopping-cart.cpp
libshopping-cart.so:
	ln -s .libs/libshopping-cart.so

dist_noinst_SCRIPTS = server-test
TESTS = server-test
