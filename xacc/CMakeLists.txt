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

# Create a xacc config header file
configure_file("${CMAKE_SOURCE_DIR}/cmake/xacc_config.hpp.in" "${CMAKE_BINARY_DIR}/xacc_config.hpp")
install(FILES "${CMAKE_BINARY_DIR}/xacc_config.hpp" DESTINATION include/xacc)

include_directories(${CMAKE_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ir)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/compiler)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/program)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/accelerator)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/utils)

file (GLOB HEADERS 
		XACC.hpp 
		ir/*.hpp 
		program/*.hpp 
		compiler/*.hpp 
		accelerator/*.hpp 
		accelerator/remote/*.hpp 
		utils/*.hpp
	)
		
file (GLOB SRCS 
		*.cpp 
		program/*.cpp 
		utils/*.cpp 
		accelerator/remote/*.cpp
	)

#link_directories(${CMAKE_INSTALL_PREFIX}/lib)

add_library(xacc SHARED ${SRCS})
add_dependencies(xacc Boost::system Boost::filesystem Boost::graph cpr)

if(APPLE)
   set_target_properties(xacc PROPERTIES INSTALL_RPATH "@loader_path")
   set_target_properties(xacc PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
else()
   set_target_properties(xacc PROPERTIES INSTALL_RPATH "$ORIGIN")
   set_target_properties(xacc PROPERTIES LINK_FLAGS "-shared")
endif()

# Get the test files
file(GLOB test_files utils/tests/*Tester.cpp accelerator/tests/*Tester.cpp)

target_link_libraries(xacc PUBLIC CppMicroServices Boost::boost Boost::filesystem Boost::graph Boost::program_options Boost::regex PRIVATE cpr)

# Add the tests
if(XACC_BUILD_TESTS)
   add_subdirectory(accelerator/tests)
   add_subdirectory(utils/tests)
endif()

	#add_tests("${test_files}" "${CMAKE_CURRENT_SOURCE_DIR}/utils" "${Boost_LIBRARIES};xacc;${CppUsLib};boost_program_options;boost_unit_test_framework;boost_system;cpr;boost_filesystem;boost_graph")
	#add_tests("${test_files}" "${CMAKE_CURRENT_SOURCE_DIR}/utils" "${Boost_LIBRARIES};xacc;${CppUsLib};boost_program_options;boost_unit_test_framework;boost_system;cpr;boost_filesystem;boost_graph")

install(FILES ${HEADERS} DESTINATION include/xacc)
install(TARGETS xacc DESTINATION lib)

