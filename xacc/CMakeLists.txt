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
project(xacc-core LANGUAGES CXX)

set(CMAKE_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)
message(STATUS "C++ version ${CXX_STANDARD} configured.")

# Modify our CMAKE_MODULE_PATH
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${XACC_ROOT}/cmake/Modules")
include(tests)

  # We require Boost without MPI
find_package(Boost COMPONENTS system program_options regex filesystem chrono thread REQUIRED)
  #endif()

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${XACC_ROOT}/tpls/spdlog)
include_directories(${CPPUS_INCLUDE_DIR})
include_directories("${CMAKE_SOURCE_DIR}")
include_directories("${CMAKE_SOURCE_DIR}/ir")
include_directories("${CMAKE_SOURCE_DIR}/compiler")
include_directories("${CMAKE_SOURCE_DIR}/program")
include_directories("${CMAKE_SOURCE_DIR}/accelerator")
include_directories("${CMAKE_SOURCE_DIR}/utils")
include_directories(${CMAKE_BINARY_DIR})

# Create a xacc config header file
configure_file("${XACC_ROOT}/cmake/xacc_config.hpp.in" "${CMAKE_BINARY_DIR}/xacc_config.hpp")
install(FILES "${CMAKE_BINARY_DIR}/xacc_config.hpp" DESTINATION include/xacc)

#set (PACKAGE_NAME "XACC Specification")
#set (PACKAGE_DESCIPTION "The XACC Programming Framework")
set (LIBRARY_NAME xacc)

file (GLOB HEADERS XACC.hpp ir/*.hpp program/*.hpp compiler/*.hpp accelerator/*.hpp accelerator/remote/*.hpp utils/*.hpp)
file (GLOB SRC *.cpp program/*.cpp utils/*.cpp)

set(CppUsLib CppMicroServicesd)
link_directories(${CPPUS_LIBRARY_DIR})
add_library(${LIBRARY_NAME} SHARED ${SRC})
find_library(cppus_has_d_suffix CppMicroServicesd)
if (NOT cppus_has_d_suffix)
	set(CppUsLib CppMicroServices)
endif()
target_link_libraries(${LIBRARY_NAME} ${Boost_LIBRARIES} ${CppUsLib})

# Get the test files
file(GLOB test_files utils/tests/*Tester.cpp accelerator/tests/*Tester.cpp)

# Add the tests
add_tests("${test_files}" "${CMAKE_CURRENT_SOURCE_DIR}/utils" "${Boost_LIBRARIES};xacc")

install(FILES ${HEADERS} DESTINATION include/xacc)
install(TARGETS ${LIBRARY_NAME} DESTINATION lib)
