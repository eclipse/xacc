
# PLUGIN STUFF HERE
if (EXISTS ${XACC_ROOT}/include/qcor/QCORPragmaHandler.hpp)
set(LIBRARY_NAME xacc-scaffold-pragmas)

set(SRC ScaffoldPragmaActivator.cpp)

usfunctiongetresourcesource(TARGET ${LIBRARY_NAME} OUT SRC)
usfunctiongeneratebundleinit(TARGET ${LIBRARY_NAME} OUT SRC)

add_library(${LIBRARY_NAME} SHARED ${SRC})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/Modules)
find_package(Clang 9.0.0 REQUIRED)
target_include_directories(${LIBRARY_NAME} PUBLIC . ${XACC_INCLUDE_DIRS} ${XACC_ROOT}/include/qcor ${CLANG_INCLUDE_DIRS}
                                  ${LLVM_INCLUDE_DIRS})

target_link_libraries(${LIBRARY_NAME} PUBLIC xacc CppMicroServices qcor ${CLANG_LIBS} ${LLVM_LIBS})

set(_bundle_name xacc_scaffold_pragmas)
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
endif()