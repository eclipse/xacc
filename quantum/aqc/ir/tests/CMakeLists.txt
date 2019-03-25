include_directories(${CMAKE_SOURCE_DIR}/quantum/aqc/ir)
include_directories(${CMAKE_SOURCE_DIR}/quantum/aqc/compiler)
include_directories(${CMAKE_SOURCE_DIR}/quantum/utils)
include_directories(${CMAKE_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR}/tpls/exprtk)

add_xacc_test(DWFunction)
add_xacc_test(DWQMI)

target_link_libraries(DWFunctionTester Boost::graph)
