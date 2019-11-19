# *******************************************************************************
# Copyright (c) 2019 UT-Battelle, LLC.
# All rights reserved. This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v1.0
# and Eclipse Distribution License v.10 which accompany this distribution.
# The Eclipse Public License is available at http://www.eclipse.org/legal/epl-v10.html
# and the Eclipse Distribution License is available at
# https://eclipse.org/org/documents/edl-v10.php
#
# Contributors:
#   Alexander J. McCaskey - initial API and implementation
# *******************************************************************************/
add_subdirectory(compiler)
add_subdirectory(plugins)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")
include_directories(.)
include_directories(${Python_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/tpls/pybind11/include)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-strict-aliasing -O2 -g -pipe -Werror=format-security -Wp,-D_FORTIFY_SOURCE=2 -Wformat -fexceptions --param=ssp-buffer-size=4 -grecord-gcc-switches -m64 -mtune=generic -D_GNU_SOURCE -fPIC -fwrapv")
if(APPLE)
   set(CMAKE_SHARED_LIBRARY_SUFFIX ".so")
endif(APPLE)

add_library(_pyxacc SHARED xacc-py.cpp py_heterogeneous_map.cpp py_ir.cpp py_accelerator.cpp py_compiler.cpp py_algorithm.cpp py_optimizer.cpp py_observable.cpp)
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

file(GLOB PYDECORATORS benchmark/vqe/*.py
                       plugins/aer/*.py
                       plugins/dwave/*.py
                       plugins/qiskit/*.py
                       plugins/observables/*.py
                       plugins/optimizers/*.py)
install(FILES ${PYDECORATORS} DESTINATION ${CMAKE_INSTALL_PREFIX}/py-plugins)
