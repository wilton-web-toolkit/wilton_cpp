# Copyright 2016, alex at staticlibs.net
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

cmake_minimum_required ( VERSION 2.8.12 )

# toolchain setup
if ( NOT DEFINED STATICLIB_DEPS )
    set ( STATICLIB_DEPS ${CMAKE_CURRENT_LIST_DIR}/../../../staticlibs CACHE INTERNAL "" )    
endif ( )
set ( STATICLIB_TOOLCHAIN linux_amd64_gcc CACHE STRING "toolchain triplet" )
if ( NOT DEFINED STATICLIB_CMAKE )
    set ( STATICLIB_CMAKE ${STATICLIB_DEPS}/cmake CACHE INTERNAL "" )    
endif ( )
set ( CMAKE_TOOLCHAIN_FILE ${STATICLIB_CMAKE}/toolchains/${STATICLIB_TOOLCHAIN}.cmake CACHE INTERNAL "" )

# project
project ( wilton_cpp_test CXX C )
include ( ${STATICLIB_CMAKE}/staticlibs_common.cmake )
staticlib_enable_deplibs_cache ( )

# options
set ( STATICLIB_WITH_ICU ON CACHE BOOL "Build test always with ICU support" )
set ( staticlib_httpserver_USE_LOG4CPLUS ON CACHE BOOL "Use log4cplus lib for logging" )
set ( staticlib_httpserver_USE_OPENSSL ON CACHE BOOL "Use OpenSSL lib for https" )

# dependencies
if ( NOT STATICLIB_TOOLCHAIN MATCHES "linux_[^_]+_[^_]+" )    
    staticlib_add_subdirectory ( ${STATICLIB_DEPS}/external_zlib )
    staticlib_add_subdirectory ( ${STATICLIB_DEPS}/external_jansson )
    staticlib_add_subdirectory ( ${STATICLIB_DEPS}/external_log4cplus )
    staticlib_add_subdirectory ( ${STATICLIB_DEPS}/external_openssl )
    staticlib_add_subdirectory ( ${STATICLIB_DEPS}/external_curl )
    staticlib_add_subdirectory ( ${STATICLIB_DEPS}/external_sqlite )
    staticlib_add_subdirectory ( ${STATICLIB_DEPS}/external_icu )
endif ( )
staticlib_add_subdirectory ( ${STATICLIB_DEPS}/external_soci )
staticlib_add_subdirectory ( ${STATICLIB_DEPS}/staticlib_config )
staticlib_add_subdirectory ( ${STATICLIB_DEPS}/staticlib_io )
staticlib_add_subdirectory ( ${STATICLIB_DEPS}/staticlib_ranges )
staticlib_add_subdirectory ( ${STATICLIB_DEPS}/staticlib_endian )
staticlib_add_subdirectory ( ${STATICLIB_DEPS}/staticlib_httpserver )
staticlib_add_subdirectory ( ${STATICLIB_DEPS}/staticlib_utils )
staticlib_add_subdirectory ( ${STATICLIB_DEPS}/staticlib_icu )
staticlib_add_subdirectory ( ${STATICLIB_DEPS}/staticlib_serialization )
staticlib_add_subdirectory ( ${STATICLIB_DEPS}/staticlib_pimpl )
staticlib_add_subdirectory ( ${STATICLIB_DEPS}/staticlib_unzip )
staticlib_add_subdirectory ( ${STATICLIB_DEPS}/staticlib_orm )
staticlib_add_subdirectory ( ${STATICLIB_DEPS}/staticlib_httpclient )
staticlib_add_subdirectory ( ${STATICLIB_DEPS}/staticlib_cron )
staticlib_add_subdirectory ( ${STATICLIB_DEPS}/staticlib_tinydir )
staticlib_add_subdirectory ( ${STATICLIB_DEPS}/staticlib_mustache )
add_subdirectory ( ${CMAKE_CURRENT_LIST_DIR}/../../libwilton ${CMAKE_BINARY_DIR}/libwilton )
staticlib_add_subdirectory ( ${CMAKE_CURRENT_LIST_DIR}/../../wilton_cpp )

set ( ${PROJECT_NAME}_DEPS_PUBLIC wilton_cpp )
staticlib_pkg_check_modules ( ${PROJECT_NAME}_DEPS_PUBLIC_PC REQUIRED ${PROJECT_NAME}_DEPS_PUBLIC )
set ( ${PROJECT_NAME}_DEPS_PRIVATE
        staticlib_config
        staticlib_pimpl
        staticlib_io
        staticlib_ranges
        staticlib_httpserver
        staticlib_unzip
        staticlib_orm
        staticlib_httpclient
        staticlib_cron
        staticlib_mustache
        staticlib_tinydir
        staticlib_serialization
        staticlib_utils
        staticlib_icu
        icu-uc
        log4cplus
        soci
        jansson
        libcurl
        openssl
        zlib 
        sqlite3
        wilton )
staticlib_pkg_check_modules ( ${PROJECT_NAME}_DEPS_PRIVATE_PC REQUIRED ${PROJECT_NAME}_DEPS_PRIVATE )

# tests
set ( ${PROJECT_NAME}_TEST_INCLUDES ${${PROJECT_NAME}_DEPS_PUBLIC_PC_INCLUDE_DIRS} )
set ( ${PROJECT_NAME}_TEST_LIBS 
        ${${PROJECT_NAME}_DEPS_PUBLIC_PC_LIBRARIES}
        ${${PROJECT_NAME}_DEPS_PRIVATE_PC_LIBRARIES} )
set ( ${PROJECT_NAME}_TEST_OPTS ${${PROJECT_NAME}_DEPS_PUBLIC_PC_CFLAGS_OTHER} )
staticlib_enable_testing ( ${PROJECT_NAME}_TEST_INCLUDES ${PROJECT_NAME}_TEST_LIBS ${PROJECT_NAME}_TEST_OPTS )
