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

option(XACC_BUILD_TESTS "Build test programs" OFF)
option(XACC_BUILD_EXAMPLES "Build example programs" OFF)
option(XACC_ENSMALLEN_INCLUDE_DIR "Path to ensmallen.hpp for mlpack optimizer" "")
option(XACC_ARMADILLO_INCLUDE_DIR "Path to armadillo header for mlpack optimizer" "")

if (FROM_SETUP_PY AND NOT APPLE)
   message(STATUS "Running build from setup.py, linking to static libstdc++")
   set (CMAKE_SHARED_LINKER_FLAGS "-static-libstdc++" CACHE INTERNAL "" FORCE)
endif()

find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${CMAKE_SOURCE_DIR}/.git")
    # Update submodules as needed
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()
endif()

if(NOT EXISTS "${CMAKE_SOURCE_DIR}/tpls/cppmicroservices/CMakeLists.txt"
    OR NOT EXISTS "${CMAKE_SOURCE_DIR}/tpls/boost-cmake/CMakeLists.txt"
    OR NOT EXISTS "${CMAKE_SOURCE_DIR}/tpls/cpr/CMakeLists.txt")
    message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()

set(CMAKE_DEBUG_POSTFIX "")
set(CMAKE_SKIP_INSTALL_RPATH OFF)
set(CMAKE_SKIP_RPATH OFF)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

LIST(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
IF("${isSystemDir}" STREQUAL "-1")
   SET(CMAKE_INSTALL_RPATH "@loader_path")
ENDIF("${isSystemDir}" STREQUAL "-1")

if (NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE "Release" CACHE STRING
	   "Choose the type of build, options are: Debug, Release, RelWithDebInfo, MinSizeRel"
	   FORCE
      )
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")
include(format)
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

if (XACC_BUILD_TESTS)
   enable_testing()
   macro(add_xacc_test _TEST_NAME)
       add_executable(${_TEST_NAME}Tester ${_TEST_NAME}Tester.cpp)
       target_include_directories(${_TEST_NAME}Tester PRIVATE ${GTEST_INCLUDE_DIRS})
       add_test(NAME xacc_${_TEST_NAME}Tester COMMAND ${_TEST_NAME}Tester)
       target_link_libraries(${_TEST_NAME}Tester ${GTEST_LIBRARIES} xacc)
   endmacro()
   macro(set_cache_variable VAR_NAME VAR_DESCRIPTION)
       set(${VAR_NAME} ${${VAR_NAME}} CACHE INTERNAL ${VAR_DESCRIPTION})
       message(STATUS "Set ${VAR_NAME} to ${${VAR_NAME}}.")
   endmacro()
endif()

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-attributes")
include_directories(${CMAKE_BINARY_DIR}/tpls/cppmicroservices/include)
add_subdirectory(tpls)

add_subdirectory(xacc)
add_subdirectory(quantum)
if(PYTHON_INCLUDE_DIR)
 add_subdirectory(python)
endif()

# Install the config file for external projects to use
# This is cool, users just add find_package(XACC REQUIRED) to
# their CMake builds, and pass -DXACC_DIR=/path/to/install/xacc
# and CMake loads include paths, libs, etc
configure_file("${CMAKE_SOURCE_DIR}/cmake/xacc-config.cmake.in" "${CMAKE_BINARY_DIR}/xacc-config.cmake" @ONLY)
install(FILES "${CMAKE_BINARY_DIR}/xacc-config.cmake" DESTINATION .)
install(FILES "${CMAKE_SOURCE_DIR}/cmake/Modules/format.cmake" DESTINATION share/xacc/)
install(FILES "${CMAKE_SOURCE_DIR}/tpls/mpark-variant/variant.hpp" DESTINATION include/xacc/)
install(FILES "${CMAKE_SOURCE_DIR}/tpls/taocpp/operators.hpp" DESTINATION include/xacc/)
install(FILES "${CMAKE_SOURCE_DIR}/tpls/nlohmann/json.hpp" DESTINATION include/xacc/)

install( DIRECTORY "${CMAKE_SOURCE_DIR}/tpls/spdlog" DESTINATION include )
install( DIRECTORY "${CMAKE_SOURCE_DIR}/tpls/exprtk" DESTINATION include )
install( DIRECTORY "${CMAKE_SOURCE_DIR}/tpls/eigen" DESTINATION include )
install( DIRECTORY "${CMAKE_SOURCE_DIR}/tpls/rapidjson" DESTINATION include )
install( DIRECTORY "${CMAKE_SOURCE_DIR}/tpls/pybind11" DESTINATION include )
