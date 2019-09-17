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