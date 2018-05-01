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

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${XACC_ROOT}/cmake/Modules")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_INSTALL_PREFIX}/share/cppmicroservices3/cmake")
include(CppMicroServicesConfig)
include(tests)

find_package(Boost COMPONENTS system program_options regex filesystem chrono graph REQUIRED)

include_directories(${CMAKE_INSTALL_PREFIX}/include)
include_directories(${CMAKE_INSTALL_PREFIX}/include/cppmicroservices3)
include_directories(${XACC_BINARY_DIR})
include_directories(${XACC_ROOT}/xacc)
include_directories(${XACC_ROOT}/xacc/ir)
include_directories(${XACC_ROOT}/xacc/compiler)
include_directories(${XACC_ROOT}/xacc/program)
include_directories(${XACC_ROOT}/xacc/accelerator)
include_directories(${XACC_ROOT}/xacc/utils)
include_directories(${XACC_ROOT}/tpls/rapidjson/include)
include_directories(${XACC_ROOT}/tpls/exprtk)
include_directories(${XACC_ROOT}/tpls/spdlog)
include_directories(${XACC_ROOT}/tpls/eigen)

link_directories(${CMAKE_INSTALL_PREFIX}/lib)
link_directories(${XACC_BINARY_DIR}/xacc)
link_directories(${XACC_BINARY_DIR}/lib)

add_subdirectory(gate)
add_subdirectory(aqc)
