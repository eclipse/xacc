include_directories(${CMAKE_SOURCE_DIR}/quantum/gate/utils)
include_directories(${CMAKE_SOURCE_DIR}/quantum/gate/ir)
include_directories(${CMAKE_SOURCE_DIR}/quantum/gate/ir/instructions)
include_directories(${CMAKE_SOURCE_DIR}/quantum/utils)
include_directories(${CMAKE_SOURCE_DIR}/quantum/gate/accelerator)

include_directories(${CMAKE_BINARY_DIR})

#add_xacc_test(ReadoutErrorAcceleratorBufferPostprocessor)
add_xacc_test(ImprovedSamplingDecorator)
add_xacc_test(RichExtrapDecorator)

target_link_libraries(RichExtrapDecoratorTester xacc-quantum-gate)
target_link_libraries(ImprovedSamplingDecoratorTester xacc-quantum-gate)