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

set(LIBRARY_NAME xacc-py-plugin-loader)
get_filename_component(PYTHON_LIB_NAME ${Python_LIBRARIES} NAME)
configure_file(py_plugin_loader.in.cpp
               ${CMAKE_BINARY_DIR}/python/plugins/py_plugin_loader.cpp)
message(STATUS " PYTHON: ${PYTHON_LIB_NAME}")
file (GLOB SRC ${CMAKE_BINARY_DIR}/python/plugins/py_plugin_loader.cpp py_plugin_loader_activator.cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")
usFunctionGetResourceSource(TARGET ${LIBRARY_NAME} OUT SRC)
usFunctionGenerateBundleInit(TARGET ${LIBRARY_NAME} OUT SRC)

add_library(${LIBRARY_NAME} SHARED ${SRC})

set(_bundle_name xacc_py_plugin_loader)

set_target_properties(${LIBRARY_NAME} PROPERTIES
  COMPILE_DEFINITIONS US_BUNDLE_NAME=${_bundle_name}
  US_BUNDLE_NAME ${_bundle_name}
  )

usFunctionEmbedResources(TARGET ${LIBRARY_NAME}
  WORKING_DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}
  FILES
    manifest.json
  )

target_include_directories(${LIBRARY_NAME} PRIVATE . ${CMAKE_SOURCE_DIR}/tpls/pybind11/include ${CMAKE_BINARY_DIR})
target_link_libraries(${LIBRARY_NAME} PUBLIC xacc Python::Python)

if(APPLE)
   set_target_properties(${LIBRARY_NAME} PROPERTIES INSTALL_RPATH "@loader_path/../lib")
   set_target_properties(${LIBRARY_NAME} PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
else()
   set_target_properties(${LIBRARY_NAME} PROPERTIES INSTALL_RPATH "$ORIGIN/../lib")
   set_target_properties(${LIBRARY_NAME} PROPERTIES LINK_FLAGS "-shared")
endif()

install(TARGETS ${LIBRARY_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/plugins)

add_subdirectory(mitiq)
add_subdirectory(scikit-quant)