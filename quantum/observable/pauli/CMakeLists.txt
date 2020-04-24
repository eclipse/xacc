# *******************************************************************************
# Copyright (c) 2019 UT-Battelle, LLC.
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
set (PACKAGE_NAME "PauliOperator Parser")
set (LIBRARY_NAME xacc-pauli)

file (GLOB_RECURSE HEADERS *.hpp generated/*.hpp)
file (GLOB SRC *.cpp generated/*.cpp)

# Set up dependencies to resources to track changes
usFunctionGetResourceSource(TARGET ${LIBRARY_NAME} OUT SRC)

# Generate bundle initialization code
usFunctionGenerateBundleInit(TARGET ${LIBRARY_NAME} OUT SRC)

add_library(${LIBRARY_NAME} SHARED ${SRC})
target_include_directories(${LIBRARY_NAME}
                            PUBLIC
                            . ${CMAKE_SOURCE_DIR}/tpls/armadillo
                            ${CMAKE_SOURCE_DIR}/tpls/antlr/runtime/src
                            ${CMAKE_CURRENT_SOURCE_DIR}
                            ${CMAKE_CURRENT_SOURCE_DIR}/generated
                            ${CMAKE_SOURCE_DIR}/tpls/taocpp
                            ${CMAKE_SOURCE_DIR}/tpls/eigen
                            ${CMAKE_BINARY_DIR})

add_dependencies(${LIBRARY_NAME} antlr4_shared)


set(_bundle_name xacc_pauli)
set_target_properties(${LIBRARY_NAME}
                      PROPERTIES # This is required for every bundle
                                 COMPILE_DEFINITIONS
                                 US_BUNDLE_NAME=${_bundle_name}
                                 # This is for convenience, used by other CMake
                                 # functions
                                 US_BUNDLE_NAME ${_bundle_name})

# Embed meta-data from a manifest.json file
usFunctionEmbedResources(TARGET
                         ${LIBRARY_NAME}
                         WORKING_DIRECTORY
                         ${CMAKE_CURRENT_SOURCE_DIR}
                         FILES
                         manifest.json)

if(APPLE)
   target_link_libraries(${LIBRARY_NAME} PUBLIC xacc ${CMAKE_SOURCE_DIR}/dist/libantlr4-runtime.dylib)
   set_target_properties(${LIBRARY_NAME} PROPERTIES INSTALL_RPATH "@loader_path")
   set_target_properties(${LIBRARY_NAME} PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
else()
   target_link_libraries(${LIBRARY_NAME} PUBLIC xacc PRIVATE ${CMAKE_SOURCE_DIR}/dist/libantlr4-runtime.so)
   set_target_properties(${LIBRARY_NAME} PROPERTIES INSTALL_RPATH "$ORIGIN")
   set_target_properties(${LIBRARY_NAME} PROPERTIES LINK_FLAGS "-shared")
endif()

if(XACC_BUILD_TESTS)
  add_subdirectory(tests)
endif()

file (GLOB HEADERS *.hpp)

install(FILES ${HEADERS} DESTINATION include/quantum/gate)
install(TARGETS ${LIBRARY_NAME} DESTINATION lib)
