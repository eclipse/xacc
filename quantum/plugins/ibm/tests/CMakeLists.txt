include_directories(${CMAKE_SOURCE_DIR}/quantum/plugins/ibm/accelerator)
include_directories(${CMAKE_SOURCE_DIR}/quantum/plugins/ibm/compiler)
include_directories(${CMAKE_SOURCE_DIR}/quantum/gate/utils)
include_directories(${CMAKE_BINARY_DIR})

add_xacc_test(LocalIBMAccelerator)
target_link_libraries(LocalIBMAcceleratorTester xacc-quantum-gate)

add_xacc_test(OQASMCompiler)
target_link_libraries(OQASMCompilerTester xacc-quantum-gate)

add_xacc_test(QObjectCompiler)
target_link_libraries(QObjectCompilerTester CppMicroServices)

#add_xacc_test(IBMAccelerator)
#target_link_libraries(IBMAcceleratorTester xacc-ibm)