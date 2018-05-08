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

set(BOOST_LIBS_OPTIONAL program_options system filesystem regex graph CACHE STRING "" FORCE)
add_subdirectory(boost-cmake)

INSTALL( DIRECTORY "${CMAKE_SOURCE_DIR}/tpls/boost-cmake/boost/boost_1_64_0/boost" DESTINATION include )

