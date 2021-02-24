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
set(LIBRARY_NAME xacc-circuits)

file(GLOB SRC hwe/hwe.cpp
              range/range.cpp
              exp/exp.cpp
              qft/QFT.cpp
              qft/InverseQFT.cpp
              uccsd/uccsd.cpp
              ucc1/ucc1.cpp
              ucc3/ucc3.cpp
              aswap/aswap.cpp
              qfast/qfast.cpp
              kak/kak.cpp
              GeneratorsActivator.cpp)

usfunctiongetresourcesource(TARGET ${LIBRARY_NAME} OUT SRC)
usfunctiongeneratebundleinit(TARGET ${LIBRARY_NAME} OUT SRC)

add_library(${LIBRARY_NAME} SHARED ${SRC})

target_include_directories(
  ${LIBRARY_NAME}
  PUBLIC . range exp hwe qft uccsd ucc1 ucc3 aswap qfast kak
  ${CMAKE_SOURCE_DIR}/quantum/plugins/utils)

target_link_libraries(${LIBRARY_NAME} PUBLIC xacc PRIVATE xacc-quantum-gate xacc-pauli xacc-fermion)

set(_bundle_name xacc_generators)
set_target_properties(${LIBRARY_NAME}
                      PROPERTIES COMPILE_DEFINITIONS
                                 US_BUNDLE_NAME=${_bundle_name}
                                 US_BUNDLE_NAME
                                 ${_bundle_name})

usfunctionembedresources(TARGET
                         ${LIBRARY_NAME}
                         WORKING_DIRECTORY
                         ${CMAKE_CURRENT_SOURCE_DIR}
                         FILES
                         manifest.json)


if(APPLE)
  set_target_properties(${LIBRARY_NAME}
                        PROPERTIES INSTALL_RPATH "@loader_path/../lib")
  set_target_properties(${LIBRARY_NAME}
                        PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
else()
  set_target_properties(${LIBRARY_NAME}
                        PROPERTIES INSTALL_RPATH "$ORIGIN/../lib")
  set_target_properties(${LIBRARY_NAME} PROPERTIES LINK_FLAGS "-shared")
endif()

if(XACC_BUILD_TESTS)
  add_subdirectory(hwe/tests)
  add_subdirectory(exp/tests)
  add_subdirectory(range/tests)
  add_subdirectory(qft/tests)
  add_subdirectory(uccsd/tests)
  add_subdirectory(ucc1/tests)
  add_subdirectory(ucc3/tests)
  add_subdirectory(aswap/tests)
  add_subdirectory(qfast/tests)
  add_subdirectory(kak/tests)
endif()

install(TARGETS ${LIBRARY_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/plugins)

add_subdirectory(py-qsearch)
add_subdirectory(py-qfactor)
