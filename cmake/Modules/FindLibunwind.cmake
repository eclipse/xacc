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
# - Try to find Libunwind
# Input variables:
#  LIBUNWIND_ROOT_DIR     - The libunwind install directory;
#                           if not set the LIBUNWIND_DIR environment variable will be used
#  LIBUNWIND_INCLUDE_DIR  - The libunwind include directory
#  LIBUNWIND_LIBRARY      - The libunwind library directory
# Output variables:
#  LIBUNWIND_FOUND        - System has libunwind
#  LIBUNWIND_INCLUDE_DIRS - The libunwind include directories
#  LIBUNWIND_LIBRARIES    - The libraries needed to use libunwind
#  LIBUNWIND_VERSION      - The version string for libunwind

include(FindPackageHandleStandardArgs)

if(NOT DEFINED LIBUNWIND_FOUND)

  # if not set already, set LIBUNWIND_ROOT_DIR from environment
  if (DEFINED ENV{LIBUNWIND_DIR} AND NOT DEFINED LIBUNWIND_ROOT_DIR)
    set(LIBUNWIND_ROOT_DIR $ENV{LIBUNWIND_DIR})
  endif()

  # Set default sarch paths for libunwind
  if(LIBUNWIND_ROOT_DIR)
    set(LIBUNWIND_INCLUDE_DIR ${LIBUNWIND_ROOT_DIR}/include CACHE PATH "The include directory for libunwind")
    if(CMAKE_SIZEOF_VOID_P EQUAL 8 AND CMAKE_SYSTEM_NAME STREQUAL "Linux")
      set(LIBUNWIND_LIBRARY ${LIBUNWIND_ROOT_DIR}/lib64;${LIBUNWIND_ROOT_DIR}/lib CACHE PATH "The library directory for libunwind")
    else()
      set(LIBUNWIND_LIBRARY ${LIBUNWIND_ROOT_DIR}/lib CACHE PATH "The library directory for libunwind")
    endif()
  endif()

  find_path(LIBUNWIND_INCLUDE_DIRS NAMES libunwind.h
      HINTS ${LIBUNWIND_INCLUDE_DIR})

  find_library(LIBUNWIND_LIBRARIES unwind
      HINTS ${LIBUNWIND_LIBRARY})
  find_library(LIBUNWINDX86_LIBRARIES unwind-x86_64
      HINTS ${LIBUNWIND_LIBRARY})

  # Get libunwind version
  if(EXISTS "${LIBUNWIND_INCLUDE_DIRS}/libunwind-common.h")
    file(READ "${LIBUNWIND_INCLUDE_DIRS}/libunwind-common.h" _libunwind_version_header)
    string(REGEX REPLACE ".*define[ \t]+UNW_VERSION_MAJOR[ \t]+([0-9]+).*" "\\1"
        LIBUNWIND_MAJOR_VERSION "${_libunwind_version_header}")
    string(REGEX REPLACE ".*define[ \t]+UNW_VERSION_MINOR[ \t]+([0-9]+).*" "\\1"
        LIBUNWIND_MINOR_VERSION "${_libunwind_version_header}")
    string(REGEX REPLACE ".*define[ \t]+UNW_VERSION_EXTRA[ \t]+([0-9]+).*" "\\1"
        LIBUNWIND_MICRO_VERSION "${_libunwind_version_header}")
    set(LIBUNWIND_VERSION "${LIBUNWIND_MAJOR_VERSION}.${LIBUNWIND_MINOR_VERSION}.${LIBUNWIND_MICRO_VERSION}")
    unset(_libunwind_version_header)
  endif()

  # handle the QUIETLY and REQUIRED arguments and set LIBUNWIND_FOUND to TRUE
  # if all listed variables are TRUE
  find_package_handle_standard_args(Libunwind
      FOUND_VAR LIBUNWIND_FOUND
      VERSION_VAR LIBUNWIND_VERSION
      REQUIRED_VARS LIBUNWIND_LIBRARIES LIBUNWIND_INCLUDE_DIRS)

  mark_as_advanced(LIBUNWIND_INCLUDE_DIR LIBUNWIND_LIBRARY LIBUNWINDX86_LIBRARIES
      LIBUNWIND_INCLUDE_DIRS LIBUNWIND_LIBRARIES)

endif()
