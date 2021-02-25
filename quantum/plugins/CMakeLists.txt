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
add_subdirectory(rigetti)
#add_subdirectory(cmr)
add_subdirectory(dwave)
add_subdirectory(algorithms)
add_subdirectory(decorators)
add_subdirectory(circuits)
add_subdirectory(optimizers)
add_subdirectory(ionq)
add_subdirectory(placement)
#add_subdirectory(scaffold)
add_subdirectory(xasm)
add_subdirectory(qpp)
add_subdirectory(staq)
add_subdirectory(observable_transforms)
#add_subdirectory(jaqal)

find_library(QRACK_LIBRARY NAMES qrack)
if (QRACK_LIBRARY)
    message("-- Found Qrack library (find_library(QRACK_LIBRARY NAMES qrack))")
    add_subdirectory(qrack)
else()
    message("-- Could NOT find Qrack library (missing: find_library(QRACK_LIBRARY NAMES qrack))")
endif()

add_subdirectory(optimal_control)
add_subdirectory(qsim)
add_subdirectory(atos_qlm)
add_subdirectory(noise_model)

install (FILES utils/OperatorPool.hpp DESTINATION include/quantum/gate)
