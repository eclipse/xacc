add_xacc_test(QlmAccelerator)
target_link_libraries(QlmAcceleratorTester xacc xacc-quantum-gate)

add_xacc_test(QlmNoisyAccelerator)
target_link_libraries(QlmNoisyAcceleratorTester xacc xacc-quantum-gate)

add_xacc_test(QlmNoiseModel)
target_link_libraries(QlmNoiseModelTester xacc xacc-quantum-gate)