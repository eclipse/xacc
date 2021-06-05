# *******************************************************************************
# Copyright (c) 2017 UT-Battelle, LLC.
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
add_xacc_test(XACCAPI xacc)
target_include_directories(XACCAPITester PRIVATE ${CMAKE_BINARY_DIR})
add_xacc_test(CLIParser xacc)
add_xacc_test(Algorithm xacc)

add_xacc_test(Heterogeneous xacc)
target_compile_features(HeterogeneousTester PRIVATE cxx_std_14)

# configure_file(InternalCompilerTester.in.cpp
#                ${CMAKE_BINARY_DIR}/xacc/tests/InternalCompilerTester.cpp)

# add_executable(InternalCompilerTester ${CMAKE_BINARY_DIR}/xacc/tests/InternalCompilerTester.cpp)
# target_include_directories(InternalCompilerTester PRIVATE ${GTEST_INCLUDE_DIRS})
# target_link_libraries(InternalCompilerTester PRIVATE xacc ${GTEST_LIBRARIES})
# add_test(NAME xacc_InternalCompilerTester COMMAND InternalCompilerTester)
# target_compile_features(InternalCompilerTester PRIVATE cxx_std_14)

#add_xacc_test(InternalCompiler xacc)