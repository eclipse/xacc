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
set(LIBRARY_NAME xacc-rigetti-qcs)
message(STATUS "BUILDING RIGETTI QCS")
file (GLOB_RECURSE HEADERS *.hpp)
file (GLOB SRC *.cpp)

usFunctionGetResourceSource(TARGET ${LIBRARY_NAME} OUT SRC)
usFunctionGenerateBundleInit(TARGET ${LIBRARY_NAME} OUT SRC)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")

add_library(${LIBRARY_NAME} SHARED ${SRC})

target_include_directories(${LIBRARY_NAME}
                           PUBLIC . ../accelerator ${UUID_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/tpls/rapidjson/include
                                  ${CMAKE_SOURCE_DIR}/tpls/eigen $ENV{HOME}/.zmq/include
                                  )
set (CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "$ENV{HOME}/.zmq")
find_package(cppzmq)

target_link_libraries(${LIBRARY_NAME}
                      PUBLIC xacc xacc-quantum-gate xacc-quantum-annealing cppzmq ${UUID_LIBRARIES})

set(_bundle_name xacc_rigetti_qcs)

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

if(APPLE)
   set_target_properties(${LIBRARY_NAME} PROPERTIES INSTALL_RPATH "@loader_path/../lib")
   set_target_properties(${LIBRARY_NAME} PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
else()
   set_target_properties(${LIBRARY_NAME} PROPERTIES INSTALL_RPATH "$ORIGIN/../lib")
   set_target_properties(${LIBRARY_NAME} PROPERTIES LINK_FLAGS "-shared")
endif()


install(TARGETS ${LIBRARY_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/plugins)

if(XACC_BUILD_TESTS)
  add_subdirectory(tests)
endif()