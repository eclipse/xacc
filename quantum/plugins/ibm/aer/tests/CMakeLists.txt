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
#   Thien Nguyen - initial API and implementation
# *******************************************************************************/

add_xacc_test(AerAccelerator)
target_link_libraries(AerAcceleratorTester xacc xacc-quantum-gate)

# Note: These tests must be disabled since the CI machine doesn't have IBMQ access.
# add_xacc_test(AerNoiseModel)
# target_link_libraries(AerNoiseModelTester xacc xacc-quantum-gate)

# add_xacc_test(AerAcceleratorPulse)
# target_link_libraries(AerAcceleratorPulseTester xacc xacc-quantum-gate)