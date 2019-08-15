include_directories(${CMAKE_BINARY_DIR})
add_xacc_test(RBMGenerator)
target_link_libraries(RBMGeneratorTester xacc-rbm-ir)