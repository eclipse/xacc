
cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(xacc-python LANGUAGES CXX)

set(CMAKE_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${XACC_ROOT}/cmake/Modules")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_INSTALL_PREFIX}/share/cppmicroservices3/cmake")
include(CppMicroServicesConfig)
include(tests)

find_package(PythonLibs REQUIRED)

set(BOOST_ROOT ${CMAKE_INSTALL_PREFIX})
find_package(Boost COMPONENTS system program_options filesystem chrono regex graph REQUIRED)

include_directories(${XACC_ROOT}/xacc)
include_directories(${XACC_ROOT}/quantum/gate)
include_directories(${XACC_ROOT}/quantum/aqc)

include_directories(${CMAKE_INSTALL_PREFIX}/include)
include_directories(${CMAKE_INSTALL_PREFIX}/include/cppmicroservices3)
include_directories(${XACC_BINARY_DIR})
include_directories(${XACC_ROOT}/xacc/ir)
include_directories(${XACC_ROOT}/xacc/compiler)
include_directories(${XACC_ROOT}/xacc/program)
include_directories(${XACC_ROOT}/xacc/accelerator)
include_directories(${XACC_ROOT}/xacc/utils)
include_directories(${XACC_ROOT}/tpls/rapidjson/include)
include_directories(${XACC_ROOT}/tpls/exprtk)
include_directories(${XACC_ROOT}/tpls/spdlog)
include_directories(${XACC_ROOT}/tpls/eigen)

include_directories(${XACC_ROOT}/quantum/gate/ir)
include_directories(${XACC_ROOT}/quantum/gate/ir/instructions)
include_directories(${XACC_ROOT}/quantum/gate/utils)

link_directories(${CMAKE_INSTALL_PREFIX}/lib)

include_directories(${PYTHON_INCLUDE_DIR})
include_directories(${XACC_ROOT}/tpls/pybind11/include)

link_directories(${XACC_BINARY_DIR}/xacc)
link_directories(${XACC_BINARY_DIR}/lib)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-strict-aliasing -O2 -g -pipe -Werror=format-security -Wp,-D_FORTIFY_SOURCE=2 -Wformat -fexceptions -fstack-protector-strong --param=ssp-buffer-size=4 -grecord-gcc-switches -m64 -mtune=generic -D_GNU_SOURCE -fPIC -fwrapv")
if(APPLE)
   set(CMAKE_SHARED_LIBRARY_SUFFIX ".so")
endif(APPLE)

add_library(pyxacc SHARED xacc-py.cpp)

set_target_properties(pyxacc PROPERTIES PREFIX "")
set(CppUsLib CppMicroServicesd)
find_library(cppus_has_d_suffix CppMicroServicesd)
if (NOT cppus_has_d_suffix)
	set(CppUsLib CppMicroServices)
endif()

target_link_libraries(pyxacc ${CppUsLib} xacc xacc-quantum-gate ${Boost_LIBRARIES} cpr curl) #xacc xacc-quantum-gate xacc-quantum-aqc CppMicroServices ${Boost_LIBRARIES} restclient-cpp) #cpprest ${OPENSSL_LIBRARIES})

if(APPLE)
   set_target_properties(pyxacc PROPERTIES INSTALL_RPATH "@loader_path/lib")
   set_target_properties(pyxacc PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
else()
   set_target_properties(pyxacc PROPERTIES INSTALL_RPATH "$ORIGIN/lib")
   set_target_properties(pyxacc PROPERTIES LINK_FLAGS "-shared")
endif()

install(TARGETS pyxacc DESTINATION ${CMAKE_INSTALL_PREFIX})
