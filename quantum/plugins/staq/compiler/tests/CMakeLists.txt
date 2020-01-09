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
#   Alexander J. McCaskey - initial API and implementation
# *******************************************************************************/
#add_xacc_test(StaqCompiler)
#target_link_libraries(StaqCompilerTester xacc )


configure_file(StaqCompilerTester.in.cpp
               ${CMAKE_BINARY_DIR}/quantum/plugins/staq/compiler/tests/StaqCompilerTester.cpp)

add_executable(StaqCompilerTester ${CMAKE_BINARY_DIR}/quantum/plugins/staq/compiler/tests/StaqCompilerTester.cpp)
target_include_directories(StaqCompilerTester PRIVATE ${GTEST_INCLUDE_DIRS})
target_link_libraries(StaqCompilerTester PRIVATE xacc ${GTEST_LIBRARIES})
add_test(NAME xacc_StaqCompilerTester COMMAND StaqCompilerTester)
target_compile_features(StaqCompilerTester PRIVATE cxx_std_14)