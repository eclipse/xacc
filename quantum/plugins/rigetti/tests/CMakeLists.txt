add_xacc_test(QVMAccelerator)
target_link_libraries(QVMAcceleratorTester xacc-rigetti)

add_xacc_test(QuilCompiler)
target_link_libraries(QuilCompilerTester xacc-rigetti)

add_xacc_test(QuilVisitor)
target_link_libraries(QuilVisitorTester xacc-rigetti)
