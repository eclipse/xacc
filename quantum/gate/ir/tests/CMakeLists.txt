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
include_directories(${CMAKE_SOURCE_DIR}/xacc/ir)
add_executable(GateTester GateTester.cpp)
target_include_directories(GateTester PRIVATE ${GTEST_INCLUDE_DIRS})
target_link_libraries(GateTester PRIVATE xacc xacc-quantum-gate ${GTEST_LIBRARIES})
add_test(NAME xacc_GateTester COMMAND GateTester)
target_compile_features(GateTester PRIVATE cxx_std_14)
