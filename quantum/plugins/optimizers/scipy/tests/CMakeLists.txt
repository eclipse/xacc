include_directories(${CMAKE_SOURCE_DIR}/tpls/pybind11/include ${Python_INCLUDE_DIRS})

add_xacc_test(ScipyOptimizer)
target_link_libraries(ScipyOptimizerTester xacc Python::Python)