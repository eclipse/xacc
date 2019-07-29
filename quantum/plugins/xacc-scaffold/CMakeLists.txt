Project(ScaffCompiler C CXX)
cmake_minimum_required(VERSION 3.13.3)
set(CMAKE_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)

option(SCAFFOLD_PATH "Path of scaffold install" "scaffcc")

find_package(XACC REQUIRED)

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set (CMAKE_INSTALL_PREFIX "${XACC_ROOT}" CACHE PATH "default install path" FORCE )
endif()

# Modify our CMAKE_MODULE_PATH
set(CppMicroServicesConfig_DIR "${XACC_ROOT}/share/cppmicroservices4/cmake")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${XACC_ROOT}/share/xacc")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CppMicroServicesConfig_DIR}")
include(CppMicroServicesConfig)

add_subdirectory(compiler)
add_subdirectory(pragmas)