
add_xacc_test(DirectedBoostGraph)
target_include_directories(DirectedBoostGraphTester PRIVATE ${CMAKE_SOURCE_DIR}/xacc/ir/graph-impl/boost)
target_link_libraries(DirectedBoostGraphTester xacc-graph Boost::graph)


add_xacc_test(UndirectedBoostGraph)
target_include_directories(UndirectedBoostGraphTester PRIVATE ${CMAKE_SOURCE_DIR}/xacc/ir/graph-impl/boost)
target_link_libraries(UndirectedBoostGraphTester xacc-graph Boost::graph)

