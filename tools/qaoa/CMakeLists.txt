# *******************************************************************************
# Copyright (c) 2020 UT-Battelle, LLC.
# All rights reserved. This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v1.0
# and Eclipse Distribution License v.10 which accompany this distribution.
# The Eclipse Public License is available at http://www.eclipse.org/legal/epl-v10.html
# and the Eclipse Distribution License is available at
# https://eclipse.org/org/documents/edl-v10.php
#
# Contributors:
#   Anthony Santana - initial API and implementation
# *******************************************************************************/

add_subdirectory(examples)

set (LIBRARY_NAME xacc-qaoa-helper)
add_library(${LIBRARY_NAME} qaoa_from_file.cpp)
target_link_libraries(${LIBRARY_NAME} PUBLIC xacc xacc-quantum-gate xacc-pauli )

if(APPLE)
  set_target_properties(${LIBRARY_NAME}
                        PROPERTIES INSTALL_RPATH "@loader_path/../lib")
  set_target_properties(${LIBRARY_NAME}
                        PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
else()
  set_target_properties(${LIBRARY_NAME}
                        PROPERTIES INSTALL_RPATH "$ORIGIN/../lib")
  set_target_properties(${LIBRARY_NAME} PROPERTIES LINK_FLAGS "-shared")
endif()


add_executable(xacc-qaoa qaoa_from_file_utils.cpp)
target_link_libraries(xacc-qaoa PUBLIC xacc-qaoa-helper)


if(APPLE)
  set_target_properties(xacc-qaoa
                        PROPERTIES INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
else()
  set_target_properties(xacc-qaoa
                        PROPERTIES INSTALL_RPATH "$ORIGIN/../lib")
endif()

install (TARGETS xacc-qaoa DESTINATION bin)
install (TARGETS ${LIBRARY_NAME} DESTINATION lib)

if (XACC_BUILD_TESTS) 
    add_subdirectory(tests)
endif ()