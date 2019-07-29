add_xacc_test(XACCAPI xacc)
target_include_directories(XACCAPITester PRIVATE ${CMAKE_BINARY_DIR})
add_xacc_test(CLIParser xacc)
add_xacc_test(Algorithm xacc)