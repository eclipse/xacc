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
#   Jay Jay Billings - initial tests.cmake implementation 
# *******************************************************************************/
# tests.cmake
#
# This is a CMake script with various unit testing utilities for doing boost-
# based testing. It is called by issuing the command "include(tests)" when the
# tests.cmake file is in the module path. It always attempts to find BOOST
# regardless of whether or not one of its functions is called.
#
# From the main build CMake file with this script in the module path, it is 
# sufficient to do
#
# include(tests)
# add_tests(...)
#
# This also works fine when include(tests) is called from a parent and the
# functions from this script are called from submodules.

# Find the BOOST
FIND_PACKAGE(Boost)
# Enable testing if boost is found.
if(Boost_FOUND)
  # Turn on the testing harness
  enable_testing()
  # Let the user know that tests will be built
  message(STATUS "Boost version ${Boost_VERSION} found.")
  message(STATUS "Tests will be built since Boost was found.")
  message(STATUS "Tests can be run with the 'make test' command.")
else(Boost_FOUND)
  # Complain that tests will be disabled
  message(WARNING "Tests enabled, but BOOST is unavailable!")
  message(WARNING "Tests will not be built!")
endif(Boost_FOUND)

# This function builds and configures ctest for a list of test files.
# @param tests - the list of test files to be built
# @param test_include_dirs - the list of header directories to include
# @param test_libs - the list of libraries to link for the tests
#
# This is a convenience call around add_tests_with_flags where the 
# test_cflags argument is set to the default.
#
# This function sets the working directory of the tests to the directory
# where the binary is build, which means that ctest will not execute it
# from the root of your build directory. Instead, it will be run from the
# subdirectory of the build directory for your module.
#
# When you call this function, you need to quote your variables like so:
# add_tests("${intTests}" "${test_include_dirs}" "${FIRE_LIBS}") 
function(add_tests tests test_include_dirs test_libs)
      add_tests_with_flags("${tests}" "${test_include_dirs}" "${test_libs}" "")
endfunction()

# This function builds and configures ctest for a list of test files.
# @param tests - the list of test files to be built
# @param test_include_dirs - the list of header directories to include
# @param test_libs - the list of libraries to link for the tests
# @param test_cflags - compiler flags for the tests. If this flag is 
#                      empty, the default flags are used.
#
# This function sets the working directory of the tests to the directory
# where the binary is build, which means that ctest will not execute it
# from the root of your build directory. Instead, it will be run from the
# subdirectory of the build directory for your module.
#
# When you call this function, you need to quote your variables like so:
# add_tests("${intTests}" "${test_include_dirs}" "${FIRE_LIBS}" "${flags}")
function(add_tests_with_flags tests test_include_dirs test_libs test_cflags)
    if(Boost_FOUND)
      # Include all the required headers and BOOST
      include_directories("${Boost_INCLUDE_DIR}" "${test_include_dirs}")
      # Make executables and link libraries for testers
      foreach(test ${tests})
         message(STATUS "Adding test ${test}")
         get_filename_component(testName ${test} NAME_WE)
         # Add the executable to the build and the test list
         add_executable(${testName} ${test})
         add_test(${testName} ${testName} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
         # Overwrite the default compile flags if needed.
         if (NOT test_cflags STREQUAL "")
            target_compile_options(${testName} PUBLIC "${test_cflags}")
         endif()
         # Only link libraries if they are provided
         if (test_libs)
            target_link_libraries(${testName} ${test_libs})
         endif(test_libs) 
         # Add a label so the tests can be run on its own by label
         set_property(TEST ${testName} PROPERTY LABELS ${PACKAGE_NAME})
      endforeach(test ${tests})
    else(Boost_FOUND)
      message(WARNING "Tests - ${tests} - not built because BOOST is unavailable!")
    endif()  
endfunction()