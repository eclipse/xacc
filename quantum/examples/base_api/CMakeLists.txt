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
add_executable(bell_quil_ibm_local bell_quil_ibm_local.cpp)
target_link_libraries(bell_quil_ibm_local PRIVATE xacc)

add_executable(bell_xasm_ibm_local bell_xasm_ibm_local.cpp)
target_link_libraries(bell_xasm_ibm_local PRIVATE xacc)

add_executable(bell_xasm_ibm_select_backend bell_xasm_ibm_select_backend.cpp)
target_link_libraries(bell_xasm_ibm_select_backend PRIVATE xacc)

add_executable(deuteron_2qbit_xasm_X0X1 deuteron_2qbit_xasm_X0X1.cpp)
target_link_libraries(deuteron_2qbit_xasm_X0X1 PRIVATE xacc)

add_executable(deuteron_2qbit_ir_api_X0X1 deuteron_2qbit_ir_api_X0X1.cpp)
target_link_libraries(deuteron_2qbit_ir_api_X0X1 PRIVATE xacc)

add_executable(circuit_optimization circuit_optimization.cpp)
target_link_libraries(circuit_optimization PRIVATE xacc)

add_executable(circuit_with_classical_control circuit_with_classical_control.cpp)
target_link_libraries(circuit_with_classical_control PRIVATE xacc)

add_executable(optimal_control_goat optimal_control_goat.cpp)
target_link_libraries(optimal_control_goat PRIVATE xacc)
