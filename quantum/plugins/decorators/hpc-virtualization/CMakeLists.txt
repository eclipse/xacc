# ******************************************************************************
# *
# Copyright (c) 2019 UT-Battelle, LLC. All rights reserved. This program and the
# accompanying materials are made available under the terms of the Eclipse
# Public License v1.0 and Eclipse Distribution License v.10 which accompany this
# distribution. The Eclipse Public License is available at
# http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution License
# is available at https://eclipse.org/org/documents/edl-v10.php
#
# Contributors: Alexander J. McCaskey - initial API and implementation
# ******************************************************************************
# */
if(MPI_FOUND)
  set(LIBRARY_NAME xacc-hpc-virt-decorator)

  file(GLOB_RECURSE HEADERS *.hpp)
  file(GLOB SRC hpc_virt_decorator.cpp MPIProxy.cpp)

  # Set up dependencies to resources to track changes
  usfunctiongetresourcesource(TARGET
                              ${LIBRARY_NAME}
                              OUT
                              SRC)
  # Generate bundle initialization code
  usfunctiongeneratebundleinit(TARGET
                               ${LIBRARY_NAME}
                               OUT
                               SRC)

  add_library(${LIBRARY_NAME} SHARED ${SRC})

  set(_bundle_name xacc_hpc_virt_decorator)

  set_target_properties(${LIBRARY_NAME}
                        PROPERTIES # This is required for every bundle
                                   COMPILE_DEFINITIONS
                                   US_BUNDLE_NAME=${_bundle_name}
                                   # This is for convenience, used by other
                                   # CMake functions
                                   US_BUNDLE_NAME ${_bundle_name})

  # Embed meta-data from a manifest.json file
  usfunctionembedresources(TARGET
                           ${LIBRARY_NAME}
                           WORKING_DIRECTORY
                           ${CMAKE_CURRENT_SOURCE_DIR}
                           FILES
                           manifest.json)

  target_include_directories(${LIBRARY_NAME} PUBLIC . ${MPI_CXX_HEADER_DIR})
  target_link_libraries(${LIBRARY_NAME} PUBLIC xacc)

  if(APPLE)
    set_target_properties(${LIBRARY_NAME}
                          PROPERTIES INSTALL_RPATH
                                     "@loader_path/../lib;@loader_path")
    set_target_properties(${LIBRARY_NAME}
                          PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
  else()
    set_target_properties(${LIBRARY_NAME}
                          PROPERTIES INSTALL_RPATH "$ORIGIN/../lib:$ORIGIN")
    set_target_properties(${LIBRARY_NAME} PROPERTIES LINK_FLAGS "-shared")
  endif()

  install(TARGETS ${LIBRARY_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/plugins)

  # Gather tests
  if(XACC_BUILD_TESTS)
    add_subdirectory(tests)
  endif()
endif()
