include_directories(${CMAKE_SOURCE_DIR}/quantum/gate/ir)
include_directories(${CMAKE_SOURCE_DIR}/quantum/gate/ir/instructions)
include_directories(${CMAKE_SOURCE_DIR}/tpls/exprtk)
include_directories(${CMAKE_SOURCE_DIR}/quantum/gate/ir/algorithms)
include_directories(${CMAKE_SOURCE_DIR}/quantum/utils)
include_directories(${CMAKE_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR}/quantum/gate/utils)
include_directories(${CMAKE_SOURCE_DIR}/tpls/taocpp)
include_directories(${CMAKE_SOURCE_DIR}/tpls/eigen)
include_directories(${CMAKE_SOURCE_DIR}/quantum/gate/observable)

add_xacc_test(PauliOperator)

target_link_libraries(PauliOperatorTester CppMicroServices xacc-pauli)