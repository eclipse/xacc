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
add_executable(tnqvm_hpc_virt tnqvm_hpc_virtualization.cpp)
target_link_libraries(tnqvm_hpc_virt PRIVATE xacc xacc-quantum-gate)

add_executable(parameter_shift parameter_shift.cpp)
target_link_libraries(parameter_shift PRIVATE xacc xacc-quantum-gate)

add_executable(mc_vqe mc_vqe.cpp)
target_link_libraries(mc_vqe PRIVATE xacc xacc-quantum-gate xacc-pauli)

add_executable(h2_hwe h2_hwe.cpp)
target_link_libraries(h2_hwe PRIVATE xacc xacc-quantum-gate)

configure_file(tnqvm_h4_chain_8qbit_adapt_gs.in.cpp
               ${CMAKE_BINARY_DIR}/quantum/examples/hpc_virtualization/tnqvm_h4_chain_8qbit_adapt_gs.cpp)

add_executable(h4_chain_8qbit_hpc_virt ${CMAKE_BINARY_DIR}/quantum/examples/hpc_virtualization/tnqvm_h4_chain_8qbit_adapt_gs.cpp)
target_link_libraries(h4_chain_8qbit_hpc_virt PRIVATE xacc xacc-quantum-gate)
