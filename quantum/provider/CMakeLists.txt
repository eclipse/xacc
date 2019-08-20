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

file (GLOB HEADERS utils/*.hpp)
install(FILES ${HEADERS} DESTINATION include/quantum/utils)

set(LIBRARY_NAME xacc-quantum-irprovider)

file(GLOB QHEADERS *.hpp)
file(GLOB SRC
          *.cpp)


# Set up dependencies to resources to track changes
usFunctionGetResourceSource(TARGET ${LIBRARY_NAME} OUT SRC)

# Generate bundle initialization code
usFunctionGenerateBundleInit(TARGET ${LIBRARY_NAME} OUT SRC)

add_library(${LIBRARY_NAME} SHARED ${SRC})
add_dependencies(${LIBRARY_NAME} antlr4_shared)

target_include_directories(${LIBRARY_NAME}
                           PRIVATE .)

set(_bundle_name xacc_quantum_irprovider)
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

target_link_libraries(${LIBRARY_NAME} PUBLIC xacc PRIVATE xacc-quantum-gate xacc-quantum-annealing)
target_compile_features(${LIBRARY_NAME}
                        PUBLIC
                        cxx_std_14
                        cxx_alias_templates)

if(APPLE)
  set_target_properties(${LIBRARY_NAME}
                        PROPERTIES INSTALL_RPATH "@loader_path/../lib")
  set_target_properties(${LIBRARY_NAME}
                        PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
else()
  set_target_properties(${LIBRARY_NAME} PROPERTIES INSTALL_RPATH "$ORIGIN/../lib")
  set_target_properties(${LIBRARY_NAME} PROPERTIES LINK_FLAGS "-shared")
endif()

#install(FILES ${QHEADERS} DESTINATION include/quantum/)
install(TARGETS ${LIBRARY_NAME} DESTINATION plugins)
