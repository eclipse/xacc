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
#   Daniel Claudino - initial API and implementation
# *******************************************************************************/
include_directories(${CMAKE_BINARY_DIR})
add_xacc_test(GradientStrategies)
target_link_libraries(GradientStrategiesTester xacc xacc-quantum-gate xacc-pauli) 

# Only test QNG module if LAPACK was installed.
if(LAPACK_FOUND)
    add_xacc_test(QuantumNatualGradient)
    target_link_libraries(QuantumNatualGradientTester xacc xacc-pauli xacc-gradient-strategies) 
    target_include_directories(QuantumNatualGradientTester PRIVATE ../) 
endif()