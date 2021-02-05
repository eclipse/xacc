set(LIBRARY_NAME xacc-verifier-qcec)

set(QCEC_INSTALL_ROOT "/usr/local" CACHE PATH "QCEC installation directory")
set(JSON_INSTALL_ROOT "/usr/local" CACHE PATH "nlohmann JSON installation directory")
list(APPEND CMAKE_PREFIX_PATH "${QCEC_INSTALL_ROOT}/lib/cmake/DDpackage/")
list(APPEND CMAKE_PREFIX_PATH "${QCEC_INSTALL_ROOT}/lib/cmake/qcec/")
list(APPEND CMAKE_PREFIX_PATH "${QCEC_INSTALL_ROOT}/lib/cmake/qfr/")
list(APPEND CMAKE_PREFIX_PATH "${JSON_INSTALL_ROOT}/lib/cmake/")

find_package(qcec REQUIRED CONFIG)
find_package(nlohmann_json 3.2.0 REQUIRED CONFIG)

usfunctiongetresourcesource(TARGET ${LIBRARY_NAME} OUT SRC)
usfunctiongeneratebundleinit(TARGET ${LIBRARY_NAME} OUT SRC)

add_library(${LIBRARY_NAME} SHARED ${SRC})

target_include_directories(${LIBRARY_NAME}
                             PUBLIC JKQ::qcec
                                    nlohmann_json::nlohmann_json
                             )
target_link_libraries(${LIBRARY_NAME}
                        PUBLIC xacc
                               xacc-quantum-gate
                               JKQ::qcec
                               nlohmann_json::nlohmann_json
                        )

target_sources(${LIBRARY_NAME} PRIVATE
  qcec.cpp)

set(_bundle_name xacc_verfier_qcec)
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
