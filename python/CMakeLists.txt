cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(xacc-python LANGUAGES CXX)

set(CMAKE_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)
message(STATUS "C++ version ${CXX_STANDARD} configured.")

# Make sure all build libraries have reference to their dependents
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib;@loader_path@/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(CMAKE_MACOSX_RPATH ON)
endif()

#find_package(PythonLibs)
#if(PythonLibs_FOUND)

set(CppMicroServices_DIR "${CPPUS_LIBRARY_DIR}/../share/cppmicroservices3/cmake")

# Modify our CMAKE_MODULE_PATH
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${XACC_ROOT}/cmake/Modules")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CppMicroServices_DIR}")
include(tests)
include(CppMicroServicesConfig)
include(tests)
include(CppMicroServicesConfig)

find_package(Boost COMPONENTS system program_options filesystem chrono thread REQUIRED)

include_directories(${XACC_INCLUDE_DIR}/xacc)
include_directories(${XACC_INCLUDE_DIR}/quantum/gate)
include_directories(${XACC_INCLUDE_DIR}/quantum/aqc)
include_directories(${CPPUS_INCLUDE_DIR})

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${XACC_ROOT}/tpls/rapidjson/include)
include_directories(${XACC_ROOT}/tpls/eigen)
include_directories(${XACC_ROOT}/tpls/pybind11/include)
include_directories(${XACC_ROOT}/tpls/spdlog)
include_directories(${XACC_PYTHON_INCLUDE_DIR})
link_directories(${XACC_LIBRARY_DIR})

add_subdirectory(pybind11)

pybind11_add_module(pyxacc xacc-py.cpp)

set(CppUsLib CppMicroServicesd)
link_directories(${CPPUS_LIBRARY_DIR})
link_directories(${Boost_LIBRARY_DIR})
find_library(cppus_has_d_suffix CppMicroServicesd)
if (NOT cppus_has_d_suffix)
	set(CppUsLib CppMicroServices)
endif()
target_link_libraries(pyxacc PRIVATE xacc xacc-quantum-gate xacc-quantum-aqc ${CppUsLib} ${Boost_LIBRARIES} cpprest ${OPENSSL_LIBRARIES})

set_target_properties(pyxacc PROPERTIES INSTALL_RPATH "@loader_path/lib")

if (FROM_SETUP_PY)
install(TARGETS pyxacc DESTINATION ${CMAKE_INSTALL_PREFIX})
else()
install(TARGETS pyxacc DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/python)
endif()

#endif()
