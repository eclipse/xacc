set(LIBRARY_NAME xacc-atos-qlm)
find_package(Python COMPONENTS Interpreter Development)
if(Python_FOUND)

  get_filename_component(PYTHON_LIB_NAME ${Python_LIBRARIES} NAME)
  configure_file(
    QlmAccelerator.in.cpp
    ${CMAKE_BINARY_DIR}/quantum/plugins/atos_qlm/accelerator/QlmAccelerator.cpp)
  file(
    GLOB SRC
    ${CMAKE_BINARY_DIR}/quantum/plugins/atos_qlm/accelerator/QlmAccelerator.cpp)

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

  set(_bundle_name xacc_qlm)
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
    set_target_properties(${LIBRARY_NAME} PROPERTIES LINK_FLAGS "-shared")
  endif()

  install(TARGETS ${LIBRARY_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/plugins)

  # Check if we have QAT (Atos QLM library) installed
  execute_process(COMMAND ${Python_EXECUTABLE} -c "import qat"
                  RESULT_VARIABLE QAT_EXISTS)
  if(QAT_EXISTS EQUAL "1")
    message(
      STATUS
        "${BoldYellow}QAT not found. Atos QLM Accelerator will not work.${ColorReset}"
    )
  else()
    # We always build the accelerator, but only test if the QAT library is
    # installed (on the QLM server).
    if(XACC_BUILD_TESTS)
      add_subdirectory(tests)
    endif()
  endif()
else()
  message(
    STATUS
      "${BoldYellow} Python Interpreter or Libs not found, skipping Atos Simulator Build.${ColorReset}"
  )
endif()
