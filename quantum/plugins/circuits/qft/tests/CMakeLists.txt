
add_xacc_test(QFT)
target_link_libraries(QFTTester xacc xacc-quantum-gate)


add_xacc_test(InverseQFT)
target_link_libraries(InverseQFTTester xacc xacc-quantum-gate)