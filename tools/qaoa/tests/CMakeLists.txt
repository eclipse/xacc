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
#   Anthony Santana - initial API and implementation
# *******************************************************************************/

include_directories(${CMAKE_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR}/tools/qaoa)
add_xacc_test(qaoa_from_file)
target_link_libraries(qaoa_from_fileTester xacc xacc-quantum-gate xacc-pauli xacc-qaoa-helper)
