find_package(OpenMP REQUIRED)

set(LIBRARY_NAME dmsimcpu-accelerator)
file(GLOB SRC accelerator/DmsimCpuAccelerator.cpp)
usfunctiongetresourcesource(TARGET ${LIBRARY_NAME} OUT SRC)
usfunctiongeneratebundleinit(TARGET ${LIBRARY_NAME} OUT SRC)
add_library(${LIBRARY_NAME} SHARED ${SRC})

target_include_directories(${LIBRARY_NAME}
                             PUBLIC .
                                    ./accelerator
                                    ./accelerator/src/include
                                    ${CMAKE_SOURCE_DIR}/tpls/eigen
                                    /home/cades/xacc_dev/DM-Sim/src)
            


set(_bundle_name dmsimcpu_accelerator)
set_target_properties(${LIBRARY_NAME}
                      PROPERTIES COMPILE_DEFINITIONS
                                 US_BUNDLE_NAME=${_bundle_name}
                                 US_BUNDLE_NAME ${_bundle_name})
usfunctionembedresources(TARGET ${LIBRARY_NAME} 
                         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                         FILES manifest.json)

# Link library with XACC
target_link_libraries(${LIBRARY_NAME} PUBLIC xacc xacc-quantum-gate OpenMP::OpenMP_CXX)

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