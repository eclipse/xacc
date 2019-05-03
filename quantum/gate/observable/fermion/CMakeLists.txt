set (PACKAGE_NAME "FermionOperator Parser")
set (LIBRARY_NAME xacc-fermion)

file (GLOB_RECURSE HEADERS *.hpp generated/*.hpp)
file (GLOB SRC *.cpp generated/*.cpp)

add_library(${LIBRARY_NAME} SHARED ${SRC})
target_include_directories(${LIBRARY_NAME}
                            PUBLIC
                            .
                            ${CMAKE_SOURCE_DIR}/tpls/antlr/runtime/src
                            ${CMAKE_CURRENT_SOURCE_DIR}
                            ${CMAKE_CURRENT_SOURCE_DIR}/generated
                            ${CMAKE_SOURCE_DIR}/tpls/taocpp
                            #${CMAKE_SOURCE_DIR}/tpls/eigen
                            ${CMAKE_BINARY_DIR})

add_dependencies(${LIBRARY_NAME} antlr4_shared)

if(APPLE)
   target_link_libraries(${LIBRARY_NAME} PUBLIC xacc PRIVATE CppMicroServices ${CMAKE_SOURCE_DIR}/dist/libantlr4-runtime.dylib)
   set_target_properties(${LIBRARY_NAME} PROPERTIES INSTALL_RPATH "@loader_path")
   set_target_properties(${LIBRARY_NAME} PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
else()
   target_link_libraries(${LIBRARY_NAME} PUBLIC xacc PRIVATE CppMicroServices ${CMAKE_SOURCE_DIR}/dist/libantlr4-runtime.so)
   set_target_properties(${LIBRARY_NAME} PROPERTIES INSTALL_RPATH "$ORIGIN")
   set_target_properties(${LIBRARY_NAME} PROPERTIES LINK_FLAGS "-shared")
endif()

if(XACC_BUILD_TESTS)
  add_subdirectory(tests)
endif()

file (GLOB HEADERS *.hpp)

install(FILES ${HEADERS} DESTINATION include/quantum/gate)
install(TARGETS ${LIBRARY_NAME} DESTINATION lib)
