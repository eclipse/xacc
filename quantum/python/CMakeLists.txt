set(LIBRARY_NAME xacc-quantum-py)

include_directories(${Python_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/tpls/pybind11/include)

add_library(${LIBRARY_NAME} SHARED xacc-quantum-py.cpp)

target_include_directories(${LIBRARY_NAME} PUBLIC . ${CMAKE_SOURCE_DIR}/quantum/gate/utils ${CMAKE_SOURCE_DIR}/quantum/plugins/utils ${CMAKE_SOURCE_DIR}/python)

target_link_libraries(${LIBRARY_NAME} PUBLIC xacc xacc-fermion xacc-pauli xacc-quantum-gate)

if(APPLE)
   set_target_properties(${LIBRARY_NAME} PROPERTIES INSTALL_RPATH "@loader_path/lib")
   set_target_properties(${LIBRARY_NAME} PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
else()
   set_target_properties(${LIBRARY_NAME} PROPERTIES INSTALL_RPATH "$ORIGIN/lib")
   set_target_properties(${LIBRARY_NAME} PROPERTIES LINK_FLAGS "-shared")
endif()

install(TARGETS ${LIBRARY_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX})