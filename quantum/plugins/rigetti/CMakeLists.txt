set(LIBRARY_NAME xacc-rigetti)
set(ANTLR_LIB ${CMAKE_SOURCE_DIR}/dist/libantlr4-runtime.so)
if (APPLE)
set(ANTLR_LIB ${CMAKE_SOURCE_DIR}/dist/libantlr4-runtime.dylib)
endif()
file(GLOB SRC
          accelerator/QVMAccelerator.cpp
          RigettiActivator.cpp
          compiler/QuilCompiler.cpp
          compiler/QuilToXACCListener.cpp
          compiler/generated/*.cpp)

usfunctiongetresourcesource(TARGET ${LIBRARY_NAME} OUT SRC)
usfunctiongeneratebundleinit(TARGET ${LIBRARY_NAME} OUT SRC)

add_library(${LIBRARY_NAME} SHARED ${SRC})

target_include_directories(${LIBRARY_NAME}
                           PUBLIC accelerator
                                  compiler
                                  compiler/generated
                                  ${CMAKE_SOURCE_DIR}/tpls/antlr/runtime/src)
target_link_libraries(${LIBRARY_NAME}
                      PUBLIC xacc
                             xacc-quantum-gate
                             ${ANTLR_LIB}
                             CppMicroServices)

set(_bundle_name xacc_rigetti)
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

if (PYTHON_INCLUDE_DIR AND PYTHON_LIBRARY)
add_subdirectory(qcs)
endif()