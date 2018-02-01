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
cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(xacc-quantum LANGUAGES CXX)

set(CMAKE_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)
message(STATUS "C++ version ${CXX_STANDARD} configured.")
# Make sure all build libraries have reference to their dependents
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

set(CppMicroServices_DIR "${CPPUS_LIBRARY_DIR}/../share/cppmicroservices3/cmake")

# Modify our CMAKE_MODULE_PATH
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${XACC_ROOT}/cmake/Modules")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CppMicroServices_DIR}")
include(tests)
include(CppMicroServicesConfig)

  # We require Boost without MPI
find_package(Boost COMPONENTS system program_options filesystem chrono thread REQUIRED)
  #endif()

include_directories(${XACC_INCLUDE_DIR})
include_directories(${XACC_ROOT}/tpls/rapidjson/include)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${XACC_ROOT}/tpls/spdlog)
include_directories(${XACC_ROOT}/tpls/eigen)
include_directories(${CPPUS_INCLUDE_DIR})
include_directories(${CMAKE_BINARY_DIR})

link_directories(${XACC_LIBRARY_DIR})
link_directories(${CPPUS_LIBRARY_DIR})

include_directories(${OPENSSL_INCLUDE_DIR})
include_directories(${CppRSDK_INCLUDE_DIR})

add_subdirectory(gate)
add_subdirectory(aqc)
