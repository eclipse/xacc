add_xacc_test(GOAT)
target_link_libraries(GOATTester xacc-optimal-control)

add_xacc_test(OptimalControl)
target_link_libraries(OptimalControlTester xacc)