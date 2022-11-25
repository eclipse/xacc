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
if(APPLE)
    # Apple Clang doesn't have this Wno-deprecated-copy flag
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DPATH_MAX=4096 -Wno-deprecated-copy")
    endif()
endif()
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "9.0.0")
     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error=deprecated-copy")
  endif()
endif()

# Disable 'deprecated-copy' warnings if the compiler recognizes the flag.
# especially for different versions of Clang/AppleClang, 
# we don't want to set the flag on compilers that don't support it yet (errors)
add_compile_flags_if_supported(-Wno-deprecated-copy)
add_subdirectory(cppmicroservices)

# Fix for bug #161
install(FILES "${CMAKE_BINARY_DIR}/tpls/cppmicroservices/CMakeFiles/Export/share/cppmicroservices4/cmake/CppMicroServicesTargets-release.cmake" DESTINATION share/cppmicroservices4/cmake OPTIONAL)

set(CPR_USE_SYSTEM_CURL ON CACHE BOOL "" FORCE)
set(CPR_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)
set(BUILD_SHARED_LIBS FALSE)
add_subdirectory(cpr)

# Googletest
set(BUILD_SHARED_LIBS TRUE)
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY "https://github.com/google/googletest"
    GIT_TAG        release-1.12.1
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

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
set(GTEST_INCLUDE_DIRS ${googletest_SOURCE_DIR}/googletest/include)

set_property(TARGET gtest PROPERTY FOLDER "tests/gtest")
set_property(TARGET gtest_main PROPERTY FOLDER "tests/gtest")

set_cache_variable(GTEST_FOUND "Set if libgtest was found or built")
set_cache_variable(GTEST_LIBRARIES "Location of libgtest")
set_cache_variable(GTEST_MAIN_LIBRARIES "Location of libgtest-main")
set_cache_variable(GTEST_BOTH_LIBRARIES "Location of both gtest libraries")
set_cache_variable(GTEST_INCLUDE_DIRS "Location of gtest include files")

install(DIRECTORY "${googletest_SOURCE_DIR}/googletest/include/gtest" DESTINATION include/gtest )

set(BUILD_SHARED_LIBS FALSE)
set(BOOST_LIBS_OPTIONAL graph CACHE STRING "" FORCE)
add_subdirectory(boost-cmake)

install(DIRECTORY staq DESTINATION ${CMAKE_INSTALL_PREFIX}/include) 

install (DIRECTORY armadillo DESTINATION ${CMAKE_INSTALL_PREFIX}/include/)
# Compiler-specific C++11 activation (FROM ANTLR4).
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU" OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "Intel")
    execute_process(
        COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
    # Just g++-5.0 and greater contain <codecvt> header. (test in ubuntu)
    if (NOT (GCC_VERSION VERSION_GREATER 5.0 OR GCC_VERSION VERSION_EQUAL 5.0))
        message(WARNING "${PROJECT_NAME} requires g++ 5.0 or greater. XACC will not build Antlr support.")
    else ()
        add_subdirectory(antlr)
    endif()
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" AND APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    add_subdirectory(antlr)
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" AND CMAKE_SYSTEM_NAME MATCHES "Linux")
    execute_process(
        COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE CLANG_VERSION)
    if (NOT (CLANG_VERSION VERSION_GREATER 4.2.1 OR CLANG_VERSION VERSION_EQUAL 4.2.1))
        message(WARNING "${PROJECT_NAME} requires clang 4.2.1 or greater. XACC will not build Antlr support.")
    else ()
        add_subdirectory(antlr)
    endif()
    # You can use libc++ to compile this project when g++ is NOT greater than or equal to 5.0.
    if (WITH_LIBCXX)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    endif()
endif ()
