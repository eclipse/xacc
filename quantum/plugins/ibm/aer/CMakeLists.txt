set(LIBRARY_NAME xacc-aer)

add_subdirectory(py-aer)

if(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "AMD64")
  if (NOT CMAKE_OSX_ARCHITECTURES STREQUAL "arm64")
    set(AER_SIMD_SOURCE_FILE src/simulators/statevector/qv_avx2.cpp)
  endif()
endif()

file(GLOB SRC
          accelerator/aer_accelerator.cpp
          ${AER_SIMD_SOURCE_FILE})

usfunctiongetresourcesource(TARGET ${LIBRARY_NAME} OUT SRC)
usfunctiongeneratebundleinit(TARGET ${LIBRARY_NAME} OUT SRC)

add_library(${LIBRARY_NAME} SHARED ${SRC})

target_include_directories(${LIBRARY_NAME}
                             PUBLIC .
                                    accelerator
                                    src
                                    ../common
                                    ${CMAKE_SOURCE_DIR}/tpls)
target_link_libraries(${LIBRARY_NAME}
                        PUBLIC xacc
                               xacc-quantum-gate
                               xacc-py-aer-adapter
                        )
if(CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "x86_64" OR CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "AMD64")
  if(APPLE OR UNIX)
    if (NOT CMAKE_OSX_ARCHITECTURES STREQUAL "arm64")
      set_source_files_properties(${AER_SIMD_SOURCE_FILE} PROPERTIES COMPILE_FLAGS "-march=native -mfma -mavx2")
    endif()
  endif()    
endif()

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
  target_link_libraries(${LIBRARY_NAME} PUBLIC OpenMP::OpenMP_CXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

set(_bundle_name xacc_aer)
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
  if(LAPACK_FOUND)
   target_link_libraries(${LIBRARY_NAME} PRIVATE ${LAPACK_LIBRARIES})
  else()
    message(STATUS "LAPACK NOT FOUND. Aer plugin may not work.")
  endif()
  set_target_properties(${LIBRARY_NAME}
                        PROPERTIES INSTALL_RPATH "$ORIGIN/../lib")
  set_target_properties(${LIBRARY_NAME} PROPERTIES LINK_FLAGS "-shared")
endif()

if(XACC_BUILD_TESTS)
  add_subdirectory(tests)
endif()

install(TARGETS ${LIBRARY_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/plugins)

