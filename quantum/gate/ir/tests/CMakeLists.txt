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
include_directories(${CMAKE_BINARY_DIR})
add_executable(GateTester GateTester.cpp)
target_include_directories(GateTester PRIVATE ${GTEST_INCLUDE_DIRS})
target_link_libraries(GateTester PRIVATE xacc xacc-quantum-gate ${GTEST_LIBRARIES})
add_test(NAME xacc_GateTester COMMAND GateTester)
target_compile_features(GateTester PRIVATE cxx_std_14)

add_executable(PulseTester PulseTester.cpp)
target_include_directories(PulseTester PRIVATE ${GTEST_INCLUDE_DIRS})
target_link_libraries(PulseTester PRIVATE xacc ${GTEST_LIBRARIES})
add_test(NAME xacc_PulseTester COMMAND PulseTester)
target_compile_features(PulseTester PRIVATE cxx_std_14)

add_executable(IfStmtTester IfStmtTester.cpp)
target_include_directories(IfStmtTester PRIVATE ${GTEST_INCLUDE_DIRS})
target_link_libraries(IfStmtTester PRIVATE xacc ${GTEST_LIBRARIES})
add_test(NAME xacc_IfStmtTester COMMAND PulseTester)
target_compile_features(IfStmtTester PRIVATE cxx_std_14)

add_executable(PulseSchedulerTester PulseSchedulerTester.cpp)
target_include_directories(PulseSchedulerTester PRIVATE ${GTEST_INCLUDE_DIRS})
target_link_libraries(PulseSchedulerTester PRIVATE xacc ${GTEST_LIBRARIES})
add_test(NAME xacc_PulseSchedulerTester COMMAND PulseSchedulerTester)
target_compile_features(PulseSchedulerTester PRIVATE cxx_std_14)
