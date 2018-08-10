# *******************************************************************************
# Copyright (c) 2018 UT-Battelle, LLC.
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
set (PACKAGE_NAME "PYXACC Compiler")
set (PACKAGE_DESCIPTION "PYXACC Compiler Bundle")
set (LIBRARY_NAME xacc-py-compiler)

file (GLOB_RECURSE HEADERS *.hpp generated/*.hpp)
file (GLOB SRC *.cpp generated/*.cpp)

include_directories(${CMAKE_BINARY_DIR}/tpls/cppmicroservices/framework/include)
include_directories(${CMAKE_BINARY_DIR}/tpls/cppmicroservices/include)
include_directories(${CMAKE_SOURCE_DIR}/tpls/antlr/runtime/src)
include_directories(${CMAKE_SOURCE_DIR}/tpls/cppmicroservices/framework/include)

include_directories(generated)
include_directories(${CMAKE_SOURCE_DIR}/tpls/boost-cmake/boost/boost_1_64_0)
 
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
link_directories(${CMAKE_SOURCE_DIR}/dist)

# Set up dependencies to resources to track changes
usFunctionGetResourceSource(TARGET ${LIBRARY_NAME} OUT SRC)
# Generate bundle initialization code
usFunctionGenerateBundleInit(TARGET ${LIBRARY_NAME} OUT SRC)

add_library(${LIBRARY_NAME} SHARED ${SRC})

set(_bundle_name xacc_py_compiler)

set_target_properties(${LIBRARY_NAME} PROPERTIES
  # This is required for every bundle
  COMPILE_DEFINITIONS US_BUNDLE_NAME=${_bundle_name}
  # This is for convenience, used by other CMake functions
  US_BUNDLE_NAME ${_bundle_name}
  )

# Embed meta-data from a manifest.json file
usFunctionEmbedResources(TARGET ${LIBRARY_NAME}
  WORKING_DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}
  FILES
    manifest.json
  )

target_link_libraries(${LIBRARY_NAME} PUBLIC xacc xacc-quantum-gate xacc-quantum-aqc antlr4-runtime) 

if(APPLE)
   set_target_properties(${LIBRARY_NAME} PROPERTIES INSTALL_RPATH "@loader_path/../lib")
   set_target_properties(${LIBRARY_NAME} PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
else()
   set_target_properties(${LIBRARY_NAME} PROPERTIES INSTALL_RPATH "$ORIGIN/../lib")
   set_target_properties(${LIBRARY_NAME} PROPERTIES LINK_FLAGS "-shared")
endif()

install(TARGETS ${LIBRARY_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/plugins) 

if (${XACC_BUILD_TESTS})
   include_directories(${GTEST_INCLUDE_DIRS})
   add_subdirectory(tests)
endif()