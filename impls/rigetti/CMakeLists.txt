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

include_directories(${CMAKE_SOURCE_DIR}/tpls/rapidjson/include)

include_directories(${CMAKE_SOURCE_DIR}/tpls/fire/tensors)
include_directories(${CMAKE_SOURCE_DIR}/tpls/fire/tensors/impl)
include_directories(${CMAKE_SOURCE_DIR}/tpls/fire/tpls/eigen)
   
include_directories(${CMAKE_SOURCE_DIR}/quantum/gate/ir)
include_directories(${CMAKE_SOURCE_DIR}/quantum/gate/utils)
include_directories(${CMAKE_SOURCE_DIR}/quantum/gate/ir/instructions)
include_directories(${CMAKE_SOURCE_DIR}/quantum/gate/ir/functions)
include_directories(${CMAKE_SOURCE_DIR}/tpls/fire/util)
include_directories(${CMAKE_SOURCE_DIR}/tpls/fire/tpls/simpleweb)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/utils)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/compiler)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/accelerator)

set (PACKAGE_NAME "Rigetti XACC")
set (PACKAGE_DESCIPTION "Rigetti XACC Plugins")
set (LIBRARY_NAME xacc-rigetti)

add_subdirectory(compiler)
add_subdirectory(accelerator)
add_subdirectory(examples)

# Gather tests
file (GLOB test_files tests/*.cpp)
add_tests("${test_files}" "${CMAKE_CURRENT_SOURCE_DIR}" "${Boost_LIBRARIES};xacc-rigetti-accelerator;xacc-rigetti-quilcompiler")
   
