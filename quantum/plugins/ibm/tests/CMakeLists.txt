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
include_directories(${CMAKE_SOURCE_DIR}/quantum/plugins/ibm/accelerator)
include_directories(${CMAKE_SOURCE_DIR}/quantum/plugins/ibm/compiler)
include_directories(${CMAKE_SOURCE_DIR}/quantum/gate/utils)
include_directories(${CMAKE_BINARY_DIR})

add_xacc_test(OQASMCompiler)
target_link_libraries(OQASMCompilerTester xacc-quantum-gate)

add_xacc_test(QObjectCompiler)
target_link_libraries(QObjectCompilerTester CppMicroServices)

#add_xacc_test(IBMAccelerator)
#target_link_libraries(IBMAcceleratorTester xacc-ibm)

# Enable this test if you have remote access to IBMQ
# i.e. the .ibm_config file is valid. 
# add_xacc_test(IBMPulseRemote)
# target_link_libraries(IBMPulseRemoteTester xacc-quantum-gate)
add_xacc_test(MidCircuitMeasurement)
target_link_libraries(MidCircuitMeasurementTester xacc-quantum-gate)

add_xacc_test(ResetInst)
target_link_libraries(ResetInstTester xacc-quantum-gate)