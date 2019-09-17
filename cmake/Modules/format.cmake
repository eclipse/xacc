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
find_program(CLANG_FORMAT_EXECUTABLE
    NAMES
	   clang-format-7.0
	   clang-format-mp-7.0
	   clang-format
)
if(CLANG_FORMAT_EXECUTABLE)
    message("-- Found clang-format: ${CLANG_FORMAT_EXECUTABLE}")
     add_custom_target(format
              COMMAND
                  CLANG_FORMAT_EXE=${CLANG_FORMAT_EXECUTABLE}
                  ${CMAKE_SOURCE_DIR}/scripts/check_format_cpp.sh --apply-patch
              WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
     )
else()
    message("-- clang-format not found! 'format' target not built. Install clang-format to enable formatting.")
endif()

