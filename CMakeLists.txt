# *******************************************************************************
# Copyright (c) 2017 UT-Battelle, LLC.
# All rights reserved. This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v1.0
# and Eclipse Distribution License v.10 which accompany this distribution. 
# The Eclipse Public License is available at http://www.eclipse.org/legal/epl-v10.html 
# and the Eclipse Distribution License is available at 
# https://eclipse.org/org/documents/edl-v10.php
#
# Contributors:
#   Alexander J. McCaskey - initial API and implementation 
# *******************************************************************************/
project(xacc LANGUAGES CXX)
cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
set(CMAKE_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)
message(STATUS "C++ version ${CXX_STANDARD} configured.")

# Make sure all build libraries have reference to their dependents
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

include(ExternalProject)

if(NOT WIN32)
  string(ASCII 27 Esc)
  set(ColorReset "${Esc}[m")
  set(ColorBold  "${Esc}[1m")
  set(Red         "${Esc}[31m")
  set(Green       "${Esc}[32m")
  set(Yellow      "${Esc}[33m")
  set(Blue        "${Esc}[34m")
  set(Magenta     "${Esc}[35m")
  set(Cyan        "${Esc}[36m")
  set(White       "${Esc}[37m")
  set(BoldRed     "${Esc}[1;31m")
  set(BoldGreen   "${Esc}[1;32m")
  set(BoldYellow  "${Esc}[1;33m")
  set(BoldBlue    "${Esc}[1;34m")
  set(BoldMagenta "${Esc}[1;35m")
  set(BoldCyan    "${Esc}[1;36m")
  set(BoldWhite   "${Esc}[1;37m")
endif()

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	set (CMAKE_INSTALL_PREFIX "$ENV{HOME}/.xacc" CACHE PATH "default install path" FORCE )
endif()

message(STATUS "${BoldGreen}Installing XACC to ${CMAKE_INSTALL_PREFIX}. Override with -DCMAKE_INSTALL_PREFIX=...${ColorReset}")

# Setup Doxygen
find_package(Doxygen)
if(DOXYGEN_FOUND)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.cmake ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
  add_custom_target(doc ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} COMMENT "Generating API documentation with Doxygen" 
    VERBATIM)
endif(DOXYGEN_FOUND)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER "7.0.0")
        set (CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-Wimplicit-fallthrough=0")
endif()

ExternalProject_Add(cppmicroservices
	GIT_REPOSITORY https://github.com/cppmicroservices/cppmicroservices
	GIT_TAG v3.1.0	
	CMAKE_ARGS -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
               -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
               -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
               -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
		       -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
        INSTALL_COMMAND ${CMAKE_MAKE_PROGRAM} install 
)

# Make it so you don't always have to 
# specify openssl dir on mac
#if(APPLE AND NOT OPENSSL_ROOT_DIR)
#        if (EXISTS /usr/local/opt/openssl)
#                message(STATUS "${BoldGreen}[OSX] Searching for OpenSSL from /usr/local/opt/openssl. Override with -DOPENSSL_ROOT_DIR=...${ColorReset}")
#                set(OPENSSL_ROOT_DIR /usr/local/opt/openssl)
#        endif()
#elseif(EXISTS /etc/redhat-release AND NOT OPENSSL_ROOT_DIR)
   # we are on fedora... check for custom openssl install
   # in /usr/local/ssl
#   if (EXISTS /usr/local/ssl)
#                message(STATUS "${BoldGreen}[Fedora] Searching for OpenSSL from /usr/local/ssl. Override with -DOPENSSL_ROOT_DIR=...${ColorReset}")
#                set(OPENSSL_ROOT_DIR /usr/local/ssl)
#   endif()
#endif()
#find_package(OpenSSL 1.0.2 REQUIRED)
#message(STATUS "${BoldGreen}Found OpenSSL version ${OPENSSL_VERSION}${ColorReset} at ${OPENSSL_ROOT_DIR}")

#ExternalProject_Add(cpprestsdk
#    DEPENDS cppmicroservices
#	GIT_REPOSITORY https://github.com/Microsoft/cpprestsdk
#	GIT_TAG master
#	CONFIGURE_COMMAND ${CMAKE_COMMAND} ../cpprestsdk/Release/ -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
#               -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
#               -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
#               -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
#		   	   -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
#		   	   -DOPENSSL_ROOT_DIR=${OPENSSL_ROOT_DIR}
#		   	   -DBUILD_TESTS=OFF
#    INSTALL_COMMAND ${CMAKE_MAKE_PROGRAM} install 
#)
ExternalProject_Add(restclient-cpp
    DEPENDS cppmicroservices
	GIT_REPOSITORY https://github.com/mrtazz/restclient-cpp
	GIT_TAG master
	BUILD_IN_SOURCE 1
	CONFIGURE_COMMAND sh autogen.sh && sh ./configure CC=${CMAKE_C_COMPILER} CXX=${CMAKE_CXX_COMPILER} --prefix=${CMAKE_INSTALL_PREFIX}
    INSTALL_COMMAND ${CMAKE_MAKE_PROGRAM} install 
)

set(CppMicroServices_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include/cppmicroservices3)
set(CppMicroServices_LIBRARY_DIR ${CMAKE_INSTALL_PREFIX}/lib)

set(CppRSDK_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include)
set(CppRSDK_LIBRARY_DIR ${CMAKE_INSTALL_PREFIX}/lib)

# Look for OpenMP, add it we find it
find_package(OpenMP)
if (OPENMP_FOUND)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

find_package(PythonLibs)
if(PYTHONLIBS_FOUND)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DXACC_HAS_PYTHON")
endif()

ExternalProject_Add(xacc-core
   DEPENDS cppmicroservices restclient-cpp
   SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/xacc
   CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
              -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
              -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
              -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
              -DBOOST_ROOT=${BOOST_ROOT}
	      -DXACC_ROOT=${CMAKE_CURRENT_SOURCE_DIR}
	      -DCPPUS_INCLUDE_DIR=${CppMicroServices_INCLUDE_DIR}
	      -DCPPUS_LIBRARY_DIR=${CppMicroServices_LIBRARY_DIR}
	      -DCppRSDK_INCLUDE_DIR=${CppRSDK_INCLUDE_DIR}
	      #-DXACC_HAS_PYTHON=${PYTHONLIBS_FOUND}
	      -DXACC_PYTHON_INCLUDE_DIR=${PYTHON_INCLUDE_DIR}
	      -DXACC_PYTHON_LIBRARIES=${PYTHON_LIBRARIES}
	      -DPYBIND11_DIR=${CMAKE_SOURCE_DIR}/python/pybind11
#              -DOPENSSL_ROOT_DIR=${OPENSSL_ROOT_DIR}
#	      -DOPENSSL_INCLUDE_DIR=${OPENSSL_INCLUDE_DIR}
#	      -DOPENSSL_LIBRARIES=${OPENSSL_LIBRARIES}
#	      -DOPENSSL_CRYPTO_LIBRARY=${OPENSSL_CRYPTO_LIBRARY}
   CMAKE_CACHE_ARGS -DCMAKE_PREFIX_PATH:PATH=${CMAKE_PREFIX_PATH}
   BUILD_ALWAYS 1
   INSTALL_COMMAND ${CMAKE_MAKE_PROGRAM} install # DESTDIR=${CMAKE_BINARY_DIR}/stage
   TEST_BEFORE_INSTALL 1
   )

#set(XACC_BASE_DIR ${CMAKE_BINARY_DIR}/stage${CMAKE_INSTALL_PREFIX})
#set(XACC_INCLUDE_DIR ${XACC_BASE_DIR}/include/xacc)
#set(XACC_LIBRARY_DIR ${XACC_BASE_DIR}/lib)
#string (REPLACE ";" " " XACC_Boost_LIBRARIES "${Boost_LIBRARIES}")
#string (REPLACE ";" " " XACC_MPI_INCLUDE_PATHS "${MPI_CXX_INCLUDE_PATH}")

ExternalProject_Add(xacc-quantum
   DEPENDS xacc-core
   SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/quantum
   CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
              -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
              -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
              -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
	      -DXACC_ROOT=${CMAKE_CURRENT_SOURCE_DIR}
	      -DBOOST_ROOT=${BOOST_ROOT}
              -DCPPUS_INCLUDE_DIR=${CppMicroServices_INCLUDE_DIR}
              -DCPPUS_LIBRARY_DIR=${CppMicroServices_LIBRARY_DIR}
	      -DXACC_INCLUDE_DIR=${CMAKE_INSTALL_PREFIX}/include/xacc
	      -DXACC_LIBRARY_DIR=${CMAKE_INSTALL_PREFIX}/lib
#	      -DCppRSDK_INCLUDE_DIR=${CppRSDK_INCLUDE_DIR}
#	      -DOPENSSL_INCLUDE_DIR=${OPENSSL_INCLUDE_DIR}
#	      -DOPENSSL_LIBRARIES=${OPENSSL_LIBRARIES}
#	      -DOPENSSL_CRYPTO_LIBRARY=${OPENSSL_CRYPTO_LIBRARY}
   CMAKE_CACHE_ARGS -DCMAKE_PREFIX_PATH:PATH=${CMAKE_PREFIX_PATH}
   BUILD_ALWAYS 1
   INSTALL_COMMAND ${CMAKE_MAKE_PROGRAM} install #DESTDIR=${CMAKE_BINARY_DIR}/stage
   TEST_AFTER_INSTALL 1
   )

if(PYTHONLIBS_FOUND)
ExternalProject_Add(xacc-python
   DEPENDS xacc-core xacc-quantum
   SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/python
   CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
              -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
              -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
              -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
	      -DXACC_ROOT=${CMAKE_CURRENT_SOURCE_DIR}
	      -DBOOST_ROOT=${BOOST_ROOT}
	      -DCPPUS_INCLUDE_DIR=${CppMicroServices_INCLUDE_DIR}
              -DCPPUS_LIBRARY_DIR=${CppMicroServices_LIBRARY_DIR}
	      -DXACC_INCLUDE_DIR=${CMAKE_INSTALL_PREFIX}/include #${XACC_INCLUDE_DIR}
	      -DXACC_LIBRARY_DIR=${CMAKE_INSTALL_PREFIX}/lib #${XACC_LIBRARY_DIR}
	      -DPYTHON_EXECUTABLE=${PYTHON_EXECUTABLE}
	      -DFROM_SETUP_PY=${FROM_SETUP_PY}
	      -DXACC_PYTHON_INCLUDE_DIR=${PYTHON_INCLUDE_DIR}
 #             -DCppRSDK_INCLUDE_DIR=${CppRSDK_INCLUDE_DIR}
  #            -DOPENSSL_INCLUDE_DIR=${OPENSSL_INCLUDE_DIR}
 #             -DOPENSSL_LIBRARIES=${OPENSSL_LIBRARIES}
  #            -DOPENSSL_CRYPTO_LIBRARY=${OPENSSL_CRYPTO_LIBRARY}
   CMAKE_CACHE_ARGS -DCMAKE_PREFIX_PATH:PATH=${CMAKE_PREFIX_PATH}
   BUILD_ALWAYS 1
   INSTALL_COMMAND ${CMAKE_MAKE_PROGRAM} install #DESTDIR=${CMAKE_BINARY_DIR}/stage
   TEST_AFTER_INSTALL 1
   )
endif()

# Install the config file for external projects to use
# This is cool, users just add find_package(XACC REQUIRED) to 
# their CMake builds, and pass -DXACC_DIR=/path/to/install/xacc
# and CMake loads include paths, libs, etc
configure_file("${CMAKE_SOURCE_DIR}/cmake/xacc-config.cmake.in" "${CMAKE_BINARY_DIR}/xacc-config.cmake" @ONLY)
install(FILES "${CMAKE_BINARY_DIR}/xacc-config.cmake" DESTINATION .)
install(FILES "${CMAKE_SOURCE_DIR}/cmake/Modules/tests.cmake" DESTINATION share/xacc/)
INSTALL( DIRECTORY "${CMAKE_SOURCE_DIR}/tpls/spdlog" DESTINATION include )
INSTALL( DIRECTORY "${CMAKE_SOURCE_DIR}/tpls/exprtk" DESTINATION include )
INSTALL( DIRECTORY "${CMAKE_SOURCE_DIR}/tpls/eigen" DESTINATION include )
INSTALL( DIRECTORY "${CMAKE_SOURCE_DIR}/tpls/rapidjson" DESTINATION include )
INSTALL( DIRECTORY "${CMAKE_SOURCE_DIR}/tpls/pybind11" DESTINATION include )
install(PROGRAMS "${CMAKE_SOURCE_DIR}/tools/framework/xacc-framework" DESTINATION bin)
