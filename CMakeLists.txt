#***********************************************************************************
# Copyright (c) 2016, UT-Battelle
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#   * Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#   * Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#   * Neither the name of the xacc nor the
#     names of its contributors may be used to endorse or promote products
#     derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# Contributors:
#   Initial API and implementation - Alex McCaskey
#
#**********************************************************************************/
project(XACC CXX)
cmake_minimum_required(VERSION 2.8)
set(CMAKE_DISABLE_IN_SOURCE_BUILDS ON)
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 14)
message(STATUS "C++ version ${CXX_STANDARD} configured.")

# Make sure all build libraries have reference to their dependents
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# Modify our CMAKE_MODULE_PATH
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")
if (EXISTS ${CMAKE_SOURCE_DIR}/tpls/fire)
  message(STATUS "Fire found in tpls")
  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/tpls/fire/cmake/Modules")
  include(tests)
endif() 

find_package(CppMicroServices NO_MODULE REQUIRED)
get_filename_component(CppMicroServices_BASE_DIR ${CppMicroServices_RUNTIME_LIBRARY_DIRS} DIRECTORY)
set(CppMicroServices_INCLUDE_DIR ${CppMicroServices_BASE_DIR}/include/cppmicroservices3)
set(CppMicroServices_LIBRARY_DIR ${CppMicroServices_BASE_DIR}/lib)

# We require MPI
find_package(MPI REQUIRED)

# We require Boost
find_package(Boost COMPONENTS system program_options filesystem python REQUIRED)

# Look for OpenMP, add it we find it
find_package(OpenMP)
if (OPENMP_FOUND)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

# Setup Doxygen
find_package(Doxygen)
if(DOXYGEN_FOUND)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.cmake ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
  add_custom_target(doc ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} COMMENT "Generating API documentation with Doxygen" 
    VERBATIM)
endif(DOXYGEN_FOUND)

# Create a xacc config header file
configure_file("${CMAKE_SOURCE_DIR}/cmake/xacc_config.hpp.in" "${CMAKE_BINARY_DIR}/xacc_config.hpp")
install(FILES "${CMAKE_BINARY_DIR}/xacc_config.hpp" DESTINATION include/xacc)

# Include dirs that everyone will use
include_directories(${CMAKE_BINARY_DIR})
include_directories(${MPI_INCLUDE_PATH})
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/tpls/spdlog)
include_directories(${CppMicroServices_INCLUDE_DIR})
include_directories("${CMAKE_SOURCE_DIR}/xacc")
include_directories("${CMAKE_SOURCE_DIR}/xacc/ir")
include_directories("${CMAKE_SOURCE_DIR}/xacc/compiler")
include_directories("${CMAKE_SOURCE_DIR}/xacc/program")
include_directories("${CMAKE_SOURCE_DIR}/xacc/accelerator")
include_directories("${CMAKE_SOURCE_DIR}/xacc/utils")

# Add subdirectories
add_subdirectory(xacc)
add_subdirectory(quantum)
add_subdirectory(impls)
add_subdirectory(tools)

# Install the config file for external projects to use
# This is cool, users just add find_package(XACC REQUIRED) to 
# their CMake builds, and pass -DXACC_DIR=/path/to/install/xacc
# and CMake loads include paths, libs, etc
configure_file("${CMAKE_SOURCE_DIR}/cmake/xacc-config.cmake.in" "${CMAKE_BINARY_DIR}/xacc-config.cmake")
install(FILES "${CMAKE_BINARY_DIR}/xacc-config.cmake" DESTINATION .)
INSTALL( DIRECTORY "${CMAKE_SOURCE_DIR}/tpls/spdlog" DESTINATION include )
