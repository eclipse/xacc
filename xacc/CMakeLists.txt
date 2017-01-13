set (PACKAGE_NAME "XACC Runtime")
set (PACKAGE_DESCIPTION "The XACC Programming Framework")
set (LIBRARY_NAME xacc)

file (GLOB HEADERS program/*.hpp compiler/*.hpp accelerator/*.hpp utils/*.hpp)
file (GLOB SRC program/*.cpp compiler/*.cpp accelerator/*.cpp)

#add_library(${LIBRARY_NAME} SHARED ${SRC})

# Get the test files
file(GLOB test_files tests/*Tester.cpp)

# Add the tests
add_tests("${test_files}" "${CMAKE_CURRENT_SOURCE_DIR}/utils;${CMAKE_CURRENT_SOURCE_DIR}/compiler;${CMAKE_CURRENT_SOURCE_DIR}/program;${CMAKE_CURRENT_SOURCE_DIR}/accelerator" "${Boost_LIBRARIES}")


install(FILES ${HEADERS} DESTINATION include)
#install(TARGETS ${LIBRARY_NAME} DESTINATION lib)
