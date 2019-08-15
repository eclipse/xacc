##add_xacc_test(DWAccelerator)
#target_link_libraries(DWAcceleratorTester xacc-dwave)
include_directories(${CMAKE_SOURCE_DIR}/quantum/plugins/dwave/accelerator)

add_xacc_test(DWQMICompiler)
target_link_libraries(DWQMICompilerTester xacc xacc-quantum-aqc CppMicroServices)
add_xacc_test(AnnealScheduleGenerator)
target_link_libraries(AnnealScheduleGeneratorTester xacc-quantum-aqc)

