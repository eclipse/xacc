set(LIBRARY_NAME xacc-ibm)

file(GLOB SRC
          accelerator/IBMAccelerator.cpp
          IBMActivator.cpp
          compiler/OQASMCompiler.cpp
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
      accelerator/qiskit-terra/src/qasm-simulator-cpp/src/third-party/headers
      ${CMAKE_SOURCE_DIR}/tpls/antlr/runtime/src)

  target_link_libraries(${LIBRARY_NAME}
                        PUBLIC xacc
                               xacc-quantum-gate
                               ${CMAKE_SOURCE_DIR}/dist/libantlr4-runtime.so
                               CppMicroServices
                               blas
                               lapack)

else()

  target_include_directories(${LIBRARY_NAME}
                             PUBLIC accelerator
                                    compiler
                                    compiler/generated
                                    ${CMAKE_SOURCE_DIR}/tpls/antlr/runtime/src)
  target_link_libraries(${LIBRARY_NAME}
                        PUBLIC xacc
                               xacc-quantum-gate
                               ${CMAKE_SOURCE_DIR}/dist/libantlr4-runtime.so
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
endif()

install(TARGETS ${LIBRARY_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/plugins)
