include_directories(${CMAKE_BINARY_DIR})
add_xacc_test(VQEEnergy)
target_link_libraries(VQEEnergyTester xacc CppMicroServices xacc-pauli)