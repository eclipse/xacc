set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-strict-aliasing -O2 -g -pipe -Werror=format-security -Wp,-D_FORTIFY_SOURCE=2 -Wformat -fexceptions -fstack-protector-strong --param=ssp-buffer-size=4 -grecord-gcc-switches -m64 -mtune=generic -D_GNU_SOURCE -fPIC -fwrapv")
include_directories(${CMAKE_SOURCE_DIR}/tpls/pybind11/include)
include_directories(${PYTHON_INCLUDE_DIR})
add_library(pyxacc SHARED xacc-py.cpp)
SET_TARGET_PROPERTIES(pyxacc PROPERTIES PREFIX "")
target_link_libraries(pyxacc ${PYTHON_LIBRARY} xacc)
install(TARGETS pyxacc DESTINATION lib/python)


