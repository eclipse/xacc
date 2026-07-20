# ******************************************************************************
# Copyright (c) 2017 UT-Battelle, LLC. All rights reserved. This program and the
# accompanying materials are made available under the terms of the Eclipse
# Public License v1.0 and Eclipse Distribution License v.10 which accompany this
# distribution. The Eclipse Public License is available at
# http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution License
# is available at https://eclipse.org/org/documents/edl-v10.php
#
# Contributors: Alexander J. McCaskey - initial API and implementation
# *****************************************************************************/

set(XACC_IS_APPLE 0)
if (APPLE)
set(XACC_IS_APPLE 1)
endif()

# Create a xacc config header file
configure_file("${CMAKE_SOURCE_DIR}/cmake/xacc_config.hpp.in"
               "${CMAKE_BINARY_DIR}/xacc_config.hpp")
install(FILES "${CMAKE_BINARY_DIR}/xacc_config.hpp" DESTINATION include/xacc)
configure_file("${CMAKE_SOURCE_DIR}/xacc/xacc_version.in"
               "${CMAKE_BINARY_DIR}/xacc/xacc_version")
install(FILES "${CMAKE_BINARY_DIR}/xacc/xacc_version" DESTINATION include/xacc)

find_package(Libunwind)

file(GLOB HEADERS
  xacc.hpp
  ir/*.hpp
  compiler/Compiler.hpp
  accelerator/*.hpp
  utils/*.hpp
  service/*.hpp
  algorithm/*.hpp
  optimizer/*.hpp
)

file(GLOB SOURCES
    xacc.cpp
    accelerator/AcceleratorBuffer.cpp
    utils/Utils.cpp
    utils/CLIParser.cpp
    utils/heterogeneous.cpp
    ir/IRBuilder.cpp
    service/ServiceRegistry.cpp
    service/xacc_service.cpp)

if(XACC_REMOTE_ACCELERATORS)
  list(APPEND HEADERS accelerator/remote/RemoteAccelerator.hpp)
  list(APPEND SOURCES accelerator/remote/RemoteAccelerator.cpp)
endif()

add_library(xacc SHARED ${SOURCES})

target_include_directories(xacc
  PRIVATE
    ${CMAKE_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/tpls/rapidjson/include
    ${CMAKE_SOURCE_DIR}/tpls/cxxopts
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ir
    compiler
    program
    accelerator
    utils
    service
    algorithm
    optimizer
    ${CMAKE_SOURCE_DIR}/tpls/mpark-variant
    ${NLOHMANN_INCLUDE_DIR}
)
target_link_libraries(xacc
  PUBLIC
    CppMicroServices
    spdlog::spdlog
)

if(LIBUNWIND_FOUND)
  message(STATUS "${BoldGreen}Building xacc with libunwind support.${ColorReset}")
  target_compile_definitions(xacc PUBLIC "-DHAS_LIBUNWIND")
  target_include_directories(xacc PRIVATE ${LIBUNWIND_INCLUDE_DIRS})
  target_link_libraries(xacc
    PUBLIC
      ${LIBUNWIND_LIBRARIES}
      ${LIBUNWINDX86_LIBRARIES}
  )
endif()

if(MPI_FOUND)
  include_directories(${MPI_CXX_HEADER_DIR})
  target_link_libraries(xacc PUBLIC ${MPI_CXX_LIBRARIES})
endif()

if(XACC_REMOTE_ACCELERATORS)
  target_link_libraries(xacc PRIVATE cpr::cpr)
  target_include_directories(xacc PUBLIC accelerator/remote)
endif()

target_compile_features(xacc
                        PUBLIC
                        cxx_std_17
                        cxx_alias_templates)
if(APPLE)
  set_target_properties(xacc PROPERTIES INSTALL_RPATH "@loader_path")
  set_target_properties(xacc PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
else()
  set_target_properties(xacc PROPERTIES INSTALL_RPATH "$ORIGIN")
  set_target_properties(xacc PROPERTIES LINK_FLAGS "-shared")
endif()

# Add the tests
if(XACC_BUILD_TESTS)
  include_directories(${GTEST_INCLUDE_DIRS})
  add_subdirectory(accelerator/tests)
  add_subdirectory(ir/tests)
  add_subdirectory(tests)
endif()

install(FILES ${HEADERS} DESTINATION include/xacc)
install(TARGETS xacc DESTINATION lib)

add_subdirectory(utils/exprtk_parsing)
add_subdirectory(ir/graph-impl)
add_subdirectory(utils/ini_config_parsing)
