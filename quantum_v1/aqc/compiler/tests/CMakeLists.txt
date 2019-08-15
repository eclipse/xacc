include_directories(${CMAKE_SOURCE_DIR}/quantum/aqc/ir)
include_directories(${CMAKE_SOURCE_DIR}/quantum/aqc/compiler)
include_directories(${CMAKE_SOURCE_DIR}/quantum/aqc/compiler/default)
include_directories(${CMAKE_SOURCE_DIR}/quantum/utils)
include_directories(${CMAKE_BINARY_DIR})

add_xacc_test(Embedding)
add_xacc_test(TrivialEmbeddingAlgorithm)
target_link_libraries(TrivialEmbeddingAlgorithmTester xacc-quantum-aqc Boost::graph)