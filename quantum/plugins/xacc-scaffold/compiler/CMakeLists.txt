
# PLUGIN STUFF HERE

set(LIBRARY_NAME xacc-scaffold)

configure_file(ScaffCompiler.in.cpp ${CMAKE_BINARY_DIR}/compiler/ScaffCompiler.cpp)
set(SRC SCAFFCompilerActivator.cpp ${CMAKE_BINARY_DIR}/compiler/ScaffCompiler.cpp)

usfunctiongetresourcesource(TARGET ${LIBRARY_NAME} OUT SRC)
usfunctiongeneratebundleinit(TARGET ${LIBRARY_NAME} OUT SRC)

add_library(${LIBRARY_NAME} SHARED ${SRC})

target_include_directories(${LIBRARY_NAME} PUBLIC . ${XACC_INCLUDE_DIRS})
target_link_libraries(${LIBRARY_NAME} PUBLIC xacc CppMicroServices)

set(_bundle_name xacc_scaffold)
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



install(TARGETS ${LIBRARY_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/plugins)

add_subdirectory(tests)
#add_subdirectory(clang)