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
set(LIBRARY_NAME xacc-ibm)

set(ANTLR_LIB ${CMAKE_SOURCE_DIR}/dist/libantlr4-runtime.so)
if (APPLE)
set(ANTLR_LIB ${CMAKE_SOURCE_DIR}/dist/libantlr4-runtime.dylib)
endif()
file(GLOB SRC
          accelerator/IBMAccelerator.cpp
          IBMActivator.cpp
          compiler/OQASMCompiler.cpp
          compiler/QObjectCompiler.cpp
          compiler/OQASMToXACCListener.cpp
          compiler/generated/*.cpp)

find_package(LAPACK)
if(LAPACK_FOUND)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DLAPACK_FOUND")
  message(
    STATUS "${BoldGreen}Building local IBM Accelerator simulator.${ColorReset}")
  list(APPEND SRC accelerator/LocalIBMAccelerator.cpp)
else()
  message(
    STATUS
      "${BoldYellow}Could not find Blas or Lapack, skipping local IBM simulator build.${ColorReset}"
    )
endif()

usfunctiongetresourcesource(TARGET ${LIBRARY_NAME} OUT SRC)
usfunctiongeneratebundleinit(TARGET ${LIBRARY_NAME} OUT SRC)

add_library(${LIBRARY_NAME} SHARED ${SRC})

if(LAPACK_FOUND)
  target_include_directories(
    ${LIBRARY_NAME}
    PUBLIC
      accelerator
      compiler
      compiler/generated
      accelerator/qiskit-terra/src/qasm-simulator-cpp/src
      accelerator/qiskit-terra/src/qasm-simulator-cpp/src/backends
      accelerator/qiskit-terra/src/qasm-simulator-cpp/src/engines
      accelerator/qiskit-terra/src/qasm-simulator-cpp/src/utilities
      accelerator/json
      ${CMAKE_SOURCE_DIR}/tpls/antlr/runtime/src
      ${CMAKE_SOURCE_DIR}/tpls/rapidjson/include)

  target_link_libraries(${LIBRARY_NAME}
                        PUBLIC xacc
                               xacc-quantum-gate
                               ${ANTLR_LIB}
                               CppMicroServices
                               blas
                               lapack)

else()

  target_include_directories(${LIBRARY_NAME}
                             PUBLIC accelerator
                                    compiler
                                    compiler/generated
                                    accelerator/json
                                    ${CMAKE_SOURCE_DIR}/tpls/antlr/runtime/src
                                    ${CMAKE_SOURCE_DIR}/tpls/rapidjson/include)
                                    #${CMAKE_SOURCE_DIR}/tpls/exprtk)
  target_link_libraries(${LIBRARY_NAME}
                        PUBLIC xacc
                               xacc-quantum-gate
                               ${ANTLR_LIB}
                               CppMicroServices)

endif()

set(_bundle_name xacc_ibm)
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
  add_subdirectory(tests)
  add_subdirectory(accelerator/json/tests)
endif()

install(TARGETS ${LIBRARY_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/plugins)
