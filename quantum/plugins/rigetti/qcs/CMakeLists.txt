find_package(Python COMPONENTS Interpreter Development)
if(Python_FOUND)
  set(LIBRARY_NAME xacc-rigetti-qcs)

  file(GLOB_RECURSE HEADERS *.hpp)
  file(GLOB SRC QCSAccelerator.cpp)

  usfunctiongetresourcesource(TARGET ${LIBRARY_NAME} OUT SRC)
  usfunctiongeneratebundleinit(TARGET ${LIBRARY_NAME} OUT SRC)

  set(CMAKE_CXX_FLAGS
      "${CMAKE_CXX_FLAGS} -Wno-attributes -Wno-deprecated-declarations")

  add_library(${LIBRARY_NAME} SHARED ${SRC})

  target_include_directories(
    ${LIBRARY_NAME}
    PUBLIC .. ${XACC_ROOT}/include ${CMAKE_SOURCE_DIR}/tpls/eigen
           ${CMAKE_SOURCE_DIR}/tpls/pybind11/include ${Python_INCLUDE_DIRS})

  target_link_libraries(${LIBRARY_NAME} PUBLIC xacc xacc-quantum-gate cpr
                                               Python::Python)

  set(_bundle_name xacc_rigetti_qcs)
  set_target_properties(
    ${LIBRARY_NAME}
    PROPERTIES # This is required for every bundle
               COMPILE_DEFINITIONS US_BUNDLE_NAME=${_bundle_name}
               # This is for convenience, used by other CMake functions
               US_BUNDLE_NAME ${_bundle_name})

  # Embed meta-data from a manifest.json file
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

  execute_process(COMMAND ${Python_EXECUTABLE} -c "import rpcq, requests"
                  RESULT_VARIABLE RPCQ_EXISTS)
  if(RPCQ_EXISTS EQUAL "1")
    message(
      STATUS
        "${BoldYellow}rpcq or requests not found. QCS Accelerator is still built, but tests will not available until you install them and re-run cmake.${ColorReset}"
    )
  else()
    if(XACC_BUILD_TESTS)
      message(
        STATUS
          "${BoldYellow}Including QCS Accelerator integration tests.${ColorReset}"
      )
      add_subdirectory(tests)
    endif()
  endif()

  install(TARGETS ${LIBRARY_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/plugins)

endif()
