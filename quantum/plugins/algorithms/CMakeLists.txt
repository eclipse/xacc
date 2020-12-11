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
add_subdirectory(adapt)
add_subdirectory(vqe)
add_subdirectory(rdm)
add_subdirectory(ml)
add_subdirectory(rotoselect)
add_subdirectory(qpt)
add_subdirectory(qaoa)
add_subdirectory(qpe)
add_subdirectory(gradient_strategies)
add_subdirectory(qite)
add_subdirectory(qcmx)
add_subdirectory(qeom)

file(GLOB PYDECORATORS ${CMAKE_CURRENT_SOURCE_DIR}/vqe/python/*.py
                       ${CMAKE_CURRENT_SOURCE_DIR}/ml/ddcl/python/*.py)
install(FILES ${PYDECORATORS} DESTINATION ${CMAKE_INSTALL_PREFIX}/py-plugins)
