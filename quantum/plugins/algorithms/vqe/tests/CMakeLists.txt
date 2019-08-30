include_directories(${CMAKE_BINARY_DIR})
add_xacc_test(VQE)
target_link_libraries(VQETester xacc xacc-pauli)