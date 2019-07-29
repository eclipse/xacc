
include_directories(${CMAKE_SOURCE_DIR}/compiler)
add_xacc_test(ScaffoldCompiler)
target_link_libraries(ScaffoldCompilerTester xacc-scaffold)