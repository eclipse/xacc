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
add_subdirectory(ibm)
add_subdirectory(algorithms)
add_subdirectory(xasm)
add_subdirectory(qpp)
add_subdirectory(staq)
if(XACC_BUILD_CUTENSORNET)
    add_subdirectory(cutensornet)
endif()

if(XACC_REMOTE_ACCELERATORS)
    add_subdirectory(ionq)
    add_subdirectory(quantinuum)
endif()

if(NOT QIREE_MINIMAL_BUILD)
    add_subdirectory(placement)
    if(XACC_BUILD_IONTRAP)
        add_subdirectory(iontrap)
    endif()
    add_subdirectory(circuits)
    add_subdirectory(optimizers)
    add_subdirectory(circuit_optimizers)
    add_subdirectory(decorators)
    add_subdirectory(observable_transforms)
    add_subdirectory(optimal_control)
    add_subdirectory(qsim)
    add_subdirectory(noise_model)
    add_subdirectory(rigetti)

    if(XACC_REMOTE_ACCELERATORS)
        add_subdirectory(atos_qlm)
    endif()

    find_library(QRACK_LIBRARY NAMES qrack)
    if (QRACK_LIBRARY)
        message("-- Found Qrack library (find_library(QRACK_LIBRARY NAMES qrack))")
        add_subdirectory(qrack)
    else()
        message("-- Could NOT find Qrack library (missing: find_library(QRACK_LIBRARY NAMES qrack))")
    endif()
endif()

if(XACC_BUILD_ANNEALING)
    add_subdirectory(dwave)
endif()

install (FILES utils/OperatorPool.hpp DESTINATION include/quantum/gate)
