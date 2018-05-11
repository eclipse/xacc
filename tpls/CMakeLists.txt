# *******************************************************************************
# Copyright (c) 2018 UT-Battelle, LLC.
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
set(BUILD_SHARED_LIBS TRUE)
add_subdirectory(cppmicroservices)

set(USE_SYSTEM_CURL ON CACHE BOOL "" FORCE)
set(BUILD_CPR_TESTS OFF CACHE BOOL "" FORCE)
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)
set(BUILD_SHARED_LIBS FALSE)
add_subdirectory(cpr)

set(BUILD_SHARED_LIBS TRUE)
add_subdirectory(cpr/opt/googletest)
if(APPLE)
   set_target_properties(gtest PROPERTIES INSTALL_RPATH "@loader_path")
   set_target_properties(gtest_main PROPERTIES INSTALL_RPATH "@loader_path")
else()
   set_target_properties(gtest PROPERTIES INSTALL_RPATH "$ORIGIN")
   set_target_properties(gtest_main PROPERTIES INSTALL_RPATH "$ORIGIN")
endif()

set(GTEST_FOUND TRUE)
set(GTEST_LIBRARIES gtest)
set(GTEST_MAIN_LIBRARIES gtest_main)
set(GTEST_BOTH_LIBRARIES gtest gtest_main)
set(GTEST_INCLUDE_DIRS ${gtest_SOURCE_DIR}/include)

set_property(TARGET gtest PROPERTY FOLDER "tests/gtest")
set_property(TARGET gtest_main PROPERTY FOLDER "tests/gtest")

set_cache_variable(GTEST_FOUND "Set if libgtest was found or built")
set_cache_variable(GTEST_LIBRARIES "Location of libgtest")
set_cache_variable(GTEST_MAIN_LIBRARIES "Location of libgtest-main")
set_cache_variable(GTEST_BOTH_LIBRARIES "Location of both gtest libraries")
set_cache_variable(GTEST_INCLUDE_DIRS "Location of gtest include files")

include_directories(${GTEST_INCLUDE_DIRS})
install( DIRECTORY "${CMAKE_SOURCE_DIR}/tpls/cpr/opt/googletest/include/gtest" DESTINATION include/gtest )
if (APPLE)
   install(FILES ${CMAKE_BINARY_DIR}/tpls/cpr/opt/googletest/libgtest.dylib DESTINATION lib)
   install(FILES ${CMAKE_BINARY_DIR}/tpls/cpr/opt/googletest/libgtest_main.dylib DESTINATION lib)
else()
   install(FILES ${CMAKE_BINARY_DIR}/tpls/cpr/opt/googletest/libgtest.so DESTINATION lib)
   install(FILES ${CMAKE_BINARY_DIR}/tpls/cpr/opt/googletest/libgtest_main.so DESTINATION lib)
endif()

set(BOOST_LIBS_OPTIONAL program_options system filesystem regex graph CACHE STRING "" FORCE)
add_subdirectory(boost-cmake)

INSTALL( DIRECTORY "${CMAKE_SOURCE_DIR}/tpls/boost-cmake/boost/boost_1_64_0/boost" DESTINATION include )

