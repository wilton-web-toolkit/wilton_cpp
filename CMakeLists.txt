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

# project
project ( wilton_cpp CXX )
set ( ${PROJECT_NAME}_STATICLIB_VERSION 1.0 )
set ( ${PROJECT_NAME}_DESCRIPTION "Wilton Web Toolkit C++ library" )
set ( ${PROJECT_NAME}_URL https://github.com/wilton-web-toolkit/wilton_cpp )
include ( ${CMAKE_CURRENT_LIST_DIR}/resources/macros.cmake )

# options
set ( STATICLIB_WITH_ICU ON CACHE BOOL "Build with ICU support" )
set ( staticlib_httpserver_USE_LOG4CPLUS ON CACHE BOOL "Use log4cplus lib for logging" )
set ( staticlib_httpserver_USE_OPENSSL ON CACHE BOOL "Use OpenSSL lib for https" )
if ( NOT DEFINED STATICLIB_DEPS )
    set ( STATICLIB_DEPS ${CMAKE_CURRENT_LIST_DIR}/../../../staticlibs CACHE INTERNAL "" )    
endif ( )
if ( NOT DEFINED WILTON_DEPS )
    set ( WILTON_DEPS ${CMAKE_CURRENT_LIST_DIR}/../.. CACHE INTERNAL "" )
endif ( )

# standalone build
if ( NOT DEFINED CMAKE_LIBRARY_OUTPUT_DIRECTORY )
    set ( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} )
endif ( )
if ( NOT DEFINED STATICLIB_TOOLCHAIN )
    if ( NOT STATICLIB_TOOLCHAIN MATCHES "linux_[^_]+_[^_]+" )    
        wilton_cpp_add_subdirectory ( ${STATICLIB_DEPS}/external_zlib )
        wilton_cpp_add_subdirectory ( ${STATICLIB_DEPS}/external_jansson )
        wilton_cpp_add_subdirectory ( ${STATICLIB_DEPS}/external_log4cplus )
        wilton_cpp_add_subdirectory ( ${STATICLIB_DEPS}/external_openssl )
        wilton_cpp_add_subdirectory ( ${STATICLIB_DEPS}/external_curl )
        wilton_cpp_add_subdirectory ( ${STATICLIB_DEPS}/external_sqlite )
        wilton_cpp_add_subdirectory ( ${STATICLIB_DEPS}/external_icu )
    endif ( )
    wilton_cpp_add_subdirectory ( ${STATICLIB_DEPS}/external_mstch )
    wilton_cpp_add_subdirectory ( ${STATICLIB_DEPS}/external_soci )
    wilton_cpp_add_subdirectory ( ${STATICLIB_DEPS}/staticlib_jni )
    wilton_cpp_add_subdirectory ( ${STATICLIB_DEPS}/staticlib_config )
    wilton_cpp_add_subdirectory ( ${STATICLIB_DEPS}/staticlib_io )
    wilton_cpp_add_subdirectory ( ${STATICLIB_DEPS}/staticlib_ranges )
    wilton_cpp_add_subdirectory ( ${STATICLIB_DEPS}/staticlib_endian )
    wilton_cpp_add_subdirectory ( ${STATICLIB_DEPS}/staticlib_httpserver )
    wilton_cpp_add_subdirectory ( ${STATICLIB_DEPS}/staticlib_utils )
    wilton_cpp_add_subdirectory ( ${STATICLIB_DEPS}/staticlib_icu )
    wilton_cpp_add_subdirectory ( ${STATICLIB_DEPS}/staticlib_serialization )
    wilton_cpp_add_subdirectory ( ${STATICLIB_DEPS}/staticlib_pimpl )
    wilton_cpp_add_subdirectory ( ${STATICLIB_DEPS}/staticlib_orm )
    wilton_cpp_add_subdirectory ( ${STATICLIB_DEPS}/staticlib_unzip )
    wilton_cpp_add_subdirectory ( ${STATICLIB_DEPS}/staticlib_httpclient )
    wilton_cpp_add_subdirectory ( ${STATICLIB_DEPS}/staticlib_cron )
    add_subdirectory ( ${WILTON_DEPS}/libwilton ${CMAKE_BINARY_DIR}/libwilton )
endif ( )

# dependencies
set ( ${PROJECT_NAME}_DEPS_PUBLIC
        icu-uc
        staticlib_config
        staticlib_pimpl )
staticlib_pkg_check_modules ( ${PROJECT_NAME}_DEPS_PUBLIC_PC REQUIRED ${PROJECT_NAME}_DEPS_PUBLIC )
set ( ${PROJECT_NAME}_DEPS_PRIVATE
        staticlib_io
        staticlib_ranges
        staticlib_httpserver
        staticlib_unzip
        staticlib_orm
        staticlib_httpclient
        staticlib_cron
        staticlib_serialization
        staticlib_utils
        log4cplus
        mstch
        soci
        jansson
        libcurl
        libssl
        libcrypto
        zlib 
        sqlite3
        wilton )
wilton_cpp_pkg_check_modules ( ${PROJECT_NAME}_DEPS_PRIVATE_PC REQUIRED ${PROJECT_NAME}_DEPS_PRIVATE )

# build steps
file ( GLOB_RECURSE ${PROJECT_NAME}_SRC ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp )
file ( GLOB_RECURSE ${PROJECT_HEADERS}_SRC ${CMAKE_CURRENT_LIST_DIR}/include/*.hpp )
file ( GLOB_RECURSE ${PROJECT_NAME}_HEADERS_PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src/*.hpp )

add_library ( ${PROJECT_NAME} STATIC
        ${${PROJECT_NAME}_SRC} 
        ${${PROJECT_NAME}_HEADERS}
        ${${PROJECT_NAME}_HEADERS_PRIVATE} )

source_group ( "src" FILES ${${PROJECT_NAME}_SRC} )
source_group ( "include" FILES ${${PROJECT_NAME}_HEADERS} )
source_group ( "include_private" FILES ${${PROJECT_NAME}_HEADERS_PRIVATE} )
target_include_directories ( ${PROJECT_NAME} BEFORE PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/src/
        ${CMAKE_CURRENT_LIST_DIR}/include
        ${${PROJECT_NAME}_DEPS_PUBLIC_PC_INCLUDE_DIRS}
        ${${PROJECT_NAME}_DEPS_PRIVATE_PC_INCLUDE_DIRS} )
target_compile_options ( ${PROJECT_NAME} PRIVATE ${${PROJECT_NAME}_DEPS_PC_CFLAGS_OTHER} )
target_compile_definitions ( ${PROJECT_NAME} PRIVATE -DSTATICLIB_WITH_ICU )

# pkg-config
wilton_cpp_list_to_string ( ${PROJECT_NAME}_PC_CFLAGS "-I" ${PROJECT_NAME}_DEPS_PUBLIC_PC_INCLUDE_DIRS )
set ( ${PROJECT_NAME}_PC_CFLAGS "${${PROJECT_NAME}_PC_CFLAGS} -I${CMAKE_CURRENT_LIST_DIR}/include -DSTATICLIB_WITH_ICU" )
set ( ${PROJECT_NAME}_PC_LIBS "-L${CMAKE_LIBRARY_OUTPUT_DIRECTORY} -l${PROJECT_NAME}" )
if ( CMAKE_SYSTEM_NAME STREQUAL "Linux" )
    set ( ${PROJECT_NAME}_PC_LIBS "${${PROJECT_NAME}_PC_LIBS} -lpthread" )
endif ( )
set ( ${PROJECT_NAME}_DEPS ${${PROJECT_NAME}_DEPS_PUBLIC} ${${PROJECT_NAME}_DEPS_PRIVATE} ) 
wilton_cpp_list_to_string ( ${PROJECT_NAME}_PC_REQUIRES "" ${PROJECT_NAME}_DEPS )
configure_file ( ${CMAKE_CURRENT_LIST_DIR}/resources/pkg-config.in 
        ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/pkgconfig/${PROJECT_NAME}.pc )
