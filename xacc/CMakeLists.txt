#***********************************************************************************
# Copyright (c) 2016, UT-Battelle
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#   * Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#   * Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#   * Neither the name of the xacc nor the
#     names of its contributors may be used to endorse or promote products
#     derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# Contributors:
#   Initial API and implementation - Alex McCaskey
#
#**********************************************************************************/
set (PACKAGE_NAME "XACC Core Runtime")
set (PACKAGE_DESCIPTION "The XACC Programming Framework")
set (LIBRARY_NAME xacc)

file (GLOB HEADERS program/*.hpp compiler/*.hpp accelerator/*.hpp utils/*.hpp)
file (GLOB SRC program/*.cpp compiler/*.cpp accelerator/*.cpp)

#add_library(${LIBRARY_NAME} SHARED ${SRC})

# Get the test files
file(GLOB test_files tests/*Tester.cpp)

# Add the tests
add_tests("${test_files}" "${CMAKE_CURRENT_SOURCE_DIR}/utils;${CMAKE_CURRENT_SOURCE_DIR}/tests;${CMAKE_CURRENT_SOURCE_DIR}/compiler;${CMAKE_CURRENT_SOURCE_DIR}/program;${CMAKE_CURRENT_SOURCE_DIR}/accelerator;${CMAKE_SOURCE_DIR}/quantum/gate/accelerators/firetensoraccelerator;${CMAKE_SOURCE_DIR}/quantum/gate/accelerators;${CMAKE_SOURCE_DIR}/quantum/gate/utils;${CMAKE_SOURCE_DIR}/quantum/gate/gateqir;${CMAKE_SOURCE_DIR}/quantum/gate/gateqir/instructions;${CMAKE_SOURCE_DIR}/quantum/gate/gateqir/functions;${CMAKE_SOURCE_DIR}/tpls/fire/tensors;${CMAKE_SOURCE_DIR}/tpls/fire/tensors/impl;${CMAKE_SOURCE_DIR}/tpls/fire/tpls/eigen" "${Boost_LIBRARIES};xacc-firetensor;xacc-gateqir")


install(FILES ${HEADERS} DESTINATION include)
#install(TARGETS ${LIBRARY_NAME} DESTINATION lib)
