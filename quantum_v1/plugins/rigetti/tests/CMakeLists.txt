include_directories(${CMAKE_SOURCE_DIR}/quantum/plugins/rigetti/accelerator)
add_xacc_test(QVMAccelerator)
target_link_libraries(QVMAcceleratorTester xacc-quantum-gate CppMicroServices)

add_xacc_test(QuilCompiler)
target_link_libraries(QuilCompilerTester xacc-quantum-gate CppMicroServices)

add_xacc_test(QuilVisitor)
target_link_libraries(QuilVisitorTester xacc-quantum-gate)
