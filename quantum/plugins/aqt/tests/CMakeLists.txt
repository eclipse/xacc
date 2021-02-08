add_xacc_test(AqtAccelerator)
target_link_libraries(AqtAcceleratorTester xacc xacc-quantum-gate)
target_compile_definitions(AqtAcceleratorTester PRIVATE RESOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/resources")
