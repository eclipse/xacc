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
add_executable(deuteron_from_qasm deuteron.cpp)
target_link_libraries(deuteron_from_qasm PRIVATE xacc xacc-quantum-gate)

add_executable(deuteron_from_qasm_vqe_algo deuteron_vqe_algo.cpp)
target_link_libraries(deuteron_from_qasm_vqe_algo PRIVATE xacc xacc-quantum-gate)

add_executable(deuteronH3_from_qasm_vqe_algo deuteronH3_vqe_algo.cpp)
target_link_libraries(deuteronH3_from_qasm_vqe_algo PRIVATE xacc xacc-quantum-gate)

add_executable(ddcl_example ddcl_example.cpp)
target_link_libraries(ddcl_example PRIVATE xacc)


add_executable(bell_noisy bell_readout_py_load.cpp)
target_link_libraries(bell_noisy PRIVATE xacc)

add_executable(rotoselect rotoselect_quantum_circuit_structure_learning.cpp)
target_link_libraries(rotoselect PRIVATE xacc xacc-quantum-gate)


add_executable(nah_ucc3 nah_ucc3_psi4.cpp)
target_link_libraries(nah_ucc3 PRIVATE xacc xacc-quantum-gate)

add_executable(qaoa_qubo solve_qubo_with_qaoa.cpp)
target_link_libraries(qaoa_qubo PRIVATE xacc xacc-quantum-gate)

add_executable(quantum_phase_estimation quantum_phase_estimation.cpp)
target_link_libraries(quantum_phase_estimation PRIVATE xacc xacc-quantum-gate)
