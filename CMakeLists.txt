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
cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(xacc VERSION 1.0.0 LANGUAGES CXX)
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
if(GIT_FOUND AND EXISTS "${CMAKE_SOURCE_DIR}/.git" AND NOT EXISTS "${CMAKE_SOURCE_DIR}/tpls/boost-cmake/CMakeLists.txt")
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

    execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD WORKING_DIRECTORY "${local_dir}"
                            OUTPUT_VARIABLE XACC_BUILD_VERSION ERROR_QUIET
                            OUTPUT_STRIP_TRAILING_WHITESPACE)
  message( STATUS "XACC GIT hash: ${XACC_BUILD_VERSION}")
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
include(cxxFlags)
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

IF(CMAKE_BUILD_TYPE MATCHES "Debug" OR CMAKE_BUILD_TYPE MATCHES "DEBUG")
  message(STATUS "${BoldYellow}Adding _XACC_DEBUG compiler flag${ColorReset}")
  add_compile_definitions(_XACC_DEBUG)
ENDIF()

# Find LAPACK (optional) 
find_package(LAPACK)

add_subdirectory(xacc)
add_subdirectory(quantum)

find_package(Python COMPONENTS Interpreter Development)
if(Python_FOUND)
 if(${Python_VERSION} VERSION_GREATER_EQUAL 3.0.0)
   message(STATUS "${BoldGreen}Found Python version ${Python_VERSION}. Building XACC Python API with ${Python_INCLUDE_DIRS}${ColorReset}")
   add_subdirectory(python)
   add_subdirectory(${CMAKE_SOURCE_DIR}/quantum/python)
   # Double check we have module ipopo installed, contributes pelix
   execute_process(COMMAND ${Python_EXECUTABLE} -c "import pelix" RESULT_VARIABLE PELIX_EXISTS)
   if (PELIX_EXISTS EQUAL "1")
     # if not, check we have pip
     execute_process(COMMAND ${Python_EXECUTABLE} -c "import pip" RESULT_VARIABLE PIP_EXISTS)
     if (PIP_EXISTS EQUAL "0")
	# we have pip, son just install ipopo
        message(STATUS "${BoldGreen}Installing Pelix OSGi framework.${ColorReset}")
        execute_process(COMMAND ${Python_EXECUTABLE} -m pip install ipopo)
     else()
	# we dont have pip, so warn the user
	message(STATUS "${BoldYellow}Pelix Framework not found, but can't install via pip. Ensure you install ipopo module before using XACC Python API.${ColorReset}")
     endif()
   else()
     message(STATUS "${BoldGreen}Found Pelix framework.${ColorReset}")
   endif()
 else()
    message(STATUS "${BoldYellow}Found Python version ${Python_VERSION}. Version must be greater than 3.0.0, skipping Python API build.${ColorReset}")
 endif()
else()
  message(STATUS "${BoldYellow}Python interpreter or development headers not found. Skipping Python API build.${ColorReset}")
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

if (XACC_CPACK_DEB_PLATFORM)
message(STATUS "CPack DEB Build Enabled.") 
set(CPACK_SET_DESTDIR "on")
set(CPACK_PACKAGING_INSTALL_PREFIX "/tmp")
set(CPACK_GENERATOR "DEB")

set(MAJOR_VERSION 1)
set(MINOR_VERSION 0)
set(PATCH_VERSION 0)

set(CPACK_PACKAGE_DESCRIPTION "xacc quantum programming framework")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "xacc provides a service-oriented architecture enabling the programming, compilation, and execution of quantum code on quantum co-processors.")
set(CPACK_PACKAGE_VENDOR "ORNL")
set(CPACK_PACKAGE_CONTACT "mccaskeyaj@ornl.gov")
set(CPACK_PACKAGE_VERSION_MAJOR "${MAJOR_VERSION}")
set(CPACK_PACKAGE_VERSION_MINOR "${MINOR_VERSION}")
set(CPACK_PACKAGE_VERSION_PATCH "${PATCH_VERSION}")
set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}")
set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_SOURCE_DIR}/scripts/debian/postinst")

#dependencies for this service menu
if (${XACC_CPACK_DEB_PLATFORM} STREQUAL "bionic")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "gcc-8, g++-8, libblas-dev, liblapack-dev, python3, python3-pip, libpython3-dev, libcurl4-openssl-dev, libssl-dev")
elseif(${XACC_CPACK_DEB_PLATFORM} STREQUAL "focal")
 set(CPACK_DEBIAN_PACKAGE_DEPENDS "gcc-10, g++-10, libblas-dev, liblapack-dev, python3, python3-pip, libpython3-dev, libcurl4-openssl-dev, libssl-dev")
endif()
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_PACKAGE_SECTION "base")
set(CPACK_DEBIAN_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR})
 
include(CPack)
endif()

if (XACC_CPACK_RPM_PLATFORM)
message(STATUS "CPack RPM Build Enabled.") 
set(CPACK_SET_DESTDIR "on")
set(CPACK_PACKAGING_INSTALL_PREFIX "/tmp")
set(CPACK_GENERATOR "RPM")

set(CPACK_COMPONENTS_ALL DIST DEVEL)
set(CPACK_COMPONENTS_GROUPING ONE_PER_GROUP)
set(CPACK_COMPONENTS_IGNORE_GROUPS 1)
SET(CPACK_RPM_PACKAGE_GROUP "Development/Libraries")
set(CPACK_RPM_DIST_POST_INSTALL_SCRIPT_FILE ${CMAKE_SOURCE_DIR}/scripts/debian/postinst)

set(MAJOR_VERSION 1)
set(MINOR_VERSION 0)
set(PATCH_VERSION 0)

set(CPACK_PACKAGE_DESCRIPTION "xacc quantum programming framework")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "xacc provides a service-oriented architecture enabling the programming, compilation, and execution of quantum code on quantum co-processors.")
set(CPACK_PACKAGE_VENDOR "ORNL")
set(CPACK_PACKAGE_CONTACT "mccaskeyaj@ornl.gov")
set(CPACK_PACKAGE_VERSION_MAJOR "${MAJOR_VERSION}")
set(CPACK_PACKAGE_VERSION_MINOR "${MINOR_VERSION}")
set(CPACK_PACKAGE_VERSION_PATCH "${PATCH_VERSION}")
set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}")

#dependencies for this service menu
if (${XACC_CPACK_RPM_PLATFORM} STREQUAL "fedora31")
set(CPACK_RPM_PACKAGE_REQUIRES "gcc-c++, gcc-gfortran, blas-devel, lapack-devel, git, python3-devel, libcurl-devel, openssl-devel")
elseif(${XACC_CPACK_RPM_PLATFORM} STREQUAL "focal")
endif()
 
include(CPack)
endif()