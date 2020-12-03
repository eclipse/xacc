set(LIBRARY_NAME xacc-qubit-tapering)
file(GLOB SRC qubit_tapering.cpp)
usfunctiongetresourcesource(TARGET ${LIBRARY_NAME} OUT SRC)
usfunctiongeneratebundleinit(TARGET ${LIBRARY_NAME} OUT SRC)
add_library(${LIBRARY_NAME} SHARED ${SRC})

# L-BFGS++ will require Eigen, XACC provides it
target_include_directories(${LIBRARY_NAME} PUBLIC . 
                                    ${XACC_ROOT}/include/eigen ${XACC_ROOT}/include/armadillo)

# _bundle_name must be == manifest.json bundle.symbolic_name !!!
set(_bundle_name xacc_qubit_tapering)
set_target_properties(${LIBRARY_NAME}
                      PROPERTIES COMPILE_DEFINITIONS
                                 US_BUNDLE_NAME=${_bundle_name}
                                 US_BUNDLE_NAME ${_bundle_name})
usfunctionembedresources(TARGET ${LIBRARY_NAME} 
                         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                         FILES manifest.json)

# Link library with XACC
target_link_libraries(${LIBRARY_NAME} PUBLIC xacc xacc-quantum-gate)
# and with LAPACK
if(LAPACK_FOUND)
  target_link_libraries(${LIBRARY_NAME} PRIVATE ${LAPACK_LIBRARIES})
endif()

# Configure RPATH
if(APPLE)
  set_target_properties(${LIBRARY_NAME} PROPERTIES INSTALL_RPATH 
                            "${XACC_ROOT}/lib")
  set_target_properties(${LIBRARY_NAME} PROPERTIES LINK_FLAGS 
                            "-undefined dynamic_lookup")
else()
  set_target_properties(${LIBRARY_NAME} PROPERTIES INSTALL_RPATH 
                        "${XACC_ROOT}/lib")
  set_target_properties(${LIBRARY_NAME} PROPERTIES LINK_FLAGS "-shared")
endif()

if(XACC_BUILD_TESTS)
 add_subdirectory(tests)
endif()

# Install to Plugins directory
install(TARGETS ${LIBRARY_NAME} DESTINATION plugins)