include_directories(${CMAKE_SOURCE_DIR}/quantum/plugins/ibm/accelerator)
include_directories(${CMAKE_SOURCE_DIR}/quantum/plugins/ibm/compiler)
include_directories(${CMAKE_SOURCE_DIR}/quantum/gate/utils)

add_xacc_test(OQASMCompiler)
target_link_libraries(OQASMCompilerTester xacc-ibm)
add_xacc_test(QObjectCompiler)
target_link_libraries(QObjectCompilerTester xacc-ibm)

#add_xacc_test(IBMAccelerator)
#target_link_libraries(IBMAcceleratorTester xacc-ibm)