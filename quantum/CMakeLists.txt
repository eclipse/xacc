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

include_directories(${XACC_INCLUDE_DIR})
include_directories(${XACC_ROOT}/tpls/rapidjson/include)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${XACC_ROOT}/tpls/spdlog)
include_directories(${XACC_ROOT}/tpls/eigen)
include_directories(${CPPUS_INCLUDE_DIR})
include_directories(${MPI_INCLUDE_PATH})
include_directories(${CMAKE_BINARY_DIR})

link_directories(${XACC_LIBRARY_DIR})
link_directories(${CPPUS_LIBRARY_DIR})

string (REPLACE " " ";" Boost_LIBRARIES "${XACC_Boost_LIBRARIES}")
add_subdirectory(gate)
add_subdirectory(aqc)
