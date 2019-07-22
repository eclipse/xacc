
include_directories(${CMAKE_SOURCE_DIR}/tools/compiler)
add_xacc_test(XACCLangCompiler)
target_link_libraries(XACCLangCompilerTester xacc CppMicroservices xacc-quantum-gate)