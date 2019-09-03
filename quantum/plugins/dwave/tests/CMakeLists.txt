##add_xacc_test(DWAccelerator)
#target_link_libraries(DWAcceleratorTester xacc-dwave)
include_directories(${CMAKE_SOURCE_DIR}/quantum/plugins/dwave/accelerator)

add_xacc_test(CMREmbedding)
target_link_libraries(CMREmbeddingTester xacc-dwave)