include_directories(${CMAKE_SOURCE_DIR}/xacc/optimizer/nlopt-optimizers)
include_directories(${CMAKE_SOURCE_DIR}/xacc/optimizer)
include_directories(${CMAKE_BUILD_DIR}/xacc/optimizer/nlopt-optimizers/nlopt/src/api)
include_directories(${CMAKE_BINARY_DIR})

add_xacc_test(NLOptimizer)
target_link_libraries(NLOptimizerTester nlopt CppMicroServices)