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

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules")

if (NOT LLVM_ROOT) 
   # By default, users should install scaffold package from 
   # us, so it will be in this location
   set (LLVM_ROOT /usr/local/scaffold/build/Release+Asserts)  
endif()

find_package(ScaffoldClang)

if (CLANG_FOUND AND LLVM_FOUND)

   set (PACKAGE_NAME "Scaffold XACC Runtime")
   set (PACKAGE_DESCIPTION "Scaffold XACC Programming Framework")
   set (LIBRARY_NAME xacc-scaffold)

   file (GLOB HEADERS *.hpp)
   file (GLOB SRC *.cpp)

   set(CMAKE_CXX_FLAGS "-D_DEBUG -D_GNU_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS -O3 -w -fomit-frame-pointer -fvisibility-inlines-hidden -fPIC -Woverloaded-virtual -Wcast-qual -rdynamic")

   include_directories(${CLANG_INCLUDE_DIRS})
   include_directories(${CLANG_INCLUDE_DIRS}/extra-tools)
   include_directories(${CMAKE_SOURCE_DIR}/quantum/gate)
   include_directories(${CMAKE_SOURCE_DIR}/quantum/gate/ir)
   include_directories(${CMAKE_SOURCE_DIR}/quantum/gate/compiler)
   include_directories(${CMAKE_SOURCE_DIR}/quantum/gate/utils)
   include_directories(${CMAKE_SOURCE_DIR}/tpls/rapidjson/include)
   include_directories(${CMAKE_SOURCE_DIR}/quantum/gate/ir/instructions)

   # Set up dependencies to resources to track changes
   usFunctionGetResourceSource(TARGET ${LIBRARY_NAME} OUT SRC)
   # Generate bundle initialization code
   usFunctionGenerateBundleInit(TARGET ${LIBRARY_NAME} OUT SRC)


   add_library(${LIBRARY_NAME} SHARED ${SRC})

set(_bundle_name xacc_scaffold)

set_target_properties(${LIBRARY_NAME} PROPERTIES
  # This is required for every bundle
  COMPILE_DEFINITIONS US_BUNDLE_NAME=${_bundle_name}
  # This is for convenience, used by other CMake functions
  US_BUNDLE_NAME ${_bundle_name}
  )

if(BUILD_SHARED_LIBS)
  set_target_properties(${LIBRARY_NAME} PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN 1
    )
endif()

# Embed meta-data from a manifest.json file
usFunctionEmbedResources(TARGET ${LIBRARY_NAME}
  WORKING_DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}
  FILES
    manifest.json
  )


   link_directories(${CLANG_LIBRARY_DIRS} ${LLVM_LIBRARY_DIRS})

   target_link_libraries(${LIBRARY_NAME} xacc-quantum-gate CppMicroServices ${CLANG_LIBS} ${LLVM_LIBS} ${Boost_LIBRARIES})

   install(TARGETS ${LIBRARY_NAME} DESTINATION lib/plugins/compilers/)

   # Gather tests
   file (GLOB test_files tests/*.cpp)
   add_tests("${test_files}" "${CMAKE_CURRENT_SOURCE_DIR}" "${LIBRARY_NAME};${CLANG_LIBS};${LLVM_LIBS};dl;pthread")
endif()