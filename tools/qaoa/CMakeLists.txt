add_subdirectory(examples)

add_executable(xacc-qaoa qaoa_from_file.cpp)
target_link_libraries(xacc-qaoa PRIVATE xacc xacc-quantum-gate xacc-pauli )

if(APPLE)
  set_target_properties(xacc-qaoa
                        PROPERTIES INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
else()
  set_target_properties(xacc-qaoa
                        PROPERTIES INSTALL_RPATH "$ORIGIN/../lib")
endif()

install (TARGETS xacc-qaoa DESTINATION bin)