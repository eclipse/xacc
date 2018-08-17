include_directories(${CMAKE_SOURCE_DIR}/xacc)
include_directories(${CMAKE_SOURCE_DIR}/quantum/gate)
include_directories(${CMAKE_SOURCE_DIR}/quantum/aqc)

include_directories(${CMAKE_BINARY_DIR})
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

include_directories(${PYTHON_INCLUDE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/tpls/pybind11/include)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-strict-aliasing -O2 -g -pipe -Werror=format-security -Wp,-D_FORTIFY_SOURCE=2 -Wformat -fexceptions --param=ssp-buffer-size=4 -grecord-gcc-switches -m64 -mtune=generic -D_GNU_SOURCE -fPIC -fwrapv")
if(APPLE)
   set(CMAKE_SHARED_LIBRARY_SUFFIX ".so")
endif(APPLE)

add_library(_pyxacc SHARED xacc-py.cpp)

set_target_properties(_pyxacc PROPERTIES PREFIX "")

target_link_libraries(_pyxacc PUBLIC CppMicroServices xacc)

if(APPLE)
   set_target_properties(_pyxacc PROPERTIES INSTALL_RPATH "@loader_path/lib")
   set_target_properties(_pyxacc PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
else()
   set_target_properties(_pyxacc PROPERTIES INSTALL_RPATH "$ORIGIN/lib")
   set_target_properties(_pyxacc PROPERTIES LINK_FLAGS "-shared")
endif()

install(TARGETS _pyxacc DESTINATION ${CMAKE_INSTALL_PREFIX})
install(FILES xacc.py DESTINATION ${CMAKE_INSTALL_PREFIX})

if (${XACC_HAS_ANTLR})
   add_subdirectory(compiler)
endif()