set (PACKAGE_NAME "Scaffold XACC Runtime")
set (PACKAGE_DESCIPTION "Scaffold XACC Programming Framework")
set (LIBRARY_NAME xacc-scaffold)

file (GLOB HEADERS *.hpp)
file (GLOB SRC *.cpp)

add_library(${LIBRARY_NAME} SHARED ${SRC})

install(FILES ${HEADERS} DESTINATION include)
install(TARGETS ${LIBRARY_NAME} DESTINATION lib)

# Gather tests
file (GLOB test_files tests/*.cpp)
add_tests("${test_files}" "${CMAKE_CURRENT_SOURCE_DIR}" "${LIBRARY_NAME}")

