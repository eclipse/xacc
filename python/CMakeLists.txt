cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(xacc-python LANGUAGES CXX)

set(CMAKE_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)
message(STATUS "C++ version ${CXX_STANDARD} configured.")

# Make sure all build libraries have reference to their dependents
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

find_package(PythonLibs)
if(PythonLibs_FOUND)

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
include_directories(${XACC_ROOT}/tpls/exprtk)
include_directories(${XACC_ROOT}/tpls/pybind11/include)
include_directories(${XACC_ROOT}/tpls/spdlog)
include_directories(${PYTHON_INCLUDE_DIR})
link_directories(${XACC_LIBRARY_DIR})

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-strict-aliasing -O2 -g -pipe -Werror=format-security -Wp,-D_FORTIFY_SOURCE=2 -Wformat -fexceptions -fstack-protector-strong --param=ssp-buffer-size=4 -grecord-gcc-switches -m64 -mtune=generic -D_GNU_SOURCE -fPIC -fwrapv")
if(APPLE)
   set(CMAKE_SHARED_LIBRARY_SUFFIX ".so")
endif(APPLE)

add_library(pyxacc SHARED xacc-py.cpp)
set_target_properties(pyxacc PROPERTIES PREFIX "")
link_directories(${CPPUS_LIBRARY_DIR})
link_directories(${Boost_LIBRARY_DIR})
#find_library(cppus_has_d_suffix CppMicroServicesd)
#if (NOT cppus_has_d_suffix)
#	set(CppUsLib CppMicroServices)
#endif()
target_link_libraries(pyxacc ${PYTHON_LIBRARY} xacc xacc-quantum-gate xacc-quantum-aqc CppMicroServices ${Boost_LIBRARIES} cpprest ${OPENSSL_LIBRARIES})

if(APPLE)
   set_target_properties(pyxacc PROPERTIES INSTALL_RPATH "@loader_path/lib")
else()
   set_target_properties(pyxacc PROPERTIES INSTALL_RPATH "$ORIGIN/lib;$ORIGIN/plugins")
endif()

install(TARGETS pyxacc DESTINATION ${CMAKE_INSTALL_PREFIX})

endif()
