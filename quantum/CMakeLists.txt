cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(xacc-quantum LANGUAGES CXX)

set(CMAKE_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 14)
message(STATUS "C++ version ${CXX_STANDARD} configured.")

set(CppMicroServices_DIR "${CPPUS_LIBRARY_DIR}/../share/cppmicroservices3/cmake")

# Modify our CMAKE_MODULE_PATH
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${XACC_ROOT}/cmake/Modules")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CppMicroServices_DIR}")
include(tests)
include(CppMicroServicesConfig)

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

include_directories(${XACC_INCLUDE_DIR})
include_directories(${XACC_ROOT}/tpls/rapidjson/include)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${XACC_ROOT}/tpls/spdlog)
include_directories(${XACC_ROOT}/tpls/eigen)
include_directories(${CPPUS_INCLUDE_DIR})
include_directories(${MPI_CXX_INCLUDE_PATH})
include_directories(${CMAKE_BINARY_DIR})

link_directories(${XACC_LIBRARY_DIR})
link_directories(${CPPUS_LIBRARY_DIR})

add_subdirectory(gate)
add_subdirectory(aqc)
