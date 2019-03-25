
add_xacc_test(Graph)
target_include_directories(GraphTester PRIVATE ${CMAKE_SOURCE_DIR}/quantum/utils)
target_link_libraries(GraphTester Boost::graph)
