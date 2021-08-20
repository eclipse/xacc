# *******************************************************************************
# Copyright (c) 2020 UT-Battelle, LLC.
# All rights reserved. This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v1.0
# and Eclipse Distribution License v.10 which accompany this distribution.
# The Eclipse Public License is available at http://www.eclipse.org/legal/epl-v10.html
# and the Eclipse Distribution License is available at
# https://eclipse.org/org/documents/edl-v10.php
#
# Contributors:
#   Thien Nguyen - initial API and implementation
# *******************************************************************************/
include_directories(${CMAKE_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR}/quantum/plugins/optimizers/pulse)
add_xacc_test(Qpe)
target_link_libraries(QpeTester xacc xacc-quantum-gate)
add_xacc_test(ControlledGate)
target_link_libraries(ControlledGateTester xacc xacc-quantum-gate)