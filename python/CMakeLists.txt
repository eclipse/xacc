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

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-strict-aliasing -O2 -g -pipe -Werror=format-security -Wp,-D_FORTIFY_SOURCE=2 -Wformat -fexceptions --param=ssp-buffer-size=4 -grecord-gcc-switches -m64 -mtune=native -D_GNU_SOURCE -fPIC -fwrapv")
if(APPLE)
   set(CMAKE_SHARED_LIBRARY_SUFFIX ".so")
endif(APPLE)

add_library(_pyxacc SHARED xacc-py.cpp 
                            py_heterogeneous_map.cpp 
                            py_ir.cpp py_accelerator.cpp 
                            py_compiler.cpp py_algorithm.cpp 
                            py_optimizer.cpp py_observable.cpp  py_graph.cpp
                            ${CMAKE_SOURCE_DIR}/quantum/python/xacc-quantum-py.cpp)
target_include_directories(_pyxacc PUBLIC .
                                          ${Python_INCLUDE_DIRS}
                                          ${CMAKE_SOURCE_DIR}/tpls/pybind11/include
                                          ${CMAKE_SOURCE_DIR}/quantum/python
                                          ${CMAKE_SOURCE_DIR}/quantum/gate/utils
                                          ${CMAKE_SOURCE_DIR}/quantum/gate/ir
                                          ${CMAKE_SOURCE_DIR}/quantum/plugins/utils)
set_target_properties(_pyxacc PROPERTIES PREFIX "")
target_link_libraries(_pyxacc PUBLIC xacc xacc-quantum-gate xacc-pauli xacc-fermion)

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

file(GLOB PYDECORATORS benchmark/chemistry/*.py benchmark/qpt/*.py
                       plugins/aer/*.py
                       plugins/rbm_classification/*.py
                       plugins/dwave/*.py
                       plugins/cirq/*.py
                       plugins/qiskit/*.py
                       plugins/observables/*.py
                       plugins/mitiq/*.py
                       plugins/pyzx/*.py
                       plugins/optimizers/*.py)
install(FILES ${PYDECORATORS} DESTINATION ${CMAKE_INSTALL_PREFIX}/py-plugins)
