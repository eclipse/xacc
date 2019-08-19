add_subdirectory(compiler)

include_directories(${PYTHON_INCLUDE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/tpls/pybind11/include)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-strict-aliasing -O2 -g -pipe -Werror=format-security -Wp,-D_FORTIFY_SOURCE=2 -Wformat -fexceptions --param=ssp-buffer-size=4 -grecord-gcc-switches -m64 -mtune=generic -D_GNU_SOURCE -fPIC -fwrapv")
if(APPLE)
   set(CMAKE_SHARED_LIBRARY_SUFFIX ".so")
endif(APPLE)

add_library(_pyxacc SHARED xacc-py.cpp)
set_target_properties(_pyxacc PROPERTIES PREFIX "")
target_link_libraries(_pyxacc PUBLIC xacc xacc-pauli xacc-fermion)

if(APPLE)
   set_target_properties(_pyxacc PROPERTIES INSTALL_RPATH "@loader_path/lib")
   set_target_properties(_pyxacc PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
else()
   set_target_properties(_pyxacc PROPERTIES INSTALL_RPATH "$ORIGIN/lib")
   set_target_properties(_pyxacc PROPERTIES LINK_FLAGS "-shared")
endif()

install(TARGETS _pyxacc DESTINATION ${CMAKE_INSTALL_PREFIX})
install(FILES xacc.py DESTINATION ${CMAKE_INSTALL_PREFIX})
install(DIRECTORY plugin_generator DESTINATION ${CMAKE_INSTALL_PREFIX})
install(DIRECTORY benchmark DESTINATION ${CMAKE_INSTALL_PREFIX})

file(GLOB PYDECORATORS ${CMAKE_CURRENT_SOURCE_DIR}/benchmark/vqe/*.py)
install(FILES ${PYDECORATORS} DESTINATION ${CMAKE_INSTALL_PREFIX}/py-plugins)
