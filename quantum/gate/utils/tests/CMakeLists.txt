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
include_directories(${CMAKE_SOURCE_DIR}/quantum/gate/utils)
include_directories(${CMAKE_SOURCE_DIR}/quantum/gate/ir)
include_directories(${CMAKE_SOURCE_DIR}/quantum/gate/ir/instructions)
include_directories(${CMAKE_SOURCE_DIR}/quantum/utils)
include_directories(${CMAKE_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR}/tpls/exprtk)
include_directories(${CMAKE_SOURCE_DIR}/tpls/rapidjson/include)
include_directories(${CMAKE_SOURCE_DIR}/xacc/utils)

add_xacc_test(AllGateVisitor)
add_xacc_test(JsonVisitor)
add_xacc_test(IRToGraphVisitor)
add_xacc_test(IRUtils)
target_link_libraries(IRToGraphVisitorTester xacc-quantum-gate)
target_link_libraries(JsonVisitorTester xacc-quantum-gate Boost::graph)
target_link_libraries(AllGateVisitorTester xacc-quantum-gate Boost::graph)
target_link_libraries(IRUtilsTester xacc-quantum-gate)

