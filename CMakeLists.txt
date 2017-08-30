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
project(xacc LANGUAGES CXX)
cmake_minimum_required(VERSION 3.3 FATAL_ERROR)
#set(CMAKE_DISABLE_IN_SOURCE_BUILDS ON)
#set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 14)
message(STATUS "C++ version ${CXX_STANDARD} configured.")

# Make sure all build libraries have reference to their dependents
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set (CMAKE_INSTALL_PREFIX "/usr/local/xacc" CACHE PATH "default install path" FORCE )
endif()

#find_package(CppMicroServices NO_MODULE REQUIRED)
#get_filename_component(CppMicroServices_BASE_DIR ${CppMicroServices_RUNTIME_LIBRARY_DIRS} DIRECTORY)
#set(CppMicroServices_INCLUDE_DIR ${CppMicroServices_BASE_DIR}/include/cppmicroservices3)
#set(CppMicroServices_LIBRARY_DIR ${CppMicroServices_BASE_DIR}/lib)

# Setup Doxygen
find_package(Doxygen)
if(DOXYGEN_FOUND)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.cmake ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
  add_custom_target(doc ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} COMMENT "Generating API documentation with Doxygen" 
    VERBATIM)
endif(DOXYGEN_FOUND)

include(ExternalProject)

set(STAGED_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/stage${CMAKE_INSTALL_PREFIX})

add_subdirectory(tpls)

set(CppMicroServices_BASE_DIR ${CMAKE_BINARY_DIR}/cppus_install/usr/local)
set(CppMicroServices_INCLUDE_DIR ${CppMicroServices_BASE_DIR}/include/cppmicroservices3)
set(CppMicroServices_LIBRARY_DIR ${CppMicroServices_BASE_DIR}/lib)

# We require MPI
find_package(MPI)
if (MPI_FOUND)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DXACC_HAS_MPI")

  # We require Boost with MPI
  find_package(Boost COMPONENTS mpi system program_options filesystem chrono thread REQUIRED)
  include_directories(${MPI_INCLUDE_PATH})
else()
  message(STATUS "MPI Not Found.")

  # We require Boost without MPI
  find_package(Boost COMPONENTS system program_options filesystem chrono thread REQUIRED)
endif()

# Look for OpenMP, add it we find it
find_package(OpenMP)
if (OPENMP_FOUND)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

ExternalProject_Add(xacc-core
   DEPENDS cppmicroservices
   SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/xacc
   CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
              -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
              -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
              -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
              -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
              -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
	      -DXACC_ROOT=${CMAKE_CURRENT_SOURCE_DIR}
	      -DCPPUS_INCLUDE_DIR=${CppMicroServices_INCLUDE_DIR}
	      -DCPPUS_LIBRARY_DIR=${CppMicroServices_LIBRARY_DIR}
	      -DBoost_INCLUDE_DIRS=${BOOST_INCLUDE_DIRS}
	      -DBoost_LIBRARIES=${Boost_LIBRARIES}
	      -DMPI_INCLUDE_PATH=${MPI_INCLUDE_PATH}
   CMAKE_CACHE_ARGS -DCMAKE_PREFIX_PATH:PATH=${CMAKE_PREFIX_PATH}
   BUILD_ALWAYS 1
   INSTALL_COMMAND ${CMAKE_MAKE_PROGRAM} install DESTDIR=${CMAKE_BINARY_DIR}/stage
   TEST_BEFORE_INSTALL 1
   )

set(XACC_BASE_DIR ${CMAKE_BINARY_DIR}/stage${CMAKE_INSTALL_PREFIX})
set(XACC_INCLUDE_DIR ${XACC_BASE_DIR}/include/xacc)
set(XACC_LIBRARY_DIR ${XACC_BASE_DIR}/lib)

string (REPLACE ";" " " XACC_Boost_LIBRARIES "${Boost_LIBRARIES}")
ExternalProject_Add(xacc-quantum
   DEPENDS xacc-core
   SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/quantum
   CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
              -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
              -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
              -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
              -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
              -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
              -DXACC_ROOT=${CMAKE_CURRENT_SOURCE_DIR}
              -DCPPUS_INCLUDE_DIR=${CppMicroServices_INCLUDE_DIR}
              -DCPPUS_LIBRARY_DIR=${CppMicroServices_LIBRARY_DIR}
	      -DXACC_INCLUDE_DIR=${XACC_INCLUDE_DIR}
	      -DXACC_LIBRARY_DIR=${XACC_LIBRARY_DIR}
	      -DBoost_INCLUDE_DIRS=${BOOST_INCLUDE_DIRS}
	      -DXACC_Boost_LIBRARIES=${XACC_Boost_LIBRARIES}
	      -DMPI_INCLUDE_PATH=${MPI_INCLUDE_PATH}
   CMAKE_CACHE_ARGS -DCMAKE_PREFIX_PATH:PATH=${CMAKE_PREFIX_PATH}
   BUILD_ALWAYS 1
   INSTALL_COMMAND ${CMAKE_MAKE_PROGRAM} install DESTDIR=${CMAKE_BINARY_DIR}/stage
   TEST_AFTER_INSTALL 1
   )

# Install the config file for external projects to use
# This is cool, users just add find_package(XACC REQUIRED) to 
# their CMake builds, and pass -DXACC_DIR=/path/to/install/xacc
# and CMake loads include paths, libs, etc
configure_file("${CMAKE_SOURCE_DIR}/cmake/xacc-config.cmake.in" "${CMAKE_BINARY_DIR}/xacc-config.cmake")
install(FILES "${CMAKE_BINARY_DIR}/xacc-config.cmake" DESTINATION .)
INSTALL( DIRECTORY "${CMAKE_SOURCE_DIR}/tpls/spdlog" DESTINATION include )
install( DIRECTORY "${CMAKE_BINARY_DIR}/cppus_install/usr/local/bin/" DESTINATION bin) 
install( DIRECTORY "${CMAKE_BINARY_DIR}/cppus_install/usr/local/include/" DESTINATION include) 
install( DIRECTORY "${CMAKE_BINARY_DIR}/cppus_install/usr/local/lib/" DESTINATION lib/cppmicroservices) 
install( DIRECTORY "${CMAKE_BINARY_DIR}/cppus_install/usr/local/share/" DESTINATION share) 
install( DIRECTORY "${CMAKE_BINARY_DIR}/stage/usr/local/xacc/" DESTINATION .) 
