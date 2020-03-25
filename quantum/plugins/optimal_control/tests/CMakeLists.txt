add_xacc_test(GOAT)
target_link_libraries(GOATTester xacc-optimal-control)

add_xacc_test(OptimalControl)
target_link_libraries(OptimalControlTester xacc)
target_include_directories(OptimalControlTester PUBLIC ${CMAKE_SOURCE_DIR}/tpls/eigen)

add_xacc_test(GrapeOptim)
target_link_libraries(GrapeOptimTester xacc)
target_include_directories(GrapeOptimTester PUBLIC ${CMAKE_SOURCE_DIR}/tpls/eigen)