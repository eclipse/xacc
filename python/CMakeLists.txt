
find_package(PythonLibs REQUIRED)

set(CppMicroServices_DIR "${CMAKE_INSTALL_PREFIX}/share/cppmicroservices3/cmake")

# Modify our CMAKE_MODULE_PATH
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CppMicroServices_DIR}")
include(CppMicroServicesConfig)

set(BOOST_ROOT ${CMAKE_INSTALL_PREFIX})
find_package(Boost COMPONENTS system program_options filesystem chrono regex graph REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/xacc)
include_directories(${CMAKE_SOURCE_DIR}/quantum/gate)
include_directories(${CMAKE_SOURCE_DIR}/quantum/aqc)

include_directories(${CMAKE_INSTALL_PREFIX}/include)
include_directories(${CMAKE_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR}/xacc)
include_directories(${CMAKE_SOURCE_DIR}/xacc/ir)
include_directories(${CMAKE_SOURCE_DIR}/xacc/compiler)
include_directories(${CMAKE_SOURCE_DIR}/xacc/program)
include_directories(${CMAKE_SOURCE_DIR}/xacc/accelerator)
include_directories(${CMAKE_SOURCE_DIR}/xacc/utils)
include_directories(${CMAKE_SOURCE_DIR}/tpls/rapidjson/include)
include_directories(${CMAKE_SOURCE_DIR}/tpls/exprtk)
include_directories(${CMAKE_SOURCE_DIR}/tpls/spdlog)
include_directories(${CMAKE_SOURCE_DIR}/tpls/eigen)

include_directories(${CMAKE_SOURCE_DIR}/quantum/gate/ir)
include_directories(${CMAKE_SOURCE_DIR}/quantum/gate/ir/instructions)
include_directories(${CMAKE_SOURCE_DIR}/quantum/gate/utils)

link_directories(${CMAKE_INSTALL_PREFIX}/lib)

include_directories(${PYTHON_INCLUDE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/tpls/pybind11/include)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-strict-aliasing -O2 -g -pipe -Werror=format-security -Wp,-D_FORTIFY_SOURCE=2 -Wformat -fexceptions -fstack-protector-strong --param=ssp-buffer-size=4 -grecord-gcc-switches -m64 -mtune=generic -D_GNU_SOURCE -fPIC -fwrapv")
if(APPLE)
   set(CMAKE_SHARED_LIBRARY_SUFFIX ".so")
endif(APPLE)

add_library(pyxacc SHARED xacc-py.cpp)
add_dependencies(pyxacc xacc xacc-quantum-gate xacc-quantum-aqc)

set_target_properties(pyxacc PROPERTIES PREFIX "")
set(CppUsLib CppMicroServicesd)
find_library(cppus_has_d_suffix CppMicroServicesd)
if (NOT cppus_has_d_suffix)
	set(CppUsLib CppMicroServices)
endif()

target_link_libraries(pyxacc ${CppUsLib} xacc xacc-quantum-gate ${Boost_LIBRARIES} cpr) #xacc xacc-quantum-gate xacc-quantum-aqc CppMicroServices ${Boost_LIBRARIES} restclient-cpp) #cpprest ${OPENSSL_LIBRARIES})

if(APPLE)
   set_target_properties(pyxacc PROPERTIES INSTALL_RPATH "@loader_path/lib")
   set_target_properties(pyxacc PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
else()
   set_target_properties(pyxacc PROPERTIES INSTALL_RPATH "$ORIGIN/lib")
   set_target_properties(pyxacc PROPERTIES LINK_FLAGS "-shared")
endif()

install(TARGETS pyxacc DESTINATION ${CMAKE_INSTALL_PREFIX})
