include_directories(${CMAKE_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR}/tpls/eigen)
add_xacc_test(RDM)
target_link_libraries(RDMTester xacc CppMicroServices xacc-fermion)