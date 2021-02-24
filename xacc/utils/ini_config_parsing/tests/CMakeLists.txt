
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
add_xacc_test(IniFileParsing)
target_link_libraries(IniFileParsingTester xacc)
target_compile_definitions(IniFileParsingTester PRIVATE INI_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/resources")
