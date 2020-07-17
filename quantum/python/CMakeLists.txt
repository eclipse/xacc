include_directories(${Python_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/tpls/pybind11/include)

add_library(xacc-quantum-py SHARED xacc-quantum-py.cpp)

target_include_directories(xacc-quantum-py PUBLIC . ${CMAKE_SOURCE_DIR}/quantum/plugins/utils ${CMAKE_SOURCE_DIR}/python)

target_link_libraries(xacc-quantum-py PUBLIC xacc xacc-fermion xacc-pauli xacc-quantum-gate)

install(TARGETS xacc-quantum-py DESTINATION ${CMAKE_INSTALL_PREFIX})