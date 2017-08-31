cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(xacc-core LANGUAGES CXX)

set(CMAKE_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 14)
message(STATUS "C++ version ${CXX_STANDARD} configured.")

# Modify our CMAKE_MODULE_PATH
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${XACC_ROOT}/cmake/Modules")
include(tests)

# We require MPI
find_package(MPI)
if (MPI_FOUND)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DXACC_HAS_MPI")

  message(STATUS "FOUND MPI: ${MPI_CXX_INCLUDE_PATH} AND ${MPI_CXX_LIBRARIES}")
  # We require Boost with MPI
  find_package(Boost COMPONENTS mpi system program_options filesystem chrono thread REQUIRED)
else()
  message(STATUS "MPI Not Found.")

  # We require Boost without MPI
  find_package(Boost COMPONENTS system program_options filesystem chrono thread REQUIRED)
endif()

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${XACC_ROOT}/tpls/spdlog)
include_directories(${CPPUS_INCLUDE_DIR})
include_directories(${MPI_CXX_INCLUDE_PATH})
include_directories("${CMAKE_SOURCE_DIR}")
include_directories("${CMAKE_SOURCE_DIR}/ir")
include_directories("${CMAKE_SOURCE_DIR}/compiler")
include_directories("${CMAKE_SOURCE_DIR}/program")
include_directories("${CMAKE_SOURCE_DIR}/accelerator")
include_directories("${CMAKE_SOURCE_DIR}/utils")
include_directories(${CMAKE_BINARY_DIR})

# Create a xacc config header file
configure_file("${XACC_ROOT}/cmake/xacc_config.hpp.in" "${CMAKE_BINARY_DIR}/xacc_config.hpp")
install(FILES "${CMAKE_BINARY_DIR}/xacc_config.hpp" DESTINATION include/xacc)

#set (PACKAGE_NAME "XACC Specification")
#set (PACKAGE_DESCIPTION "The XACC Programming Framework")
set (LIBRARY_NAME xacc)

file (GLOB HEADERS XACC.hpp ir/*.hpp program/*.hpp compiler/*.hpp accelerator/*.hpp utils/*.hpp)
file (GLOB SRC *.cpp utils/*.cpp)

link_directories(${CPPUS_LIBRARY_DIR})
add_library(${LIBRARY_NAME} SHARED ${SRC})
target_link_libraries(${LIBRARY_NAME} ${Boost_LIBRARIES} CppMicroServicesd)

# Get the test files
file(GLOB test_files utils/tests/*Tester.cpp)

# Add the tests
add_tests("${test_files}" "${CMAKE_CURRENT_SOURCE_DIR}/utils" "${Boost_LIBRARIES}")

install(FILES ${HEADERS} DESTINATION include/xacc)
install(TARGETS ${LIBRARY_NAME} DESTINATION lib)
