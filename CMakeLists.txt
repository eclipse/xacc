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

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")

if (EXISTS ${CMAKE_SOURCE_DIR}/tpls/fire)
  message(STATUS "Fire found in tpls")
  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/tpls/fire/cmake/Modules")
  include(tests)
endif() 

find_package(MPI REQUIRED)
find_package(Boost COMPONENTS system program_options REQUIRED)
#find_package(LLVM 3.8.1 REQUIRED)

#message(STATUS "Found LLVM ${LLVM_ROOT_DIR}")
#message(STATUS "LLVM Includes: ${LLVM_INCLUDE_DIRS}")
#message(STATUS "LLVM LIBRARIES: ${LLVM_LIBRARIES}")

message(STATUS "Found Boost Headers = ${Boost_INCLUDE_DIRS}")
message(STATUS "Found Boost Libraries = ${Boost_LIBRARIES}")

find_package(OpenMP)
if (OPENMP_FOUND)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

include_directories(${MPI_INCLUDE_PATH})
include_directories(${Boost_INCLUDE_DIRS})
include_directories("${CMAKE_SOURCE_DIR}/xacc")
include_directories("${CMAKE_SOURCE_DIR}/xacc/compiler")
include_directories("${CMAKE_SOURCE_DIR}/xacc/program")
include_directories("${CMAKE_SOURCE_DIR}/xacc/accelerator")
include_directories("${CMAKE_SOURCE_DIR}/xacc/utils")

add_subdirectory(tpls)
add_subdirectory(xacc)
add_subdirectory(quantum)
add_subdirectory(tools)
add_subdirectory(examples)
