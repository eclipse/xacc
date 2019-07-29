configure_file(KernelVisitorTester.in.cpp
               ${CMAKE_BINARY_DIR}/tools/compiler/clang/tests/KernelVisitorTester.cpp)
include_directories(${CMAKE_SOURCE_DIR}/tools/compiler/clang)

#add_xacc_test(KernelVisitor)
#target_link_libraries(KernelVisitorTester xacc xacc-ast-plugin ${CLANG_LIBS})