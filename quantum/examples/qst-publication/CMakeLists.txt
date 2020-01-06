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
add_executable(base_api base_api.cpp)
target_link_libraries(base_api PRIVATE xacc)

add_executable(kernel_compilation kernel_compilation.cpp)
target_link_libraries(kernel_compilation PRIVATE xacc)

add_executable(hybrid_deuteron hybrid_deuteron.cpp)
target_link_libraries(hybrid_deuteron PRIVATE xacc xacc-quantum-gate)

add_executable(hybrid_ddcl hybrid_ddcl.cpp)
target_link_libraries(hybrid_ddcl PRIVATE xacc)
