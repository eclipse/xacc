set(LIBRARY_NAME xacc-ast-plugin)
add_library(${LIBRARY_NAME}
            SHARED
            FuzzyParsingExternalSemaSource.cpp
            KernelVisitor.cpp
            XACCASTConsumer.cpp)
target_include_directories(${LIBRARY_NAME}
                           PUBLIC .
                                ${CMAKE_BINARY_DIR}
                                  ${CLANG_INCLUDE_DIRS}
                                  ${LLVM_INCLUDE_DIRS})
target_link_libraries(${LIBRARY_NAME}
                      PUBLIC ${CLANG_LIBS} ${LLVM_LIBS} xacc CppMicroServices)
if(APPLE)
   set_target_properties(${LIBRARY_NAME} PROPERTIES INSTALL_RPATH "@loader_path")
   set_target_properties(${LIBRARY_NAME} PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
else()
   set_target_properties(${LIBRARY_NAME} PROPERTIES INSTALL_RPATH "$ORIGIN")
   set_target_properties(${LIBRARY_NAME} PROPERTIES LINK_FLAGS "-shared")
endif()
install(TARGETS ${LIBRARY_NAME} DESTINATION lib)

if(XACC_BUILD_TESTS)
  add_subdirectory(tests)
endif()

configure_file(xacc-driver.in.cpp
               ${CMAKE_BINARY_DIR}/tools/compiler/clang/xacc-driver.cpp)
add_executable(xacc-driver ${CMAKE_BINARY_DIR}/tools/compiler/clang/xacc-driver.cpp)
target_link_libraries(xacc-driver PRIVATE xacc-ast-plugin xacc)
install(PROGRAMS ${CMAKE_BINARY_DIR}/tools/compiler/clang/xacc-driver DESTINATION bin)
install(FILES xacc_runtime.hpp DESTINATION include/xacc)