

add_xacc_test(AnnealingProgram)
add_xacc_test(DWQMI)
target_link_libraries(DWQMITester xacc-quantum-annealing)
target_link_libraries(AnnealingProgramTester xacc-quantum-annealing)
