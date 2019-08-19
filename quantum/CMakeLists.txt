add_subdirectory(gate)
add_subdirectory(provider)
add_subdirectory(observable)
add_subdirectory(plugins)

if (XACC_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()