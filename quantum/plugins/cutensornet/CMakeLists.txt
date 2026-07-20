set(LIBRARY_NAME xacc-cutensornet)

file(GLOB SRC *.cpp)

usfunctiongetresourcesource(TARGET ${LIBRARY_NAME} OUT SRC)
usfunctiongeneratebundleinit(TARGET ${LIBRARY_NAME} OUT SRC)

add_library(${LIBRARY_NAME} SHARED ${SRC})
if(CMAKE_MINOR_VERSION GREATER_EQUAL 24)
  set_property(TARGET ${LIBRARY_NAME} PROPERTY CUDA_ARCHITECTURES native)
else()
  if(CUDA_ARCH STREQUAL "")
    message(SEND_ERROR "Your version of cmake does not set CUDA ARCHITECTURES automatically. Pass it with the -DCUDA_ARCH flag.")
  else()
    set(CMAKE_CUDA_ARCHITECTURES ${CUDA_ARCH})
  endif()
endif()

target_include_directories(${LIBRARY_NAME}
                             PUBLIC .
                             ${CUDA_TOOLKIT_ROOT}/include)

target_link_directories(${LIBRARY_NAME}
                          PRIVATE
                          ${CUTENSOR_LIB}
                          ${CUDA_TOOLKIT_ROOT}/lib64)

target_link_libraries(${LIBRARY_NAME}
                        PUBLIC
                        xacc
                        xacc-quantum-gate
                        PRIVATE
                        cutensornet
                        cudart)

set(_bundle_name xacc_cutensornet)
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

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
  target_compile_definitions(${LIBRARY_NAME} PUBLIC WITH_OPENMP_)
  target_link_libraries(${LIBRARY_NAME} PUBLIC OpenMP::OpenMP_CXX)
endif()
                         
if(APPLE)
  set_target_properties(${LIBRARY_NAME}
                        PROPERTIES INSTALL_RPATH "@loader_path/../lib")
  set_target_properties(${LIBRARY_NAME}
                        PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
else()
  set_target_properties(${LIBRARY_NAME}
                        PROPERTIES INSTALL_RPATH "$ORIGIN/../lib")
set_target_properties(${LIBRARY_NAME} PROPERTIES
    BUILD_RPATH "${CUTENSOR_ROOT}/lib/11")              
  set_target_properties(${LIBRARY_NAME} PROPERTIES LINK_FLAGS "-shared")
endif()

if(XACC_BUILD_TESTS)
  add_subdirectory(tests)
endif()

message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")
install(TARGETS ${LIBRARY_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/plugins)