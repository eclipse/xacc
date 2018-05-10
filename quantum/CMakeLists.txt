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

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_BINARY_DIR}/tpls/cppmicroservices")
include(CppMicroServicesConfig)


include_directories(${CMAKE_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR}/xacc)
include_directories(${CMAKE_SOURCE_DIR}/xacc/ir)
include_directories(${CMAKE_SOURCE_DIR}/xacc/compiler)
include_directories(${CMAKE_SOURCE_DIR}/xacc/program)
include_directories(${CMAKE_SOURCE_DIR}/xacc/accelerator)
include_directories(${CMAKE_SOURCE_DIR}/xacc/utils)
include_directories(${CMAKE_SOURCE_DIR}/tpls/rapidjson/include)
include_directories(${CMAKE_SOURCE_DIR}/tpls/exprtk)
include_directories(${CMAKE_SOURCE_DIR}/tpls/spdlog)
include_directories(${CMAKE_SOURCE_DIR}/tpls/eigen)
include_directories(${GTEST_INCLUDE_DIRS})

add_subdirectory(gate)
add_subdirectory(aqc)
