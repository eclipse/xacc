# *******************************************************************************
# Copyright (c) 2019 UT-Battelle, LLC. All rights reserved. This program and the
# accompanying materials are made available under the terms of the Eclipse
# Public License v1.0 and Eclipse Distribution License v.10 which accompany this
# distribution. The Eclipse Public License is available at
# http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution License
# is available at https://eclipse.org/org/documents/edl-v10.php
#
# Contributors: Thien Nguyen - initial API and implementation
# *******************************************************************************/
set(LIBRARY_NAME xacc-py-qfactor)

find_package(Python COMPONENTS Interpreter Development)
if(Python_FOUND)

  get_filename_component(PYTHON_LIB_NAME ${Python_LIBRARIES} NAME)

  configure_file(
    py_qfactor.in.cpp
    ${CMAKE_BINARY_DIR}/quantum/plugins/circuits/py-qfactor/py_qfactor.cpp)
  file(GLOB SRC
       ${CMAKE_BINARY_DIR}/quantum/plugins/circuits/py-qfactor/py_qfactor.cpp)

  set(CMAKE_CXX_FLAGS
      "${CMAKE_CXX_FLAGS} -fno-strict-aliasing -O2 -g -pipe -Werror=format-security -Wp,-D_FORTIFY_SOURCE=2 -Wformat -fexceptions --param=ssp-buffer-size=4 -grecord-gcc-switches -m64 -mtune=native -D_GNU_SOURCE -fPIC -fwrapv"
  )
  usfunctiongetresourcesource(TARGET ${LIBRARY_NAME} OUT SRC)
  usfunctiongeneratebundleinit(TARGET ${LIBRARY_NAME} OUT SRC)

  add_library(${LIBRARY_NAME} SHARED ${SRC})

  target_include_directories(
    ${LIBRARY_NAME} PUBLIC . ${Python_INCLUDE_DIRS}
                           ${CMAKE_SOURCE_DIR}/tpls/pybind11/include)

  target_link_libraries(${LIBRARY_NAME} PUBLIC xacc xacc-quantum-gate
                                               Python::Python)

  set(_bundle_name xacc_py_qfactor)
  set_target_properties(
    ${LIBRARY_NAME}
    PROPERTIES COMPILE_DEFINITIONS US_BUNDLE_NAME=${_bundle_name}
               US_BUNDLE_NAME ${_bundle_name})

  usfunctionembedresources(TARGET ${LIBRARY_NAME} WORKING_DIRECTORY
                           ${CMAKE_CURRENT_SOURCE_DIR} FILES manifest.json)

  if(APPLE)
    set_target_properties(${LIBRARY_NAME} PROPERTIES INSTALL_RPATH
                                                     "@loader_path/../lib")
    set_target_properties(${LIBRARY_NAME}
                          PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
  else()
    set_target_properties(${LIBRARY_NAME} PROPERTIES INSTALL_RPATH
                                                     "$ORIGIN/../lib")
    set_target_properties(${LIBRARY_NAME} PROPERTIES LINK_FLAGS
                                                     "-rdynamic -shared")
  endif()
  

  install(TARGETS ${LIBRARY_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/plugins)

  execute_process(COMMAND ${Python_EXECUTABLE} -c "import qfactor"
                  RESULT_VARIABLE QFACTOR_EXISTS)
  if(QFACTOR_EXISTS EQUAL "1")
    message(
      STATUS
        "${BoldYellow}QFactor not found. py-qfactor Synthesis not available until you install it.${ColorReset}"
    )
  else()
    if(XACC_BUILD_TESTS)
      add_subdirectory(tests)
    endif()
  endif()
else()
  message(STATUS "${BoldYellow} Python Interpreter or Libs not found, skipping QFactor Plugin Build.${ColorReset}")
endif()

