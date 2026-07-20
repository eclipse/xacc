message(STATUS "${BoldGreen}Building Scipy Optimizer.${ColorReset}")
set(LIBRARY_NAME xacc-scipy-optimizer)

file(GLOB
     SRC
     scipy_optimizer.cpp)

usfunctiongetresourcesource(TARGET
                            ${LIBRARY_NAME}
                            OUT
                            SRC)
usfunctiongeneratebundleinit(TARGET
                             ${LIBRARY_NAME}
                             OUT
                             SRC)
#find_package(pybind11 REQUIRED)
find_package(Python COMPONENTS Interpreter Development)

add_library(${LIBRARY_NAME} SHARED ${SRC})

target_include_directories(${LIBRARY_NAME}
                           PUBLIC . ${CMAKE_SOURCE_DIR}/tpls/pybind11/include ${Python_INCLUDE_DIRS})

                           target_link_libraries(${LIBRARY_NAME} PUBLIC xacc Python::Python)

set(_bundle_name xacc_optimizer_scipy)
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
