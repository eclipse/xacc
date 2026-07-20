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
# Alexander J. McCaskey - initial API and implementation
# *******************************************************************************/
cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(xacc VERSION 1.0.0 LANGUAGES CXX)
set(CMAKE_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
message(STATUS "C++ version ${CXX_STANDARD} configured.")

if(NOT WIN32)
  string(ASCII 27 Esc)
  set(ColorReset "${Esc}[m")
  set(ColorBold "${Esc}[1m")
  set(Red "${Esc}[31m")
  set(Green "${Esc}[32m")
  set(Yellow "${Esc}[33m")
  set(Blue "${Esc}[34m")
  set(Magenta "${Esc}[35m")
  set(Cyan "${Esc}[36m")
  set(White "${Esc}[37m")
  set(BoldRed "${Esc}[1;31m")
  set(BoldGreen "${Esc}[1;32m")
  set(BoldYellow "${Esc}[1;33m")
  set(BoldBlue "${Esc}[1;34m")
  set(BoldMagenta "${Esc}[1;35m")
  set(BoldCyan "${Esc}[1;36m")
  set(BoldWhite "${Esc}[1;37m")
endif()

option(XACC_BUILD_TESTS "Build test programs" OFF)
option(XACC_BUILD_EXAMPLES "Build example programs" OFF)
option(XACC_ENSMALLEN_INCLUDE_DIR "Path to ensmallen.hpp for mlpack optimizer" "")
option(XACC_ARMADILLO_INCLUDE_DIR "Path to armadillo header for mlpack optimizer" "")
option(XACC_BUILD_SCIPY "Build Scipy optimizer plugin" OFF)
option(XACC_BUILD_ANNEALING "Build annealing libraries" OFF)
option(XACC_BUILD_GSL "Build GNU Scientific Library optimizer plugin" OFF)
option(XACC_BUILD_IONTRAP "Build the iontrap plugin" OFF)
option(XACC_BUILD_SCIKIT_QUANT "Build the Scikit-quant plugin" OFF)
option(XACC_BUILD_ATOS "Build the Atos QLM plugin" OFF)
option(XACC_BUILD_QFACTOR "Build the QFactor plugin" OFF)
option(XACC_BUILD_QSEARCH "Build the QSearch plugin" OFF)
option(XACC_BUILD_QRACK "Build the Qrack plugin" OFF)
option(XACC_BUILD_CUTENSORNET "Build the cuTensorNet plugin" OFF)


if(XACC_BUILD_ANNEALING)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DANNEALING_ENABLED")
else()
  message(STATUS "${BoldYellow}Skipping Annealing libraries by default. You can turn it on with -DXACC_BUILD_ANNEALING=ON${ColorReset}")
endif()

option(QIREE_MINIMAL_BUILD "Build only components for QIREE CI" OFF)
option(XACC_DEPS_EXTERNAL "Try find external dependencies (system installation) rather than tpls" OFF)

if(QIREE_MINIMAL_BUILD)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DQIREE_BUILD")
endif()

option(XACC_REMOTE_ACCELERATORS "Build remove (cloud) accelerators" ON)
if(NOT XACC_REMOTE_ACCELERATORS)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DREMOTE_DISABLED")
  message(STATUS "${BoldYellow}Skipping remote accelerators. Only local simulators are now enabled.${ColorReset}")
endif()

if(FROM_SETUP_PY AND NOT APPLE)
  message(STATUS "Running build from setup.py, linking to static libstdc++")
  set(CMAKE_SHARED_LINKER_FLAGS "-static-libstdc++" CACHE INTERNAL "" FORCE)
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
endif()

execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  OUTPUT_VARIABLE XACC_BUILD_VERSION ERROR_QUIET
  OUTPUT_STRIP_TRAILING_WHITESPACE)
message(STATUS "XACC GIT hash: ${XACC_BUILD_VERSION}")

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

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING
    "Choose the type of build, options are: Debug, Release, RelWithDebInfo, MinSizeRel"
    FORCE
  )
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")
include(format)
include(cxxFlags)
include(ExternalProject)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "$ENV{HOME}/.xacc" CACHE PATH "default install path" FORCE)
endif()

message(STATUS "${BoldGreen}Installing XACC to ${CMAKE_INSTALL_PREFIX}. Override with -DCMAKE_INSTALL_PREFIX=...${ColorReset}")
set(CMAKE_INSTALL_LIBDIR "${CMAKE_INSTALL_PREFIX}/lib")

macro(set_cache_variable VAR_NAME VAR_DESCRIPTION)
  set(${VAR_NAME} ${${VAR_NAME}} CACHE INTERNAL ${VAR_DESCRIPTION})
  message(STATUS "Set ${VAR_NAME} to ${${VAR_NAME}}.")
endmacro()

if(XACC_BUILD_TESTS)
  enable_testing()

  macro(add_xacc_test _TEST_NAME)
    add_executable(${_TEST_NAME}Tester ${_TEST_NAME}Tester.cpp)
    target_include_directories(${_TEST_NAME}Tester PRIVATE ${GTEST_INCLUDE_DIRS})
    add_test(NAME xacc_${_TEST_NAME}Tester COMMAND ${_TEST_NAME}Tester)
    target_link_libraries(${_TEST_NAME}Tester ${GTEST_LIBRARIES} xacc)
  endmacro()

  macro(add_xacc_mpi_test _TEST_NAME)
    add_executable(${_TEST_NAME}Tester ${_TEST_NAME}Tester.cpp)
    target_include_directories(${_TEST_NAME}Tester PRIVATE ${GTEST_INCLUDE_DIRS})
    add_test(NAME xacc_${_TEST_NAME}Tester COMMAND sh -c "mpiexec -np 2 ./${_TEST_NAME}Tester")
    target_link_libraries(${_TEST_NAME}Tester ${GTEST_LIBRARIES} xacc)
  endmacro()
endif()

add_compile_flags_if_supported(-Wno-attributes)
add_compile_flags_if_supported(-Wno-deprecated-declarations)
add_compile_flags_if_supported(-Wno-maybe-uninitialized)

# NOTE: to enable MPI, need to configure CMAKE with -DXACC_ENABLE_MPI=TRUE
# (some XACC Python binding is not compatible with MPI, hence not building with MPI as default)
# Check MPI status
# if MPI_CXX_COMPILER is not empty and XACC_ENABLE_MPI is set
# turn MPI_ENABLED on
# Update: we don't really need to give the path to the compiler
# because if MPI is found, MPI_CXX_COMPILER is populated
if(XACC_ENABLE_MPI)
  find_package(MPI)

  if(MPI_FOUND)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DMPI_ENABLED")
  else()
    message(STATUS "${BoldYellow}MPI is enabled but no MPI installation could be found.${ColorReset}")
  endif()
endif()

if(XACC_BUILD_CUTENSORNET)
  enable_language(CUDA)
  find_package(CUDAToolkit REQUIRED)

  if(CUDAToolkit_FOUND)
    message(STATUS "${Green}Found CUDA Toolkit.${ColorReset}")
  else()
    message(STATUS "${BoldYellow}CUDA Toolkit not found.${ColorReset}")
  endif()
endif()

if (XACC_DEPS_EXTERNAL)
  find_package(Boost COMPONENTS graph)
  find_package(cpr)
  if(cpr_FOUND)
    set(XACC_USE_EXTERNAL_CPR ON)
  endif()
  find_package(GTest)
endif()

include_directories(${CMAKE_BINARY_DIR}/tpls/cppmicroservices/include)
add_subdirectory(tpls)

IF(CMAKE_BUILD_TYPE MATCHES "Debug" OR CMAKE_BUILD_TYPE MATCHES "DEBUG")
  message(STATUS "${BoldYellow}Adding _XACC_DEBUG compiler flag${ColorReset}")
  add_compile_definitions(_XACC_DEBUG)
ENDIF()

# Find LAPACK (optional)
find_package(LAPACK)

# Dependencies:


if (XACC_DEPS_EXTERNAL)
  find_package(spdlog)
endif()
if(NOT spdlog_FOUND)
  # Create an interface target to the vendored spdlog
  set(SPDLOG_INCLUDE_DIR "${SPDLOG_TARGET_INC_DIR}")
  add_library(spdlog::spdlog INTERFACE IMPORTED)
  target_include_directories(spdlog::spdlog INTERFACE
    "${PROJECT_SOURCE_DIR}/tpls/spdlog/include"
  )
endif()

set(NLOHMANN_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/tpls/nlohmann/single_include)
if (XACC_DEPS_EXTERNAL)
  find_package(nlohmann_json)
  if (nlohmann_json_FOUND)
    get_target_property(NLOHMANN_TARGET_INC_DIR nlohmann_json INTERFACE_INCLUDE_DIRECTORIES)
    list(GET NLOHMANN_TARGET_INC_DIR 0 NLOHMANN_TARGET_INC_DIR)
    if (EXISTS ${NLOHMANN_TARGET_INC_DIR}/nlohmann/json.hpp)
      # Found a seemingly valid nlohmann installation, use it when XACC_DEPS_EXTERNAL is set
      message(STATUS "${BoldGreen}nlohmann: Found system installation at ${NLOHMANN_TARGET_INC_DIR}/nlohmann${ColorReset}")
      set(NLOHMANN_INCLUDE_DIR "${NLOHMANN_TARGET_INC_DIR}")
    endif()
  endif()
endif()

set(EIGEN_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/tpls/eigen)
if (XACC_DEPS_EXTERNAL)
  find_package(Eigen3)
  if (Eigen3_FOUND)
    message(STATUS "${BoldGreen}Eigen3: Found system installation (version ${EIGEN3_VERSION_STRING}) config at ${Eigen3_DIR}; include directory: ${EIGEN3_INCLUDE_DIR}.${ColorReset}")
    set(EIGEN_INCLUDE_DIR ${EIGEN3_INCLUDE_DIR})
  endif()
endif()

add_subdirectory(xacc)
add_subdirectory(quantum)

# qiree doesn't need python, so we can ignore this and everything else that is python related
if(NOT QIREE_MINIMAL_BUILD)

find_package(Python COMPONENTS Interpreter Development)

  if(Python_FOUND)
    if(${Python_VERSION} VERSION_GREATER_EQUAL 3.0.0)
      message(STATUS "${BoldGreen}Found Python version ${Python_VERSION}. Building XACC Python API with ${Python_INCLUDE_DIRS}${ColorReset}")
      add_subdirectory(python)
      add_subdirectory(${CMAKE_SOURCE_DIR}/quantum/python)

      # Double check we have module ipopo installed, contributes pelix
      execute_process(COMMAND ${Python_EXECUTABLE} -c "import pelix" RESULT_VARIABLE PELIX_EXISTS)

      if(PELIX_EXISTS EQUAL "1")
        # if not, check we have pip
        execute_process(COMMAND ${Python_EXECUTABLE} -c "import pip" RESULT_VARIABLE PIP_EXISTS)

        if(PIP_EXISTS EQUAL "0")
          # we have pip, son just install ipopo
          message(STATUS "${BoldGreen}Installing Pelix OSGi framework.${ColorReset}")
          # iPOPO 3.0 only works with python 3.10+
          if(${Python_VERSION} VERSION_GREATER_EQUAL 3.10.0)
            execute_process(COMMAND ${Python_EXECUTABLE} -m pip install ipopo)
          else()
            execute_process(COMMAND ${Python_EXECUTABLE} -m pip install ipopo==1.0.2)
          endif()
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

endif()

# Version info
set(MAJOR_VERSION 1)
set(MINOR_VERSION 0)
set(PATCH_VERSION 0)

# Install the config file for external projects to use
# This is cool, users just add find_package(XACC REQUIRED) to
# their CMake builds, and pass -DXACC_DIR=/path/to/install/xacc
# and CMake loads include paths, libs, etc
configure_file("${CMAKE_SOURCE_DIR}/cmake/xacc-config.cmake.in" "${CMAKE_BINARY_DIR}/xacc-config.cmake" @ONLY)
configure_file("${CMAKE_SOURCE_DIR}/cmake/xacc-config-version.cmake.in" "${CMAKE_BINARY_DIR}/xacc-config-version.cmake" @ONLY)
install(FILES "${CMAKE_BINARY_DIR}/xacc-config.cmake" DESTINATION .)
install(FILES "${CMAKE_BINARY_DIR}/xacc-config-version.cmake" DESTINATION .)
install(FILES "${CMAKE_SOURCE_DIR}/cmake/Modules/format.cmake" DESTINATION share/xacc/)
install(FILES "${CMAKE_SOURCE_DIR}/tpls/mpark-variant/variant.hpp" DESTINATION include/xacc/)
install(FILES "${CMAKE_SOURCE_DIR}/tpls/taocpp/operators.hpp" DESTINATION include/xacc/)
if(NOT XACC_DEPS_EXTERNAL OR NOT nlohmann_json_FOUND)
  install(FILES "${NLOHMANN_INCLUDE_DIR}/nlohmann/json.hpp" DESTINATION include/xacc/)
endif()

if(NOT XACC_DEPS_EXTERNAL OR NOT spdlog_FOUND)
  install(DIRECTORY "${SPDLOG_INCLUDE_DIR}" DESTINATION include/spdlog/)
endif()
install(DIRECTORY "${CMAKE_SOURCE_DIR}/tpls/exprtk" DESTINATION include)
if(NOT XACC_DEPS_EXTERNAL OR NOT Eigen3_FOUND)
  install(DIRECTORY "${EIGEN_INCLUDE_DIR}" DESTINATION include)
endif()
install(DIRECTORY "${CMAKE_SOURCE_DIR}/tpls/rapidjson" DESTINATION include)

if(NOT QIREE_MINIMAL_BUILD)
  install(DIRECTORY "${CMAKE_SOURCE_DIR}/tpls/pybind11" DESTINATION include)
endif()

if(XACC_CPACK_DEB_PLATFORM)
  message(STATUS "CPack DEB Build Enabled.")
  set(CPACK_SET_DESTDIR "on")
  set(CPACK_PACKAGING_INSTALL_PREFIX "/tmp")
  set(CPACK_GENERATOR "DEB")

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

  # dependencies for this service menu
  if(${XACC_CPACK_DEB_PLATFORM} STREQUAL "bionic")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "gcc-8, g++-8, libblas-dev, liblapack-dev, python3, python3-pip, libpython3-dev, libcurl4-openssl-dev, libssl-dev")
  elseif(${XACC_CPACK_DEB_PLATFORM} STREQUAL "focal")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "gcc-10, g++-10, libblas-dev, liblapack-dev, python3, python3-pip, libpython3-dev, libcurl4-openssl-dev, libssl-dev")
  endif()

  set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
  set(CPACK_DEBIAN_PACKAGE_SECTION "base")
  set(CPACK_DEBIAN_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR})

  include(CPack)
endif()

if(XACC_CPACK_RPM_PLATFORM)
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

  # dependencies for this service menu
  if(${XACC_CPACK_RPM_PLATFORM} STREQUAL "fedora31")
    set(CPACK_RPM_PACKAGE_REQUIRES "gcc-c++, gcc-gfortran, blas-devel, lapack-devel, git, python3-devel, libcurl-devel, openssl-devel")
  elseif(${XACC_CPACK_RPM_PLATFORM} STREQUAL "focal")
  endif()

  include(CPack)
endif()
