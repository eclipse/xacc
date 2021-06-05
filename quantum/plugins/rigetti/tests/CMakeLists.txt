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
include_directories(${CMAKE_SOURCE_DIR}/quantum/plugins/rigetti)
#add_xacc_test(QVMAccelerator)
#target_link_libraries(QVMAcceleratorTester xacc-quantum-gate CppMicroServices)

# add_xacc_test(QuilCompiler)
# target_link_libraries(QuilCompilerTester xacc-quantum-gate CppMicroServices)

add_xacc_test(QuilVisitor)
target_link_libraries(QuilVisitorTester xacc-quantum-gate)
