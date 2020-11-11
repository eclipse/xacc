set(LIBRARY_NAME xacc-qsim)

file(GLOB SRC accelerator/*.cpp)

usfunctiongetresourcesource(TARGET ${LIBRARY_NAME} OUT SRC)
usfunctiongeneratebundleinit(TARGET ${LIBRARY_NAME} OUT SRC)

add_library(${LIBRARY_NAME} SHARED ${SRC})

target_include_directories(${LIBRARY_NAME}
                             PUBLIC .
                                    ./accelerator
                                    ${CMAKE_SOURCE_DIR}/tpls/qsim/lib)
target_link_libraries(${LIBRARY_NAME}
                        PUBLIC xacc
                               xacc-quantum-gate
                        )

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
  target_compile_definitions(${LIBRARY_NAME} PUBLIC _OPENMP)
  target_link_libraries(${LIBRARY_NAME} PUBLIC OpenMP::OpenMP_CXX)
endif()
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_SYSTEM_PROCESSOR MATCHES "(x86)|(X86)|(amd64)|(AMD64)")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
endif()

set(_bundle_name xacc_qsim)
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

