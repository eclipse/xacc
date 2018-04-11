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
set (PACKAGE_NAME "XACC Gate Model Quantum Computing Runtime")
set (PACKAGE_DESCIPTION "Runtime library for gate model quantum computing")

set (LIBRARY_NAME xacc-quantum-gate)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/utils)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ir)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/compiler)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ir/instructions)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ir/algorithms)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ir/transformations)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/accelerator)

file (GLOB_RECURSE HEADERS *.hpp)
file (GLOB SRC *.cpp accelerator/*.cpp compiler/*.cpp ir/*.cpp ir/instructions/*.cpp utils/*.cpp ir/algorithms/*.cpp ir/transformations/*.cpp)

# Set up dependencies to resources to track changes
usFunctionGetResourceSource(TARGET ${LIBRARY_NAME} OUT SRC)
# Generate bundle initialization code
usFunctionGenerateBundleInit(TARGET ${LIBRARY_NAME} OUT SRC)

add_library(${LIBRARY_NAME} SHARED ${SRC})

set(_bundle_name xacc_quantum_gate)
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
  
link_directories(${Boost_LIBRARY_DIR})

set(CppUsLib CppMicroServicesd)
link_directories(${CPPUS_LIBRARY_DIR})
find_library(cppus_has_d_suffix CppMicroServicesd)
if (NOT cppus_has_d_suffix)
	set(CppUsLib CppMicroServices)
endif()
target_link_libraries(${LIBRARY_NAME} ${Boost_LIBRARIES} xacc ${CppUsLib} restclient-cpp) #cpprest ${OPENSSL_LIBRARIES})

add_subdirectory(compiler)
add_subdirectory(ir)
add_subdirectory(utils)
add_subdirectory(accelerator)

install(FILES ${HEADERS} DESTINATION include/quantum/gate)
install(TARGETS ${LIBRARY_NAME} DESTINATION lib)
