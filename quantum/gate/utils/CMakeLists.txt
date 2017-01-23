set (PACKAGE_NAME "Quantum XACC Utils Runtime")
set (PACKAGE_DESCIPTION "Quantum XACC Common Utilities")

file (GLOB HEADERS *.hpp)

install(FILES ${HEADERS} DESTINATION include)

# Gather tests
#file (GLOB test_files tests/*.cpp)
#add_tests("${test_files}" "${CMAKE_CURRENT_SOURCE_DIR}" "${LIBRARY_NAME}")

