set(LIBRARY_NAME xacc-qrack)

file(GLOB SRC
          *.cpp
          accelerator/*.cpp)

usfunctiongetresourcesource(TARGET ${LIBRARY_NAME} OUT SRC)
usfunctiongeneratebundleinit(TARGET ${LIBRARY_NAME} OUT SRC)

add_library(${LIBRARY_NAME} SHARED ${SRC})

# Need to get include dir if this is installed in non-standard path
get_filename_component(QRACK_LIB_PARENT_DIR ${QRACK_LIBRARY} DIRECTORY)
get_filename_component(QRACK_ROOT_DIR ${QRACK_LIB_PARENT_DIR} DIRECTORY)

target_include_directories(${LIBRARY_NAME}
                             PUBLIC . ${QRACK_ROOT_DIR}/include
                                    ./accelerator
                                    ./accelerator/src/include)

find_package(OpenCL)
if (OpenCL_FOUND)
    target_link_libraries(${LIBRARY_NAME}
                            PUBLIC xacc
                                   xacc-quantum-gate
                                   ${QRACK_LIBRARY}
                                   OpenCL
                        )
else()
    target_link_libraries(${LIBRARY_NAME}
                            PUBLIC xacc
                                   xacc-quantum-gate
                                   ${QRACK_LIBRARY}
                        )
endif()

set(_bundle_name xacc_qrack)

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

