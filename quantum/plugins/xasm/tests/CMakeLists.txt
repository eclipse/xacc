
include_directories(${CMAKE_SOURCE_DIR}/tools/compiler)
add_xacc_test(XASMCompiler)
target_link_libraries(XASMCompilerTester xacc CppMicroServices xacc-quantum-gate)